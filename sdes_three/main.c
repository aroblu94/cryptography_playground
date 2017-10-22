#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "sdes_three.c"

/*
 * Please modify FIRSTROUND in sdes.h to 2
 * in order to run this
 */
int main(int argc, char *argv[]) {
	char plaintext_string[] = "000111011011";
	char key_string[] = "001001101";
	uint16_t cryptotext, plaintext, key, res;

	plaintext = string_to_hex(plaintext_string);
	key = string_to_hex(key_string);

	print_bits(plaintext, 12);
	print_bits(key, 9);

	cryptotext = encrypt(plaintext, key);
	res = decrypt(cryptotext, key);
	printf("CRYPTOTEXT:\n");
	print_bits(cryptotext,12);
	printf("\nPLAINTEXT:\n");
	print_bits(res,12);

	return 0;
}