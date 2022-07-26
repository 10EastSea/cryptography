#include <stdio.h>
#include <openssl/bn.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <ctype.h>
#include <iostream>

using namespace std;

void BN_scanf(BIGNUM *input) {
	int x;
	scanf("%d", &x);
	BN_set_word(input, x);
}

void BN_printf(const BIGNUM *input) {
	char *c = BN_bn2dec(input);
	printf("%s ", c);
}

class BN_dxy{
public:
    BIGNUM *d;
	BIGNUM *x;
	BIGNUM *y;

    BN_dxy(const BIGNUM *d, const BIGNUM *x, const BIGNUM *y){
        this->d = BN_new(); this->x = BN_new(); this->y = BN_new();
        BN_copy(this->d, d);
        BN_copy(this->x, x);
        BN_copy(this->y, y);
    }
    // void BN_copy(BIGNUM *d, BIGNUM *x, BIGNUM *y){
    //     BN_copy(this->d, d);
	//     BN_copy(this->x, x);
	//     BN_copy(this->y, y);
    // }
    // ~BN_dxy(){
    //     BN_free(this->d);
	//     BN_free(this->x);
	//     BN_free(this->y);
    // }
    
    void Print(){
        cout << "d : " << BN_bn2dec(this->d) << endl;
        cout << "x : " << BN_bn2dec(this->x) << endl;
        cout << "y : " << BN_bn2dec(this->y) << endl;
    }
};



BN_dxy BN_Ext_Euclid(BIGNUM* a, BIGNUM* b) {
	if (BN_is_zero(b)) {
		BN_dxy dxy = BN_dxy(a, BN_value_one(), b);
		return dxy;
	}
	else {
		/* your code here */
		BN_CTX *ctx = BN_CTX_new();

		BIGNUM *amodb = BN_new(); BN_mod(amodb, a, b, ctx);
		BN_dxy dxy_prime = BN_Ext_Euclid(b, amodb);
		BN_free(amodb);

		BIGNUM *d = BN_new(); BN_copy(d, dxy_prime.d);
		BIGNUM *x = BN_new(); BN_copy(x, dxy_prime.y);
		BIGNUM *y = BN_new(); BN_div(y, NULL, a, b, ctx); BN_mul(y, y, dxy_prime.y, ctx); BN_sub(y, dxy_prime.x, y);
		BN_dxy dxy = BN_dxy(dxy_prime.d, dxy_prime.y, y);
		BN_free(d); BN_free(x); BN_free(y);

		BN_CTX_free(ctx);
		return dxy;
	}
}

int main(int argc, char* argv[]) {
	BIGNUM *a, *b;
	a = BN_new(); b = BN_new();
	printf("a: "); BN_scanf(a);
	printf("b: "); BN_scanf(b);
	BN_dxy dxy = BN_Ext_Euclid(a, b);
	cout << "====== result ======" << endl;
    dxy.Print();
	return 0;
}