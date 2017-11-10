#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "atk_three.h"
#include "sdes_three.c"

/*
 * Shift n mod m
 * Just an utility function.
 */
uint16_t shift_bits(uint16_t in, int len, int shift) {
	uint16_t lenmask = 0;
	uint16_t shiftmask = 0;

	in = in >> shift;

	for (int i = 0; i < len; ++i)
		lenmask |= (1 << i);

	for (int i = 0; i < shift; ++i)
		shiftmask |= (1 << i) << len;

	return (in & lenmask) | ((in & shiftmask) >> len);
}

/*
 * Get data in A state.
 * Note that result is based
 * on differences between 2 plaintexts
 */
uint8_t get_sbox_input(uint8_t l4a, uint8_t l4b) {
	return expansion((l4a ^ l4b) & 0x3f);
}

/*
 * Get data in B state.
 * Note that result is based
 * on differences between 2 plaintextes
 * and cryptotextes
 */
uint8_t get_sbox_output(uint8_t l1a, uint8_t l1b, uint8_t r4a, uint8_t r4b) {
	return r4a ^ r4b ^ ((l1a ^ l1b) & 0x3f);
}

/*
 * Get S1 input couples.
 * Bruteforcing left part of sbox input
 * (O1 ^ k) ^ (O2 ^ k) = left part of sbox input
 * working on 4 bits -> 2^4 = 16 possibilities.
 *
 * It gives back a list of DNode(s).
 * A DNode store a couple of uint8_t
 * => all the possible S1 input couples
 */
DQueue * get_s1_input_couples(DQueue *dq, uint8_t in, uint8_t out, uint8_t l4a, uint8_t l4b) {
	in = (in & 0xf0) >> 4;
	out = (out & 0x30) >> 3;

	for(int i=0; i<16; i++) {
		// IDEA: 
		// 	i ^ O2 = left part sbox input
		uint8_t O2 = i ^ in;

		uint8_t s1_1 = S1[(i & 0x08) >> 3][i & 0x7];
		uint8_t s1_2 = S1[(O2 & 0x08) >> 3][O2 & 0x7];

		// We store only couples that gives
		// the correct output.
		if((s1_1 ^ s1_2) == out)
			append_dqueue(dq, i, O2);
	}

	#ifdef ATKDEBUG
	printf("\n######### [get_s1_input_couples()] #########\n");
	DNode *n = dq->head;
	while(n != NULL) {
		print_bits(n->first, 4);
		print_bits(n->second, 4);
		n = n->next;
	}
	#endif

	return dq;
}

/*
 * Get S2 input couples
 * Bruteforcing right part of sbox input
 * (O1 ^ k) ^ (O2 ^ k) = right part of sbox input
 * working on 4 bits -> 2^4 = 16 possibilities
 *
 * It gives back a list of DNode(s).
 * A DNode store a couple of uint8_t
 * => all the possible S1 input couples
 *
 * (Same as previous function, but for right part)
 */
DQueue * get_s2_input_couples(DQueue *dq, uint8_t in, uint8_t out, uint8_t l4a, uint8_t l4b) {
	in = in & 0xf;
	out = out & 0x7;

	for(int i=0; i<16; i++) {
		// IDEA: 
		// 	i ^ O2 = right part sbox input
		uint8_t O2 = i ^ in;

		uint8_t s2_1 = S2[(i & 0x08) >> 3][i & 0x7];
		uint8_t s2_2 = S2[(O2 & 0x08) >> 3][O2 & 0x7];

		// We store only couples that gives
		// the correct output.
		if((s2_1 ^ s2_2) == out)
			append_dqueue(dq, i, O2);
	}

	#ifdef ATKDEBUG
	printf("\n######### [get_s2_input_couples()] #########\n");
	DNode *n = dq->head;
	while(n != NULL) {
		print_bits(n->first, 4);
		print_bits(n->second, 4);
		n = n->next;
	}
	#endif

	return dq;
}

/*
 * Find possible 8-bit keys.
 * 
 * Takes left and right S-BOXes input couples
 * and returns a Queue of possible 8-bit keys
 */
Queue * get_pkeys(Queue *q, DQueue *dql, DQueue *dqr, uint8_t l4) {
	Node *n, *nl, *nr;
	Queue *l = new_queue();
	Queue *r = new_queue();
	Queue *lk = new_queue();
	Queue *rk = new_queue();

	for (DNode *dn = dql->head; dn != NULL; dn = dn->next) {
		if(!is_in_queue(l, dn->first))
			append_queue(l, dn->first);
		if(!is_in_queue(l, dn->second))
			append_queue(l, dn->second);
	}

	for (DNode *dn = dqr->head; dn != NULL; dn = dn->next) {
		if(!is_in_queue(r, dn->first))
			append_queue(r, dn->first);
		if(!is_in_queue(r, dn->second))
			append_queue(r, dn->second);
	}

	// IDEA:
	// 	Extract the round key by difference
	// 	left (or right) part of expansion ^ left (or right) S-BOX input gives 4 bits of round key
	//	(Works with l4(a). At this point makes no differencies working with l4a or l4b)

	// First four bit of expansion
	// Making a list of first-four-key bits
	uint8_t ffe = (expansion(l4) & 0xf0) >> 4;
	n = l->head;
	while(n != NULL) {
		append_queue(lk, ffe ^ n->info);
		n = n->next;
	} 

	// Last four bit of expansion
	// Making a list of last-four-key bits
	uint8_t lfe = (expansion(l4) & 0xf);
	n = r->head;
	while(n != NULL) {
		append_queue(rk, lfe ^ n->info);
		n = n->next;
	}

	// Making 8-bit keys
	nl = lk->head;
	while(nl != NULL) {
		nr = rk->head;
		while(nr != NULL) {
			append_queue(q, (nl->info << 4) | nr->info);
			nr = nr->next;
		}
		nl = nl->next;
	}

	#ifdef ATKDEBUG
	printf("\n######### [get_pkeys()] #########\n");
	printf("L4 expansion\n");
	print_bits(expansion(l4), 8);
	printf("Left part L4 expansion\n");
	print_bits((expansion(l4) & 0xf0) >> 4, 4);
	printf("Right part L4 expansion\n");
	print_bits(expansion(l4) & 0xf, 4);
	printf("\nLeft O1 possibilities\n");
	n = l->head;
	while(n != NULL) {
		print_bits(n->info, 4);
		n = n->next;
	}
	printf("Right O1 possibilities\n");
	n = r->head;
	while(n != NULL) {
		print_bits(n->info, 4);
		n = n->next;
	}
	printf("\nLeft key possibilities\n");
	n = lk->head;
	while(n != NULL) {
		print_bits(n->info, 4);
		n = n->next;
	}
	printf("Right key possibilities\n");
	n = rk->head;
	while(n != NULL) {
		print_bits(n->info, 4);
		n = n->next;
	}
	#endif

	return q;
}

/*
 * Perform a single attack
 * based on two choosen plaintextes
 *
 * It takes 2 plaintextes and their 2 cyphertextes.
 * Returns a Queue of possible 8-bit keys.
 */
Queue * single_attack(Queue *k8b, uint16_t p1, uint16_t p2, uint16_t c1, uint16_t c2) {
	uint8_t l1a = (p1 & 0xfc0) >> 6;
	uint8_t l1b = (p2 & 0xfc0) >> 6;
	uint8_t r1a = p1 & 0x3f;
	uint8_t r1b = p2 & 0x3f;

	uint8_t l4a = (c1 & 0xfc0) >> 6;
	uint8_t l4b = (c2 & 0xfc0) >> 6;
	uint8_t r4a = c1 & 0x3f;
	uint8_t r4b = c2 & 0x3f;

	uint8_t sinput = get_sbox_input(l4a, l4b);
	uint8_t soutput = get_sbox_output(l1a, l1b, r4a, r4b);
	
	DQueue *s1_input  = new_dqueue();
	DQueue *s2_input  = new_dqueue();

	s1_input = get_s1_input_couples(s1_input, sinput, soutput, l4a, l4b);
	s2_input = get_s2_input_couples(s2_input, sinput, soutput, l4a, l4b);

	Queue *q = new_queue();
	q = get_pkeys(q, s1_input, s2_input, l4a);

	#ifdef ATKDEBUG
	printf("\n######### [single_attack()] #########\n");
	printf("sbox input-output\n");
	print_bits(sinput, 8);
	print_bits(soutput, 6);

	printf("\npossible 8-bit keys\n");
	Node *n = q->head;
	while(n != NULL) {
		print_bits(n->info, 8);
		n = n->next;
	}
	#endif

	return q;
}

/*
 * Extract 8-bit key.
 * S-DES has three rounds, so 3 choosen plaintextes 
 * will be enough.
 *
 * Every attack will results in a bunch of bossible 8-bit keys.
 * The correct key will be the one is present in every single attack results.
 *
 * Note that you will have to found the 9th-key-bit.
 */
uint8_t attack(uint16_t* plains, uint16_t* cryptos) {
	uint8_t k8b;
	Queue *q1 = new_queue();
	Queue *q2 = new_queue();
	Queue *q3 = new_queue();
	Queue *q = new_queue();
	Node *n;

	// Attacco coppia 1,2 -> ottengo lista di possibili chiavi a 8 bit
	q1 = single_attack(q1, plains[0], plains[1], cryptos[0], cryptos[1]);

	// Attacco coppia 1,3 -> ottengo lista di possibili chiavi a 8 bit
	q2 = single_attack(q1, plains[0], plains[2], cryptos[0], cryptos[2]);

	// Attacco coppia 2,3 -> ottengo lista di possibili chiavi a 8 bit
	q3 = single_attack(q1, plains[1], plains[2], cryptos[1], cryptos[2]);

	#ifdef ATKDEBUG
	printf("\n######### [attack()] #########\nq1\n");
	n = q1->head;
	while(n != NULL) {
		print_bits(n->info, 8);
		n = n->next;
	}
	printf("\nq2\n");
	n = q2->head;
	while(n != NULL) {
		print_bits(n->info, 8);
		n = n->next;
	}
	printf("\nq3\n");
	n = q3->head;
	while(n != NULL) {
		print_bits(n->info, 8);
		n = n->next;
	}
	#endif

	// Searching for the right key
	n = q1->head;
	while(n != NULL) {
		if(is_in_queue(q2, n->info) && is_in_queue(q3, n->info))
			append_queue(q, n->info);
		n = n->next;
	}

	if(q->len == 1)
		return q->head->info;
	else {
		printf("### ATTACK ERROR: more than one 8bit-key found.\n");
		n = q->head;
		while(n != NULL) {
			print_bits(n->info, 8);
			n = n->next;
		}
		return 0;
	}
}


