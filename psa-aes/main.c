#include <stdlib.h>
#include <stdio.h>
#include "pAES.c"

int main() {
	BYTE in[] = { 'c', 'i', 'a', 'o' };
	BYTE key[] = { 'p','p','p','p','p','p','p','p','p','p','p','p','p','p','p','p' };
	BYTE out[16];

	p_aes128_encrypt(in, out, key);

	for(int i=0; i<sizeof(out); i++)
		printf("%4x", out[i]);
	printf("\n");
	return 0;
}