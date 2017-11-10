#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// #define DEBUGF 1
// #define FIRSTROUND  1
// #define LASTROUND   4

/* Utils */
uint16_t string_to_hex_four(char* string);
void print_bits_four(uint16_t hex, int dim);

/*S-DES functions*/
uint8_t expansion_four(uint8_t r);
/* round starts from value 1! Be careful and take precautions! */
uint8_t gen_key_four(uint16_t key, int round);
/* feistel function should include expansion invocation */
uint8_t feistel_four(uint8_t r, uint8_t k);
/* round performs a single s-des round */
uint16_t exec_round_four(uint16_t text, uint8_t key);
/* swap_left_right represents the last swap before the final output */
uint16_t swap_left_right_four(uint16_t in);

uint16_t encrypt_four(uint16_t text, uint16_t key);
uint16_t decrypt_four(uint16_t text, uint16_t key);