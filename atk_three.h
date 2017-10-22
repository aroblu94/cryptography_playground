#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define ATKDEBUG 1

uint8_t get_8bit_key();
uint16_t attack(uint16_t p[], uint16_t c[]);