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
#include <stdint.h>

int euclid[3];

int* extendedEuclid(int a,int b){ /* implementation of extended Euclid algorithm */
	if(b==0){
		euclid[0]=a;
		euclid[1]=1;
		euclid[2]=0;
		int *p=&euclid;
		return p;
	}
	int *p;
	p=extendedEuclid(b,a%b);
	euclid[0]=*p;
	euclid[1]=*(p+1);
	euclid[2]=*(p+2);
	int temp;
	temp=euclid[2];
	euclid[2]=euclid[1]-(a/b)*euclid[2];
	euclid[1]=temp;
	p=&euclid;
	return p;
}

int main(void) {
	// => This is a Common Modules Attack!

	char *result;
	int tmp,len;
	mpz_t n,ea,eb,ca,cb,c,x,y,r,m;
	mpz_inits(n,ea,eb,ca,cb,c,x,y,r,m,NULL);
	mpz_set_str(n,"734832328116465196692838283564479145339399316070744350481249855342603693301556011015131519",10);
	mpz_set_str(ea,"17",10);
	mpz_set_str(eb,"2",10);
	mpz_pow_ui(eb,eb,16);
	mpz_add_ui(eb,eb,1); // eb = 2^16 + 1
	mpz_set_str(ca,"93734709405860059315217893988749243105435169093069740192815939750720282941641642747093171",10);
	mpz_set_str(cb,"269125499927676766418170761935369755935025654357890774691167668682175704132138220498309902",10);

	// Trying to solve ea*x + eb*y = 1

	// GCD
	mpz_gcd(c,ea,eb);
	mpz_cdiv_r_ui(r,c,1);
	if(mpz_cmp_ui(r,0)!=0) {
		printf("ERROR: c != GCD(ea,eb)\n");
		return 1;
	}

	int *p;
	p=extendedEuclid(17,2^16+1);
	printf("Extended Euclide results: %d %d\n",*(p+1),*(p+2));

	// At this point we should have
	// r = *(p+1)
	// s = *(p+2)
	// Lets find plaintext!
	if(mpz_invert(ca,ca,n)) {
		tmp = abs(*(p+2));
		printf("cA invertibile! tmp: %d\n",tmp);
		mpz_pow_ui(ca,ca,tmp);
		mpz_pow_ui(cb,cb,*(p+1));
	}
	else if(mpz_invert(ca,ca,n)) {
		tmp = abs(*(p+2));
		printf("cB invertibile! tmp: %d\n",tmp);
		mpz_pow_ui(cb,cb,abs(*(p+2)));
		mpz_pow_ui(ca,ca,*(p+1));
	}
	mpz_mul(n,ca,cb);
	gmp_printf("M: %Zd\n",n);

	return 0;
}


