/*
 * We have 2 pub keys:
 * Key A: (734832328116465196692838283564479145339399316070744350481249855342603693301556011015131519,17)
 * Key B: (734832328116465196692838283564479145339399316070744350481249855342603693301556011015131519, 2^16+1)
 *
 * Using kA and kB, rsa(M,key) are:
 * cA = 93734709405860059315217893988749243105435169093069740192815939750720282941641642747093171
 * cB = 269125499927676766418170761935369755935025654357890774691167668682175704132138220498309902
 *
 * We have to find M, convert into ASCII and check the result in browser
 */

#include <stdio.h>
#include <gmp.h>
#include <string.h>
#include <stdlib.h>

int main(void) {
	// => This is a Common Modules Attack!

	char *result;
	int tmp,len;
	mpz_t n,ea,eb,ca,cb,c,r,m,s,rest;
	mpz_inits(n,ea,eb,ca,cb,c,r,m,s,rest,NULL);
	mpz_set_str(n,"734832328116465196692838283564479145339399316070744350481249855342603693301556011015131519",10);
	mpz_set_str(ea,"17",10);
	mpz_set_str(eb,"2",10);
	mpz_pow_ui(eb,eb,16);
	mpz_add_ui(eb,eb,1); // eb = 2^16 + 1
	mpz_set_str(ca,"93734709405860059315217893988749243105435169093069740192815939750720282941641642747093171",10);
	mpz_set_str(cb,"269125499927676766418170761935369755935025654357890774691167668682175704132138220498309902",10);

	// Trying to solve ea*x + eb*y = 1

	// Expanded Euclid
	mpz_gcdext(c, r, s, ea, eb);
	// Check on GCD
	mpz_cdiv_r_ui(rest,c,1);
	if(mpz_cmp_ui(rest,0)!=0) {
		printf("ERROR: c != GCD(ea,eb)\n");
		return 1;
	}

	// At this point we should have
	// r = *(p+1)
	// s = *(p+2)
	// Lets find plaintext!
	mpz_powm(ca, ca, r, n);
	mpz_powm(cb, cb, s, n);
	//M = ca^r * cb^s mod n
	mpz_mul(m, ca, cb);
	mpz_mod(m, m, n);

	gmp_printf("M: %Zd\n",m);
	result = mpz_get_str(result, 10, m);

	// Is result ASCII readable?
	// If yes print it
	if (strlen(result) % 3 != 0)
		return 1;
	while (*result != '\0') {
		char string_3l[4] = {'\0'};
		strncpy(string_3l, result, 3);
		putchar(atoi(string_3l));
		result += 3;
	}
	printf("\n");

	return 0;
}


