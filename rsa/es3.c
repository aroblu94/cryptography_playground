/*
 * We have 10 32-bits public keys stored into db.bin
 * We know that some of them have p or q in common.
 * 
 * We have to find those keys as K = p * q
 */

#include <stdio.h>
#include <gmp.h>
#include <string.h>

#define DEBUG 0
#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 

void fp_read(FILE* fp, mpz_t *arr);

int main(void) {
	// Feeding the keys array
	FILE *file;
	file = fopen("db.bin", "rb");
	mpz_t keys[10];
	fp_read(file,keys);

	// Now pub keys are stored into keys[]
	// Lets start the attack.
	mpz_t N[10];
	mpz_t gcd[10];
	if(DEBUG) { printf("----------- DEBUGGING GCD -----------\n"); }
	for(int i=0; i<10; i++) {
		mpz_init(N[i]);
		mpz_set_ui(N[i],1);
		// N[i] contains keys[0] * keys[1] * ... * keys[9] except for keys[i]
		for(int j=0; j<10; j++) {
			if(i==j) { break; }
			mpz_mul(N[i],N[i],keys[j]);
		}

		// Store GCD(N[i],keys[i]) into gcd[i]
		mpz_init(gcd[i]);
		mpz_gcd(gcd[i],N[i],keys[i]);
		if(DEBUG) { gmp_printf("GCD[%d]: %Zd\n",i,gcd[i]); }
	}
	if(DEBUG) { printf("-------------------------------------\n"); }

	// If gcd[i]!=1 => keys[i] = gcd[i] * q
	// Anyway we do other tests like "founded_q * gcd[i] == keys[i]"
	mpz_t tmp,q,r;
	mpz_inits(tmp,q,r,NULL);
	for(int i=0; i<10; i++) {
		if(mpz_cmp_ui(gcd[i],1)!=0) {
			 mpz_cdiv_qr(q,r,keys[i],gcd[i]);
			 mpz_mul(tmp,q,gcd[i]);
			 if(mpz_cmp_ui(r,0)==0 && mpz_cmp(tmp,keys[i])==0)
			 	gmp_printf("[K%d]:\t%Zd = %Zd * %Zd\n",i+1,keys[i],gcd[i],q);
		}
	}

	return 0;
}

void fp_read(FILE* fp, mpz_t *arr) {
	// Populating the keys array
	if(DEBUG) { printf("-------- DEBUGGING fp_read() --------\n"); }
	char buffer[320], tmp[1000];
	fread(buffer,sizeof(buffer),1,fp);
	for(int j=0; j<10; j++) {
		for(int i=0; i<32; i++) {
	    	sprintf(tmp+(i*8),""BYTE_TO_BINARY_PATTERN"", BYTE_TO_BINARY(buffer[i+j*32] & 0xff));
	    	if(DEBUG) { printf("%02x ",buffer[i+j*32] & 0xff); }
		}
		if(DEBUG) { printf("\n"); }
	    mpz_init(arr[j]);
	    mpz_set_str(arr[j],tmp,2);
	    memset(tmp, 0, sizeof tmp);
	}
	if(DEBUG) { printf("-------------------------------------\n"); }
}