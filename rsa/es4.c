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
 */
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <gmp.h>

#define DEBUG 1
#define MLD 100
#define LIST "list.bin"
#define SORTED "sorted.bin"
#define WRITE "wb"
#define READ "r"
#define APPEND "ab"
#define RW "rwb+"
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

void fp_read(FILE* fp, long pos, mpz_t num);
void fp_write(FILE* fp, long pos, mpz_t num);
void populateDB(FILE* fp, mpz_t n, mpz_t e);
void sortDB(FILE* out, FILE* in);
void printDB(FILE* fp);
void findEqual(FILE* sorted,int x,mpz_t yval);

void removeFiles();

int main(void) {
	FILE *file,*sorted;
	int a,b,x;
	mpz_t n,e,c,m,yval,tmpa,tmpb;
	mpz_inits(n,e,c,m,tmpa,yval,tmpb,NULL);
	mpz_set_str(n,"618240007109027021",10);
	mpz_set_str(e,"65537",10);
	mpz_set_str(c,"254665136275622757",10);

	removeFiles();
	populateDB(file, n, e);
	// printDB(file);
	sortDB(sorted,file);
	findEqual(sorted,x,yval);

	
	// if(findEqualsPos(vec1,vec2,a,b))
	// 	return 1;

	if(DEBUG) { printf("a: %d - b: %d\n", a,b); }

	// At this point we have vec1[a]==vec2[b]:
	// c*a^(-e) mod n == b^e mod n
	// => C = b^e * a^e = (a*b)^e
	// => M = a*b
	mpz_set_ui(tmpa,a);
	mpz_set_ui(tmpb,b);
	mpz_mul(m,tmpa,tmpb);
	mpz_mod(m, m, n);
	gmp_printf("M: %Zd\n", m);

	return 0;
}

void fp_write(FILE* fp, long pos, mpz_t num) {
	fp = fopen(LIST, APPEND);
	char *buff = (char*) malloc(sizeof(char) * 8);
	mpz_export(buff, NULL, 1, 8, 1, 0, num);
	fwrite(buff,sizeof(buff),1,fp);
	free(buff);
	fclose(fp);
}

void fp_read(FILE* fp, long pos, mpz_t num) {
	fp = fopen(LIST, READ);
	char *buff = (char*) malloc(sizeof(char) * 8);
	char *tmp = (char*) malloc(sizeof(char) * 2);
	char *str = (char*) malloc(sizeof(char) * 8);
	if(fseek(fp, pos, SEEK_SET) != 0)
		printf("Seek error\n");
	fread(buff,8,1,fp);
	for(int i=0; i<8; i++) {
    	sprintf(tmp,"%02x", (buff[i] & 0xff));
    	strcat(str,tmp);
	}
	mpz_set_str(num,str,16);
	free(buff);
	free(tmp);
	fclose(fp);
}

void populateDB(FILE* fp, mpz_t n, mpz_t e) {
	if(DEBUG) { printf("------- DEBUGGING populateDB() ------\n"); }
	mpz_t tmp1,tmp2,tmp3;
	mpz_inits(tmp1,tmp2,NULL);
	for(int i=0; i<MLD; i++) {
		mpz_set_ui(tmp1,i+1);
		mpz_powm(tmp2,tmp1,e,n);
		fp_write(fp, i*8, tmp2);
		if(DEBUG) {
			mpz_init(tmp3);
			fp_read(fp, i*8, tmp3);
			gmp_printf("\t[%d]: %Zx\n",i,tmp3); 
		}
	}
	if(DEBUG) { printf("-------------------------------------\n"); }
}

void printDB(FILE* fp) {
	if(DEBUG) { printf("-------- DEBUGGING printDB() --------\n"); }
	mpz_t num;
	mpz_init(num);
	for(int i=0; i<MLD; i++) {
		fp_read(fp, i*8, num);
		gmp_printf("\t[%d]: %Zx\n",i,num);
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

void findEqual(FILE* sorted,int x,mpz_t yval) {
	return;
}

void removeFiles() {
	remove(LIST);
	remove(SORTED);
	return;	
}