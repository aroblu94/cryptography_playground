#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <gmp.h>

int main(int argc, char **argv) {
	if(argc<2) {
		printf("ERROR!\nUsage: ./main random_length\n");
		return -1;
	}

	int length, int_seed;
	mpz_t random, seed;
	gmp_randstate_t state;

	srand(time(NULL));

	// Init
	mpz_inits(random,seed,NULL);
	int_seed = rand();

	mpz_set_ui(seed, int_seed);

	// Set the seed
	gmp_randinit_default(state);
	gmp_randseed(state, seed);

	// Gen the random!
	length = atoi(argv[1]);
	mpz_urandomb(random, state, length);
	gmp_printf("%Zd\n",random);

	// Clear
	gmp_randclear(state);
	mpz_clear(random);

	return 0;
}