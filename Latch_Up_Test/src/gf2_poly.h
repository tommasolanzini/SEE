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

#ifdef __cplusplus
}
#endif

#endif /* GF2_POLY_H */