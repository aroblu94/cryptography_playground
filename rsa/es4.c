/*
 * KPUB = (618240007109027021, 65537)
 * C = 254665136275622757
 *
 * C is too short, we can easily find M
 *
 * Idea:
 * - generate 10^9 y^e mod n
 * - save them into a file
 * - order them into the file
 * - binary search using c*(x^-e) mod n
 *
 * UPDATE:
 * In order to speed up process we'll use rsa16()
 * in order to produce a 16bit key pair.
 * MLD is set to 1000.
 *
 * NOTE that to reproduce this attack, m (so, c too) HAS TO BE LESS THAN n.
 *
 * Thanks to @cdzeno for the original RSA key generation.
 */

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <gmp.h>

// RSA key length in bit and byte
#define NBIT 64
#define NBYTE 8
#include "rsa.c"


#define DEBUG 0
#define MLD 1000
#define LIST "list.bin"
#define SORTED "sorted.bin"
#define WRITE "wb"
#define READ "r"
#define APPEND "ab"
#define RW "rwb+"

void fp_read(FILE* fp, long pos, mpz_t num);
void fp_write(FILE* fp, long pos, mpz_t num);
void populateDB(FILE* fp, mpz_t n, mpz_t e);
void sortDB(FILE* out, FILE* in);
void printDB(FILE* fp);
void findEqualPos(FILE* fp, mpz_t e, mpz_t n, mpz_t c, int *x, int *y);

void removeFiles();

int main(void) {
	FILE *file,*sorted;
	int x,y;
	mpz_t n,e,c,m,tmpa,tmpb;
	mpz_inits(n,e,c,m,tmpa,tmpb,NULL);

	// SPEED UP PROCESS
	// RSA with 16bit key
	rsaGetPub(n,e);
	mpz_set_str(m,"12345",10);
	gmp_printf("M: %Zd\n", m);
	// C = m^e mod n
	mpz_powm(c,m,e,n);

	removeFiles();
	populateDB(file, n, e);
	printDB(file);
	sortDB(sorted,file);
	findEqualPos(sorted,e,n,c,&x,&y);

	printf("-------------------- RESULT  --------------------\n");
	printf("x: %d - y: %d\n", x,y);

	// At this point we have giant[x]==small[y]:
	// c*x^(-e) mod n == y^e mod n
	// => C = y^e * x^e = (x*y)^e
	// => M = x*y
	mpz_set_ui(tmpa,x);
	mpz_set_ui(tmpb,y);
	mpz_mul(m,tmpa,tmpb);
	mpz_mod(m, m, n);
	gmp_printf("found M: %Zd\n", m);

	mpz_clears(n,e,c,m,tmpa,tmpb,NULL);
	return 0;
}

void fp_write(FILE* fp, long pos, mpz_t num) {
	fp = fopen(LIST, APPEND);
	char *buff = (char*) malloc(sizeof(char) * NBYTE);
	mpz_export(buff, NULL, 1, NBYTE, 1, 0, num);
	fwrite(buff,sizeof(buff),1,fp);
	free(buff);
	fclose(fp);
}

void fp_read(FILE* fp, long pos, mpz_t num) {
	fp = fopen(LIST, READ);
	int plusone = NBYTE + 1;
	char *buff = (char*) malloc(sizeof(char) * NBYTE);
	char *tmp = (char*) malloc(sizeof(char) * 2);
	char *str = (char*) malloc(sizeof(char) * plusone);
	str[0] = '\0';
	if(fseek(fp, pos, SEEK_SET) != 0)
		printf("Seek error\n");
	fread(buff,NBYTE,1,fp);
	for(int i=0; i<NBYTE; i++) {
    	sprintf(tmp,"%02x", (buff[i] & 0xff));
    	str = strcat(str,tmp);
	}
	mpz_set_str(num,str,16);
	free(buff);
	free(tmp);
	free(str);
	fclose(fp);
}

void populateDB(FILE* fp, mpz_t n, mpz_t e) {
	if(DEBUG) { printf("------- DEBUGGING populateDB() ------\n"); }
	mpz_t tmp1,tmp2,tmp3;
	mpz_inits(tmp1,tmp2,tmp3,NULL);
	for(int i=0; i<MLD; i++) {
		// Build small step:
		// y^e mod n
		mpz_set_ui(tmp1,i+1);
		mpz_powm(tmp2,tmp1,e,n);
		fp_write(fp, i*NBYTE, tmp2);
		if(DEBUG) {
			fp_read(fp, i*NBYTE, tmp3);
			gmp_printf("\t[%d]: %Zx\n",i,tmp3);
		}
	}
	mpz_clears(tmp1,tmp2,tmp3,NULL);
	if(DEBUG) { printf("-------------------------------------\n"); }
}

void printDB(FILE* fp) {
	if(DEBUG) { printf("-------- DEBUGGING printDB() --------\n"); }
	mpz_t num;
	mpz_init(num);
	for(int i=0; i<MLD; i++) {
		fp_read(fp, i*NBYTE, num);
		if(DEBUG) { gmp_printf("\t[%d]: %Zx\n",i,num); }
	}
	if(DEBUG) { printf("-------------------------------------\n"); }
}

void sortDB(FILE* out, FILE* in) {
	// TODO
	// Currently only copy list.bin to sorted.bin
	pid_t pid = fork();
	if (pid == 0)
        execl("/bin/cp", "/bin/cp", "list.bin", "sorted.bin", (char *)0);
	return;
}

void findEqualPos(FILE* fp,mpz_t e,mpz_t n,mpz_t c,int *x,int *y) {
	if(DEBUG) { printf("------ DEBUGGING  findEqualPos() ------\n"); }
	// Build giant step:
	// c*(x^-e) mod n
	// ne contains -e
	mpz_t ne,tmp,giant,small;
	mpz_inits(ne,tmp,giant,small,NULL);
	mpz_neg(ne,e);
	for(int i=0; i<MLD; i++) {
		mpz_set_ui(tmp,i+1);
		// giant = c * i^(-e) mod n
		mpz_powm(giant,tmp,ne,n);
		mpz_mul(giant,giant,c);
		mpz_mod(giant, giant, n);

		// read small from file
		for(int j=0; j<MLD; j++) {
			if(DEBUG) { printf("\t[i: %d - j: %d]\n",i,j); }
			fp_read(fp,j*NBYTE,small);
			if(mpz_cmp(giant,small)==0) {
				*x = i+1;
				*y = j+1;
				if(DEBUG) { gmp_printf("\tGiant: %Zd\n\tSmall: %Zd\n",giant,small); }
				mpz_clears(ne,tmp,giant,small,NULL);
				return;
			}
		}
	}
	mpz_clears(ne,tmp,giant,small,NULL);
	if(DEBUG) { printf("-------------------------------------\n"); }
	return;
}

void removeFiles() {
	remove(LIST);
	remove(SORTED);
	return;
}
