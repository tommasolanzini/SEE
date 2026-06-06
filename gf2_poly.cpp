/**
 * gf2_poly.cpp – BCH/CRC codec over GF(2^m)  [STATIC / NO-HEAP build]
 *
 * Versione senza allocazione dinamica, pensata per microcontrollori con
 * RAM limitata:
 *   - Tabelle GF(2^13) statiche in .bss come uint16_t (32 KB invece di
 *     64 KB allocati su heap con make_unique).
 *   - BigPoly usa un array uint64_t a dimensione fissa al posto di
 *     std::vector, quindi nessuna allocazione heap.
 *   - I piccoli array di lavoro (sindromi, lambda, posizioni errori)
 *     sono array fissi al posto di std::vector<int>.
 *
 * L'algoritmo BCH(t=2) + CRC-32 è identico alla versione originale.
 */

#include <cstdint>
#include <stdexcept>
#include "gf2_poly.h"


// =============================================================================
// Parametri di campo e dimensioni massime
// =============================================================================

static const int GF_M          = 13;            // GF(2^13)
static const int GF_FIELD_SIZE = 1 << GF_M;     // 8192

// Codeword max = 520 byte = 4160 bit -> 65 word da 64 bit.
// Margine per gli shift intermedi.
static const int BIGPOLY_MAX_WORDS = 72;

// Dimensione array di lavoro per i polinomi "piccoli" (sindromi/lambda/pos).
static const int SMALL_MAX = 24;

// =============================================================================
// BigPoly – polinomio su GF(2) a dimensione fissa
//
// Memorizzato come array little-endian di word a 64 bit:
//   w[0] = bit  0..63, w[1] = bit 64..127, ...
// Invariante: w[i] == 0 per ogni i >= n  (rappresentazione "trimmata").
// =============================================================================

struct BigPoly {
    uint64_t w[BIGPOLY_MAX_WORDS];
    int      n;   // numero di word significative (w[n-1] != 0, oppure n == 0)

    BigPoly() : n(0) {
        for (int i = 0; i < BIGPOLY_MAX_WORDS; ++i) w[i] = 0;
    }

    bool empty() const { return n == 0; }

    void trim() { while (n > 0 && w[n - 1] == 0) --n; }

    int bit_length() const {
        for (int i = n - 1; i >= 0; --i)
            if (w[i]) return i * 64 + (64 - __builtin_clzll(w[i]));
        return 0;
    }

    int bit(int pos) const {
        int wi = pos / 64, bi = pos % 64;
        if (wi >= n) return 0;
        return static_cast<int>((w[wi] >> bi) & 1);
    }

    BigPoly& operator^=(const BigPoly& o) {
        int m = (o.n > n) ? o.n : n;
        for (int i = 0; i < o.n; ++i) w[i] ^= o.w[i];
        n = m;
        trim();
        return *this;
    }
    BigPoly operator^(const BigPoly& o) const { BigPoly r = *this; r ^= o; return r; }

    // XOR di (val << shift) senza costruire un BigPoly temporaneo.
    void xor_shifted_u64(uint64_t val, int shift) {
        int wi = shift / 64, bi = shift % 64;
        int needed = wi + (bi ? 2 : 1);
        if (n < needed) n = needed;          // gli slot oltre n sono già 0
        w[wi] ^= val << bi;
        if (bi) w[wi + 1] ^= val >> (64 - bi);
        trim();
    }

    BigPoly operator<<(int sh) const {
        BigPoly res;
        if (n == 0 || sh == 0) { res = *this; return res; }
        int wi = sh / 64, bi = sh % 64;
        for (int i = 0; i < n; ++i) {
            res.w[i + wi] |= w[i] << bi;
            if (bi) res.w[i + wi + 1] |= w[i] >> (64 - bi);
        }
        res.n = n + wi + 1;
        res.trim();
        return res;
    }
    BigPoly operator>>(int sh) const {
        BigPoly res;
        if (n == 0 || sh == 0) { res = *this; return res; }
        int wi = sh / 64, bi = sh % 64;
        if (wi >= n) return res;

        for (int i = wi; i < n; ++i) {
            res.w[i - wi] |= (w[i] >> bi);
            if (bi > 0 && (i + 1) < n) {
                res.w[i - wi] |= (w[i + 1] << (64 - bi));
            }
        }
        res.n = n - wi;
        res.trim();
        return res;
    }

    bool operator==(const BigPoly& o) const {
        if (n != o.n) return false;
        for (int i = 0; i < n; ++i) if (w[i] != o.w[i]) return false;
        return true;
    }
    bool operator!=(const BigPoly& o) const { return !(*this == o); }
};

// Array intero "piccolo" a dimensione fissa (sindromi, lambda, posizioni).
struct IntArray {
    int v[SMALL_MAX];
    int len;
    IntArray() : len(0) { for (int i = 0; i < SMALL_MAX; ++i) v[i] = 0; }
};

// =============================================================================
// Tabelle GF statiche (in .bss, nessun heap)
// =============================================================================

static uint16_t exp_table[GF_FIELD_SIZE];
static uint16_t log_table[GF_FIELD_SIZE];
static int      gf_n = 0;

// =============================================================================
// Helper GF force-inlined
// =============================================================================

[[gnu::always_inline]] static inline int gf_mult(int a, int b) {
    if (__builtin_expect(a == 0 || b == 0, 0)) return 0;
    return exp_table[(log_table[a] + log_table[b]) % gf_n];
}

[[gnu::always_inline]] static inline int gf_inv(int a) {
    if (a == 0) throw std::domain_error("0 has no inverse in GF");
    return exp_table[(gf_n - log_table[a]) % gf_n];
}

// =============================================================================
// Aritmetica polinomiale su GF(2) (i polinomi piccoli restano uint64_t)
// =============================================================================

uint64_t poly_mult_gf2(uint64_t p1, uint64_t p2) {
    uint64_t result = 0;
    while (p2) {
        if (p2 & 1) result ^= p1;
        p1 <<= 1; p2 >>= 1;
    }
    return result;
}

/// Divide BigPoly per un divisore uint64_t; ritorna il resto.
BigPoly poly_div_gf2(BigPoly dividend, uint64_t divisor) {
    int div_len = 64 - __builtin_clzll(divisor);
    int dd_len  = dividend.bit_length();
    while (dd_len >= div_len) {
        dividend.xor_shifted_u64(divisor, dd_len - div_len);
        dd_len = dividend.bit_length();
    }
    return dividend;
}

// =============================================================================
// Generazione tabelle GF(2^m)
// =============================================================================

int generate_gf_tables(int m, int prim_poly = -1) {
    if (prim_poly == -1) {
        switch (m) {
            case 3:  prim_poly = 0b1011;   break;
            case 4:  prim_poly = 0x13;     break;
            case 5:  prim_poly = 0b100101; break;
            case 8:  prim_poly = 0x11D;    break;
            case 13: prim_poly = 0x201B;   break;
            default: throw std::invalid_argument("No primitive polynomial for this m");
        }
    }
    int field_size = 1 << m;
    gf_n = field_size - 1;
    int x = 1;
    for (int i = 0; i < gf_n; ++i) {
        exp_table[i] = static_cast<uint16_t>(x);
        log_table[x] = static_cast<uint16_t>(i);
        x <<= 1;
        if (x & field_size) x ^= prim_poly;
    }
    exp_table[gf_n] = 1;
    return field_size;
}

// =============================================================================
// Polinomio generatore BCH
// =============================================================================

uint64_t get_g_polynomial(int m, int t = 2) {
    uint64_t p1 = 0, p2 = 0;
    switch (m) {
        case 3:  p1 = 0b1011;   p2 = 0b1101;   break;
        case 4:  p1 = 0b10011;  p2 = 0b11111;  break;
        case 5:  p1 = 0b100101; p2 = 0b111101; break;
        case 13: p1 = 0x201B;   p2 = 0x26B1;   break;
        default: throw std::invalid_argument("No data for this m");
    }
    if (t == 1) return p1;
    if (t == 2) return poly_mult_gf2(p1, p2);
    throw std::invalid_argument("Only t=1 or t=2 supported.");
}

// =============================================================================
// Codifica BCH sistematica
// =============================================================================

BigPoly encoding(const BigPoly& word, uint64_t g) {
    int     parity_bits = 64 - __builtin_clzll(g) - 1;   // grado di g
    BigPoly word_s      = word << parity_bits;
    BigPoly remainder   = poly_div_gf2(word_s, g);
    return word_s ^ remainder;
}

// =============================================================================
// Calcolo sindromi
// =============================================================================

/// Valutazione di Horner di BigPoly in alpha^root_idx.
int calculate_horner(const BigPoly& msg, int root_idx) {
    int root = exp_table[root_idx];
    int s    = 0;
    int len  = msg.bit_length();
    for (int bp = len - 1; bp >= 0; --bp)
        s = gf_mult(s, root) ^ msg.bit(bp);
    return s;
}

IntArray calculate_syndromes(const BigPoly& msg, int t) {
    IntArray r;
    int s1 = calculate_horner(msg, 1);
    int s2 = gf_mult(s1, s1);
    r.v[0] = s1; r.v[1] = s2;
    if (t == 1) { r.len = 2; return r; }
    int s3 = calculate_horner(msg, 3);
    int s4 = gf_mult(s2, s2);
    r.v[2] = s3; r.v[3] = s4; r.len = 4;
    return r;
}

// =============================================================================
// Berlekamp-Massey (opera su elementi GF, niente BigPoly)
// =============================================================================

IntArray bch_berlekamp_massey(const IntArray& S, int t = 2) {
    int lam[SMALL_MAX] = {0};
    int b[SMALL_MAX]   = {0};
    int T[SMALL_MAX]   = {0};
    int lamLen = 1, bLen = 1, TLen = 0;
    lam[0] = 1;
    b[0]   = 1;
    int b_offset = 0, L = 0;

    for (int k = 1; k <= 2 * t; ++k) {
        int d = S.v[k - 1];
        for (int i = 1; i <= L; ++i)
            if (i < lamLen)
                d ^= gf_mult(lam[i], S.v[k - 1 - i]);
        ++b_offset;
        if (d != 0) {
            for (int i = 0; i < lamLen; ++i) T[i] = lam[i];
            TLen = lamLen;
            int needed = bLen + b_offset;
            if (needed > lamLen) {
                for (int i = lamLen; i < needed; ++i) lam[i] = 0;
                lamLen = needed;
            }
            for (int i = 0; i < bLen; ++i)
                lam[i + b_offset] ^= gf_mult(d, b[i]);
            if (2 * L < k) {
                L = k - L;
                int d_inv = gf_inv(d);
                bLen = TLen;
                for (int i = 0; i < TLen; ++i) b[i] = gf_mult(T[i], d_inv);
                b_offset = 0;
            }
        }
    }

    IntArray out;
    for (int i = 0; i < t + 1; ++i) out.v[i] = (i < lamLen) ? lam[i] : 0;
    out.len = t + 1;
    return out;
}

// =============================================================================
// Chien Search (opera su elementi GF, niente BigPoly)
// =============================================================================

IntArray bch_chien_search(const IntArray& lambda, int message_length, int m = GF_M) {
    IntArray out;
    const int n  = (1 << m) - 1;
    int alpha_inv_1 = exp_table[n - 1];
    int alpha_inv_2 = exp_table[n - 2];
    int term1 = lambda.v[1];
    int term2 = lambda.v[2];
    for (int i = 0; i < message_length; ++i) {
        if ((1 ^ term1 ^ term2) == 0) {
            if (out.len < SMALL_MAX) out.v[out.len++] = i;
        }
        term1 = gf_mult(term1, alpha_inv_1);
        term2 = gf_mult(term2, alpha_inv_2);
    }
    return out;
}

// =============================================================================
// CRC-32 (chiave = 0xBADB17CC, 32 bit di parità appesi)
// =============================================================================

BigPoly CRC_encoding(const BigPoly& word) {
    const uint64_t key = 0xBADB17CC;
    BigPoly word_s = word << 32;
    return word_s ^ poly_div_gf2(word_s, key);
}

bool CRC_check(const BigPoly& received) {
    const uint64_t key = 0xBADB17CC;
    return poly_div_gf2(received, key).empty();
}

// =============================================================================
// Helper bit-flip su BigPoly
// =============================================================================

void flip_bit(BigPoly& p, int pos) {
    int wi = pos / 64, bi = pos % 64;
    if (wi >= p.n) p.n = wi + 1;             // gli slot oltre n sono già 0
    p.w[wi] ^= static_cast<uint64_t>(1) << bi;
    p.trim();
}

/* * -----------------------------------------------------------------
 * IMPLEMENTAZIONE DEI WRAPPER C-CALLABLE
 * -----------------------------------------------------------------
 */

void gf2_initialize(void) {
    // Genera le tabelle del campo di Galois per m=13
    generate_gf_tables(GF_M);
}

void gf2_encode_data(uint8_t* input_data, int input_length, uint8_t* output_codeword) {
    // Impacchetta il byte array in un BigPoly
    BigPoly input_poly;
    int num_words = (input_length + 7) / 8;
    for (int i = 0; i < input_length; ++i) {
        int word_idx = i / 8;
        int byte_idx = i % 8;
        input_poly.w[word_idx] |= (static_cast<uint64_t>(input_data[i]) << (byte_idx * 8));
    }
    input_poly.n = num_words;
    input_poly.trim();

    // Codifica
    int m = GF_M;
    uint64_t g = get_g_polynomial(m);
    BigPoly word_crc = CRC_encoding(input_poly);
    BigPoly codeword = encoding(word_crc, g);

    // Spacchetta il codeword nel byte array di uscita
    int total_bytes = (codeword.bit_length() + 7) / 8;
    for (int i = 0; i < total_bytes; ++i) {
        int word_idx = i / 8;
        int byte_idx = i % 8;
        if (word_idx < codeword.n) {
            output_codeword[i] = (codeword.w[word_idx] >> (byte_idx * 8)) & 0xFF;
        } else {
            output_codeword[i] = 0;
        }
    }
}

int gf2_correct_errors(uint8_t* data, int length, uint8_t* crc_valid) {
    // Impacchetta il byte array corrotto in un BigPoly
    BigPoly corrupted;
    int num_words = (length + 7) / 8;
    for (int i = 0; i < length; ++i) {
        int word_idx = i / 8;
        int byte_idx = i % 8;
        corrupted.w[word_idx] |= (static_cast<uint64_t>(data[i]) << (byte_idx * 8));
    }
    corrupted.n = num_words;
    corrupted.trim();

    // Trova e corregge gli errori
    int m = GF_M;
    int cw_len = corrupted.bit_length();
    IntArray syndromes = calculate_syndromes(corrupted, 2);
    IntArray lambda    = bch_berlekamp_massey(syndromes, 2);
    IntArray error_pos = bch_chien_search(lambda, cw_len, m);

    // Inverte i bit errati
    for (int idx = 0; idx < error_pos.len; ++idx) {
        flip_bit(corrupted, error_pos.v[idx]);
    }

    // Spacchetta il BigPoly corretto nel C array
    for (int i = 0; i < length; ++i) {
        int word_idx = i / 8;
        int byte_idx = i % 8;
        if (word_idx < corrupted.n) {
            data[i] = (corrupted.w[word_idx] >> (byte_idx * 8)) & 0xFF;
        } else {
            data[i] = 0;
        }
    }

    // CRC sulla parola appena corretta
    BigPoly data_for_crc = corrupted >> 26;
    *crc_valid = CRC_check(data_for_crc) ? 1 : 0;

    // Se il numero di posizioni trovate != grado di lambda, il BCH ha fallito.
    int lambda_degree = lambda.len - 1;
    while (lambda_degree > 0 && lambda.v[lambda_degree] == 0) lambda_degree--;

    if (error_pos.len != lambda_degree) {
        return -1; // Fallimento decodifica BCH
    }

    return error_pos.len;
}