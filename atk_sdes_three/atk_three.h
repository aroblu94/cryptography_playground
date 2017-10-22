#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "utils.c"

// #define ATKDEBUG 1

uint16_t shift_bits(uint16_t in, int len, int shift);

uint8_t get_sbox_input(uint8_t l4a, uint8_t l4b);
uint8_t get_sbox_output(uint8_t l1a, uint8_t l1b, uint8_t r4a, uint8_t r4b);

DQueue * get_s1_input_couples(DQueue *dq, uint8_t in, uint8_t out, uint8_t l4a, uint8_t l4b);
DQueue * get_s2_input_couples(DQueue *dq, uint8_t in, uint8_t out, uint8_t l4a, uint8_t l4b);

Queue * get_pkeys(Queue *q, DQueue *dql, DQueue *dqr, uint8_t l4);

Queue * single_attack(Queue *k8b, uint16_t p1, uint16_t p2, uint16_t c1, uint16_t c2);

uint8_t attack(uint16_t* plains, uint16_t* cryptos);