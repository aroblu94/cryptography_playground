#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <gmp.h>

int main(void) {
	int length, int_seed;
	mpz_t random, seed;
	gmp_randstate_t state;

	// Init
	srand(time(NULL));
	int_seed = rand();
	length = (rand()%5000)+3000;
	mpz_inits(random,seed,NULL);


	mpz_set_ui(seed, int_seed);

	// Set the seed
	gmp_randinit_default(state);
	gmp_randseed(state, seed);

	// Gen the random!
	mpz_urandomb(random, state, length);
	gmp_printf("%Zd\n",random);

	// Clear
	gmp_randclear(state);
	mpz_clear(random);

	return 0;
}