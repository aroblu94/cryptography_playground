#include <stdio.h>
#include <gmp.h>

int main(void) {
	mpz_t a,b,c;
	mpz_inits(a,b,c,NULL);
	gmp_printf("%Zd %Zd %Zd\n", a,b,c);

	mpz_set_ui(a,1234567890987654321);
	mpz_set(b,a);
	mpz_add(c,a,b);
	gmp_printf("%Zd %Zd %Zd\n", a,b,c);

	mpz_mul(c,a,b);
	gmp_printf("%Zd %Zd %Zd\n", a,b,c);

	return 0;
}