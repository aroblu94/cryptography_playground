#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>

int main(int argc, char **argv) {
	if(argc<4) {
		printf("ERROR!\nUsage: ./main seed seed_base random_length\n");
		return -1;
	}

	int seed_base,length;
	mpz_t random, seed;
	gmp_randstate_t state;

	// Init
	mpz_inits(random,seed,NULL);

	// Get the seed from argv
	seed_base = atoi(argv[2]);
	mpz_set_str(seed, argv[1], seed_base);

	// Set the seed
	gmp_randinit_default(state);
	gmp_randseed(state, seed);

	// Gen the random!
	length = atoi(argv[3]);
	mpz_urandomb(random, state, length);
	gmp_printf("%Zd\n",random);

	// Clear
	gmp_randclear(state);
	mpz_clear(random);

	return 0;
}