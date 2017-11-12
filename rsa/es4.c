/*
 * KPUB = (618240007109027021, 65537)
 * C = 254665136275622757
 * 
 * C is too short, we can easily find M
 */
#include <stdio.h>
#include <gmp.h>

#define DEBUG 1
#define MLD 100000
// Note: At first MLD=10^9.
// Due to - I think - machine memory,
// creating 2 arrays of 10^9 mpz_t cause SIGSEGV (Address boundary error)

void populateVectors(mpz_t *v1, mpz_t *v2, mpz_t n, mpz_t e, mpz_t c);
int findEqualsPos(mpz_t *v1, mpz_t *v2, int a, int b);

int main(void) {
	int a,b;
	mpz_t n,e,c,m,tmpa,tmpb,vec1[MLD],vec2[MLD];
	mpz_inits(n,e,c,m,tmpa,tmpb,NULL);
	mpz_set_str(n,"618240007109027021",10);
	mpz_set_str(e,"65537",10);
	mpz_set_str(c,"254665136275622757",10);

	populateVectors(vec1,vec2,n,e,c);

	if(findEqualsPos(vec1,vec2,a,b))
		return 1;

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

void populateVectors(mpz_t *v1, mpz_t *v2, mpz_t n, mpz_t e, mpz_t c) {
	// ne contains -e
	mpz_t ne,tmp;
	mpz_inits(ne,tmp,NULL);
	mpz_neg(ne,e);
	if(DEBUG) { printf("---- DEBUGGING  populateVector() ----\n"); }
	for(int i=0; i<MLD; i++) {
		mpz_set_ui(tmp,i+1);
		mpz_inits(v1[i],v2[i],NULL);
		// v1[i] = c * i^(-e) mod n
		mpz_powm(v1[i],tmp,ne,n);
		mpz_mul(v1[i],v1[i],c);
		mpz_mod(v1[i], v1[i], n);

		// v2[i] = i^e mod n
		mpz_powm(v2[i],tmp,e,n);
		if(DEBUG) { gmp_printf("[I] %d\n\tv1[i]: %Zd\n\tv2[i]: %Zd\n", i, v1[i], v2[i]); }
	}
	if(DEBUG) { printf("-------------------------------------\n"); }
}

int findEqualsPos(mpz_t *v1, mpz_t *v2, int a, int b) {
	if(DEBUG) { printf("----- DEBUGGING findEqualsPos() -----\n"); }
	for(int i=0; i<MLD; i++) {
		for(int j=0; j<MLD; j++) {
			if(DEBUG) { printf("[i: %d - j: %d]\n",i,j); }
			if(mpz_cmp(v1[i],v2[j])==0) {
				a = i;
				b = j;
				if(DEBUG) { gmp_printf("\tv1[i]: %Zd\n\tv2[j]: %Zd\n",v1[i],v2[j]); }
				return 0;
			}
		}
	}
	if(DEBUG) { printf("-------------------------------------\n"); }
	printf("ERROR: a,b not found\n");
	return 1;
}