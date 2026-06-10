#ifndef GF2_POLY_H
#define GF2_POLY_H

/* 
 * If this header is included by a C++ file, tell the compiler 
 * to use C linkage for everything inside this block.
 */
#ifdef __cplusplus
extern "C" {
#endif

/* 
 * Declare the functions you want to call from main.c here.
 * IMPORTANT: You can only use C-compatible data types here. 
 * Do not put C++ classes, references, or templates in these parameters.
 */
void gf2_initialize(void);
// Modifica la dichiarazione così:
int gf2_correct_errors(uint8_t* data, int length, uint8_t* crc_valid);

void gf2_encode_data(uint8_t* input_data, int input_length, uint8_t* output_codeword);

/*
 * Estrae il payload (allineato al byte) da un codeword sistematico.
 * Il payload NON occupa i primi byte del codeword: e' spostato in alto di
 * (32 + grado(g)) bit. Questa funzione annulla lo shift.
 */
void gf2_extract_payload(uint8_t* codeword, int codeword_length,
                         uint8_t* payload_out, int payload_length);

#ifdef __cplusplus
}
#endif

#endif /* GF2_POLY_H */