#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#ifndef __SDES_H__
#define __SDES_H__

#define FIRSTROUND  1
#define LASTROUND   4

/* Utils */
uint16_t string_to_hex(char* string);
void print_bits (uint16_t hex, int dim);

/*S-DES functions*/
uint8_t expansion(uint8_t r);
/* round starts from value 1! Be careful and take precautions! */
uint8_t gen_key(uint16_t key, int round);
/* feistel function should include expansion invocation */
uint8_t feistel(uint8_t r, uint8_t k);
/* round performs a single s-des round */
uint16_t exec_round(uint16_t text, uint8_t key);
/* swap_left_right represents the last swap before the final output */
uint16_t swap_left_right(uint16_t in);

uint16_t encrypt(const uint16_t text, const uint16_t key);
uint16_t decrypt(const uint16_t text, const uint16_t key);

#endif
