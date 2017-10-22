#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "sdes_four.c"

/*
 * Please modify FIRSTROUND in sdes.h to 1
 * in order to run this
 */
int main(int argc, char *argv[]) {
	char plaintext_string[] = "100010110101";
	char key_string[] = "111000111";
	uint16_t cryptotext, plaintext, key, res;

	plaintext = string_to_hex(plaintext_string);
	key = string_to_hex(key_string);

	print_bits(plaintext, 12);
	print_bits(key, 9);

	cryptotext = encrypt(plaintext, key);
	res = decrypt(cryptotext, key);
	printf("PLAINTEXT:\n");
	print_bits(res,12);

	return 0;
}