#include <stdio.h>
#include <gmp.h>

int main(void) {
	// n of Key A is built from too near p and q.
	// Find p,q;

	mpz_t key,a,b,p,q;
	mpz_inits(key,a,b,p,q,NULL);
	mpz_set_str(key,"7723537042671053533349250770213334979939936820556134073043654409875795653759249636472530580914872873851196626721386328853913207026848732669575096806570611",10);

	// Hence p and q are near, let's crack them with Fermat
	mpz_sqrt(a,key);

	mpz_pow_ui(b,a,2);
	mpz_sub(b,b,key);
	while(!mpz_perfect_power_p(b)) {
		mpz_add_ui(a,a,1);
		mpz_pow_ui(b,a,2);
		mpz_sub(b,b,key);
	}
	mpz_sqrt(b,b);
	mpz_sub(p,a,b);
	mpz_add(q,a,b);
	gmp_printf("P: %Zd\nQ: %Zd\n",p,q);

	mpz_mul(a,p,q);
	if(mpz_cmp(key,a)==0)
		printf("OK\n");

	return 0;
}