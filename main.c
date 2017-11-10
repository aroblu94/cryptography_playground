#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "atk_three.c"
#include "sdes_four.c"

uint8_t unexpand(uint8_t in) {
	uint8_t out = 0;

	out = (in & 0xc0) >> 2;
	out |= (in & 0x10) >> 1;
	out |= (in & 0x20) >> 3;
	out |= (in & 0x3);
	return out;
}

Queue * make_right_parts() {
	DQueue *s1_input  = new_dqueue();
	DQueue *s2_input  = new_dqueue();
	s1_input = get_s1_input_couples(s1_input, string_to_hex("01000111"), string_to_hex("011010"), string_to_hex("011011"), string_to_hex("011011"));
	s1_input = get_s1_input_couples(s1_input, string_to_hex("01001101"), string_to_hex("011010"), string_to_hex("011011"), string_to_hex("011011"));
	s1_input = get_s1_input_couples(s1_input, string_to_hex("11111101"), string_to_hex("011010"), string_to_hex("011011"), string_to_hex("011011"));
	s1_input = get_s1_input_couples(s1_input, string_to_hex("11110111"), string_to_hex("011010"), string_to_hex("011011"), string_to_hex("011011"));

	s2_input = get_s2_input_couples(s2_input, string_to_hex("01000111"), string_to_hex("011010"), string_to_hex("011011"), string_to_hex("011011"));
	s2_input = get_s2_input_couples(s2_input, string_to_hex("01001101"), string_to_hex("011010"), string_to_hex("011011"), string_to_hex("011011"));
	s2_input = get_s2_input_couples(s2_input, string_to_hex("11111101"), string_to_hex("011010"), string_to_hex("011011"), string_to_hex("011011"));
	s2_input = get_s2_input_couples(s2_input, string_to_hex("11110111"), string_to_hex("011010"), string_to_hex("011011"), string_to_hex("011011"));

	Node *n, *n1, *nl, *nr;
	Queue *l = new_queue();
	Queue *r = new_queue();
	Queue *lk = new_queue();
	Queue *rk = new_queue();

	for (DNode *dn = s1_input->head; dn != NULL; dn = dn->next) {
		if(!is_in_queue(l, dn->first))
			append_queue(l, dn->first);
		if(!is_in_queue(l, dn->second))
			append_queue(l, dn->second);
	}

	for (DNode *dn = s2_input->head; dn != NULL; dn = dn->next) {
		if(!is_in_queue(r, dn->first))
			append_queue(r, dn->first);
		if(!is_in_queue(r, dn->second))
			append_queue(r, dn->second);
	}

	// Print
	Queue *q = new_queue();
	//printf("Left 8 input bits\n");
	for (Node *n = l->head; n != NULL; n = n->next) {
		for (Node *n1 = r->head; n1 != NULL; n1 = n1->next) {
			uint8_t tmp = (n->info << 4) | n1->info;
			//print_bits(tmp,8);
			if(!(((tmp & 0x30) >> 2) ^ (tmp & 0xc)))
				append_queue(q, unexpand(tmp));
		}
	}
	return q;
}


/*
 * Differential cryptoanalysis of S-DES
 * (4 rounds DES)
 */
int main(int argc, char* argv[]) {
	char plaintext1_string[] = "011011000111";
	char plaintext2_string[] = "011011101110";
	char plaintext3_string[] = "011011111111";
	char key_string[] = "001001101";

	// TEST ######################################

	uint16_t plaintext1, plaintext2, plaintext3, cyphertext1, cyphertext2, cyphertext3, key, k1, k2;
	uint16_t plains[3];
	uint16_t cyphers[3];
	uint8_t k8b;

	key = string_to_hex(key_string);
	plains[0] = string_to_hex(plaintext1_string);
	plains[1] = string_to_hex(plaintext2_string);
	plains[2] = string_to_hex(plaintext3_string);

	cyphers[0] = encrypt_four(plains[0], key);
	cyphers[1] = encrypt_four(plains[1], key);
	cyphers[2] = encrypt_four(plains[2], key);

	// ############################################

	// Attack first round
	// we choose 011 as out_s1 and 010 as out_s2

	// 011011 ^ 011010 = 000001
	uint16_t first_round_out[3];
	first_round_out[0] = string_to_hex("000111000001");
	first_round_out[1] = string_to_hex("101110000001");
	first_round_out[2] = string_to_hex("111111000001");	

	// Perform the attack to find the 8-bit key
	k8b = attack(first_round_out, cyphers);

	// printf("\n8-BIT KEY:\n");
	// print_bits(k8b, 8);

	// Real S-DES key is 9-bit long
	// So we have just to bruteforce the most-significant-key bit (0 or 1)
	// NOTE: 
	// 	In order to get the real key we have to
	// 	shift the founded key right by 2 (mod 9)
	//	because we're doing the attack to the 3rd round key.
	// 	shift_bits does this.
	// k1 = shift_bits(0 << 8 | k8b, 9, 2);
	// k2 = shift_bits(1 << 8 | k8b, 9, 2);
	// printf("\n9-BIT KEYs (shifted by >> 2):\n");
	// print_bits(k1, 9);
	// print_bits(k2, 9);

	// Now we test the 2 keys we founded using
	// those as keys in S-DES.
	// The output has to be equal to the "real" cyphertext.
	// printf("\n*THE REAL KEY*\n");
	// if(encrypt_four(plains[0], k1) == cyphers[0])
	// 	print_bits(k1,9);
	// else if(encrypt_four(plains[0], k2) == cyphers[0])
	// 	print_bits(k2,9);
	// else {
	// 	printf("ERROR: founded key not working.\n");
	// 	return -1;
	// }

	return 0;
}