#include "sdes_four.h"

/*************/
/** S-BOXes **/
/*************/
static const uint8_t S1[2][8] = {
    {5, 2, 1, 6, 3, 4, 7, 0},
    {1, 4, 6, 2, 0, 7, 5, 3}
};

static const uint8_t S2[2][8] = {
    {4, 0, 6, 5, 7, 1, 3, 2},
    {5, 3, 0, 7, 6, 2, 1, 4}
};


/***********/
/** UTILS **/
/***********/
uint16_t string_to_hex(char* string) {
	return (uint16_t) strtoul(string, NULL, 2);
}

void print_bits(uint16_t hex, int dim) {
	int i;
    for (i=0; i<dim; i++)
        putchar('0'+(hex>>(dim-1-i)&1));
    printf("\n");
}


/*********************/
/** S-DES FUNCTIONS **/
/*********************/
uint8_t expansion(uint8_t r) {
	uint8_t out = 0;
    
    out = r & 0x3;
    out |= (r & 0x30) << 2;
    out |= (r & 0xc) << 1;
    out |= (r & 0x4) << 3;
    out |= (r & 0x8) >> 1;
    
    return out;
}

/* 
 * Generate round key
 * round starts from value 1! Be careful and take precautions! 
 */
uint8_t gen_key(uint16_t key, int round) {
	uint8_t out;
    int round_ck = ((round % 12)==0) ? 1 : (round % 12);  

	if(round_ck == 1)
		out = (key>>1) & 0xff;
	else {
        out = (key<<(round_ck-2)) & 0xff;
        out |= key>>(11-round_ck);
    }

	return out;
}

/* 
 * feistel function should include expansion invocation 
 */
uint8_t feistel(uint8_t r, uint8_t k) {
	/***********************************************************
    Accessing the S-BOX values:
    when xor = E(R0) XOR K1 = 01010111 XOR 01001101 = 00011010,
    S0[0][001] = (S[0][1]) = 010
    S1[1][010] = (S[1][2]) = 000
    ************************************************************/
	uint8_t expanded, left, right, sbox_left, sbox_right;

	#ifdef DEBUG
    printf("# E(R):\t\t\t");
    print_bits(expansion(r), 8);
    printf("# K:\t\t\t");
    print_bits(k, 8);
    #endif

	expanded = expansion(r);
	expanded = expanded ^ k;

	#ifdef DEBUG
    printf("# E(R) XOR K:\t\t");
    print_bits(expanded, 8);
    #endif

	left = (expanded & 0xf0) >> 4;
	right = expanded & 0x0f;

	sbox_left = S1[(left & 0x08) >> 3][left & 0x7];
	sbox_right = S2[(right & 0x08) >> 3][right & 0x7];

	#ifdef DEBUG
    printf("# feistel(R, K):\t");
    print_bits(((sbox_left << 3) | sbox_right), 6);
    #endif

	return (uint8_t) (sbox_left << 3) | sbox_right;
}

/* 
 * round performs a single s-des round 
 * key is the round key (already shifted)
 */
uint16_t exec_round(uint16_t text, uint8_t key) {
	uint16_t out = 0;
    
    uint8_t left = (text & 0xfc0) >> 6;
    uint8_t right = text & 0x3f;
    
    #ifdef DEBUG
    printf("# L:\t\t\t");
    print_bits(left, 6);
    printf("# R:\t\t\t");
    print_bits(right, 6);
    #endif
    
    out = (right << 6) | (left ^ feistel(right, key));
    
    #ifdef DEBUG
    printf("# Output:\t\t");
    print_bits(out, 12);
    #endif
    
    return out;
}

/* 
 * swap_left_right represents the last swap before the final output 
 */
uint16_t swap_left_right(uint16_t in) {
    uint16_t out = 0;
    
    out = (in & 0x3f) << 6;
    out |= (in & 0xfc0) >> 6;
    
    return out;
}


/*****************************/
/** ENCRIPTION & DECRIPTION **/
/*****************************/
uint16_t encrypt(const uint16_t text, const uint16_t key) {
    uint16_t cryptotext;
    uint8_t round_key;
    int i;
    
    #ifdef DEBUG
    printf("# DEBUG MODE: ENCRYPTION ###########\n");
    #endif
    
    cryptotext = text;
    
    for (i=FIRSTROUND; i<=LASTROUND; ++i) {
        #ifdef DEBUG
        printf("# ROUND %d --------------------------\n", i);
        #endif
        round_key = gen_key(key, i);
        cryptotext = exec_round(cryptotext, round_key);
    }
    
    #ifdef DEBUG
    printf("####################################\n");
    #endif
    
    cryptotext = swap_left_right(cryptotext);
    return cryptotext;
}

uint16_t decrypt(const uint16_t cryptotext, const uint16_t key) {
    uint16_t text;
    uint8_t round_key;
    int i;
    
    text = swap_left_right(cryptotext);
    
    #ifdef DEBUG
    printf("# DEBUG MODE: DECRYPTION ###########\n");
    #endif
    
    text = cryptotext;
    for (i=LASTROUND; i>=FIRSTROUND; --i) {
        #ifdef DEBUG
        printf("# ROUND %d --------------------------\n", i);
        #endif
        round_key = gen_key(key, i);
        text = exec_round(text, round_key);
    }
    
    #ifdef DEBUG
    printf("####################################\n");
    #endif
    
    text = swap_left_right(text);
    return text;
}
