#include <stdlib.h>
#include <stdio.h>
#include "pAES.c"

#define TOP 256

void p_aes128_encrypt_lite(BYTE input[], BYTE output[], BYTE key[], int first, int last) {
    BYTE bufferKey [16];
    memcpy(bufferKey, key, 16);
    memcpy(output, input, 16);

    AddRoundKey(output,bufferKey);
    for(int i = first; i <= last;i++){
        SubBytes(output);
        ShiftRows(output);
        MixColumns(output);
        newRoundKey(bufferKey,i);
        AddRoundKey(output,bufferKey);
    }
}

int main() {
	BYTE in[] = { 0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };
	BYTE key[] = { 'p','p','p','p','p','p','p','p','p','p','p','p','p','p','p','p' };
	BYTE out[16];
	BYTE *res[TOP];

	// p_aes128_encrypt(in, out, key);

	// for(int i=0; i<sizeof(out); i++)
	// 	printf("%4x", out[i]);
	// printf("\n");

	for(int i=0; i<TOP; i++) {
		in[0] += i;

		p_aes128_encrypt_lite(in, out, key, 1, 3);
		for(int j=0; i<sizeof(out); j++)
			printf("%4x", out[j]);
		printf("\n");

		//res[i] = malloc(16 * sizeof(BYTE));
	}

	return 0;
}