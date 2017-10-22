#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "atk_three.c"

/*
 * Cryptanalysis of S-DES
 * (3 rounds DES)
 */
int main(int argc, char* argv[]) {
	char plaintext1_string[] = "000111011011";
	char plaintext2_string[] = "101110011011";
	char plaintext3_string[] = "111111011011";
	char key_string[] = "001001101";

	uint16_t plaintext1, plaintext2, plaintext3, cyphertext1, cyphertext2, cyphertext3, key, k1, k2;
	uint16_t plains[3];
	uint16_t cyphers[3];
	uint8_t k8b;

	key = string_to_hex(key_string);
	plains[0] = string_to_hex(plaintext1_string);
	plains[1] = string_to_hex(plaintext2_string);
	plains[2] = string_to_hex(plaintext3_string);

	cyphers[0] = encrypt(plains[0], key);
	cyphers[1] = encrypt(plains[1], key);
	cyphers[2] = encrypt(plains[2], key);

	// Perform the attack to find the 8-bit key
	k8b = attack(plains, cyphers);
	printf("\n8-BIT KEY:\n");
	print_bits(k8b, 8);

	// Real S-DES key is 9-bit long
	// So we have just to bruteforce the most-significant-key bit (0 or 1)
	// NOTE: 
	// 	In order to get the real key we have to
	// 	shift the founded key right by 2 (mod 9).
	// 	shift_bits does this.
	// 	DUNNO WHY WE HAVE TO DO THAT.
	k1 = shift_bits(0 << 8 | k8b, 9, 2);
	k2 = shift_bits(1 << 8 | k8b, 9, 2);
	printf("\n9-BIT KEYs (shifted by >> 2):\n");
	print_bits(k1, 9);
	print_bits(k2, 9);

	// Now we test the 2 keys we founded using
	// those as keys in S-DES.
	// The output has to be equal to the "real" cyphertext.
	printf("\n*THE REAL KEY*\n");
	if(encrypt(plains[0], k1) == cyphers[0])
		print_bits(k1,9);
	else if(encrypt(plains[0], k2) == cyphers[0])
		print_bits(k2,9);
	else {
		printf("ERROR: founded key not working.\n");
		return -1;
	}
	return 0;
}