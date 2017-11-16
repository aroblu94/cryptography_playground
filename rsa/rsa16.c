#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <gmp.h>

#define length 16

#define TRUE  1
#define FALSE 0

void getRandomKey(mpz_t key, mpz_t p, mpz_t q, unsigned long n);
void getRandomPrime(mpz_t prime, unsigned long n, int coeff);
void findED(mpz_t n,mpz_t p, mpz_t q, mpz_t e, mpz_t d);
void find_fi(mpz_t fi, mpz_t p, mpz_t q);
int rsa16pub(mpz_t key, mpz_t e);

int rsa16pub(mpz_t key, mpz_t e){

    unsigned long int n; 
    mpz_t p,q,d;

    mpz_inits(p,q,d,NULL);
   
    getRandomKey(key,p,q,length);
    gmp_printf("N: %Zd = %Zd * %Zd\n",key,p,q);

    findED(key,p,q,e,d);
    return 0;
}

void getRandomPrime(mpz_t prime, unsigned long n, int coeff){
     
    unsigned long seed;

    mpz_t random;
    gmp_randstate_t state;

    srand(time(NULL));
    seed = rand() * coeff;
    mpz_init(random);
    gmp_randinit_mt(state);

    gmp_randseed_ui(state, seed);
    mpz_rrandomb(random, state, n);

    mpz_nextprime(prime, random);
    
    gmp_randclear(state);
    mpz_clear(random);
}

void getRandomKey(mpz_t key, mpz_t p, mpz_t q, unsigned long n){

    getRandomPrime(p,n/2, 179);
    getRandomPrime(q,n/2, 199);

    mpz_mul(key,p,q);
}

void findED(mpz_t n,mpz_t p, mpz_t q, mpz_t e, mpz_t d){
    // return: d = e^-1 mod fi(n) = e^-1 mod (p-1)(q-1)     
    mpz_t fi,bound,mcd;
    mpz_init(fi);    
    mpz_init(bound);
    mpz_init(mcd);

    mpz_set_ui(e,65536);
    find_fi(fi,p,q);

    do{
        mpz_add_ui(e,e,1);
        mpz_gcd(mcd,e,fi);
    }while(mpz_cmp_ui(mcd,1) != 0 );

    mpz_invert(d, e, fi);
    gmp_printf("D: %Zd\nE: %Zd\n", d,e);
}

void find_fi(mpz_t fi, mpz_t p, mpz_t q) {
    // find_fi: return fi = (p-1)(q-1)
    // nb: p,q are prime numbers

    mpz_t p_1,q_1;
    mpz_init(p_1);
    mpz_init(q_1);

    mpz_sub_ui(p_1,p,1);
    mpz_sub_ui(q_1,q,1);
    mpz_mul(fi,p_1,q_1);
}
