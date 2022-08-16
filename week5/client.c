#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <openssl/bn.h>
#include <openssl/rand.h>
#include <json-c/json.h>
#define PK2_new() BN_dxy_new(2)
#define PK2_free(a) BN_dxy_free(a, 2)
#define SK2_new() BN_dxy_new(2)
#define SK2_free(a) BN_dxy_free(a, 2)
#define CT2_new() BN_dxy_new(2)
#define CT2_free(a) BN_dxy_free(a, 2)
#define PK3_new() BN_dxy_new(3)
#define PK3_free(a) BN_dxy_free(a, 3)
#define SK3_new() BN_dxy_new(3)
#define SK3_free(a) BN_dxy_free(a, 3)
#define DXY_new() BN_dxy_new(3)
#define DXY_free(a) BN_dxy_free(a, 3)
typedef unsigned char U8;
static struct sockaddr_in client_addr;
static int client_fd, n, n2, state = 1;
static char recv_data[6000];

typedef struct
{
	union{
		BIGNUM *p;
		BIGNUM *d;
		BIGNUM *N;
		BIGNUM *C0;
	};
	union{
		BIGNUM *y;
		BIGNUM *key;
		BIGNUM *C1;
	};
	union{
		BIGNUM *g;
		BIGNUM *x;
	};
}BN_dxy;
typedef BN_dxy PK;
typedef BN_dxy SK;
typedef BN_dxy CT;

BN_dxy * BN_dxy_new(int element)
{
	BN_dxy * dxy = (BN_dxy *)calloc(1, sizeof(BN_dxy));
	if(element >=1)	dxy->d = BN_new(); 
	if(element >=2)	dxy->y = BN_new();
	if(element >=3)	dxy->x = BN_new();
	return dxy;
}
void BN_dxy_free(BN_dxy * dxy, int element)
{
	if(element >=1)	BN_free(dxy->d);
	if(element >=2)	BN_free(dxy->y);
	if(element >=3)	BN_free(dxy->x);
	free(dxy);
}
void BN_Square_Multi(BIGNUM *z,BIGNUM *x, BIGNUM *a, BIGNUM *n, BN_CTX *bn_ctx)
{
    BN_one(z);

    BIGNUM *temp = BN_new();
    BN_zero(temp);
    if( BN_cmp(a, temp) == 0 ) {
        BN_free(temp);
        return;
    }

    int length = BN_num_bits( a );
    BN_copy(temp, x);
    if( BN_is_bit_set( a, 0 ) ) {
        BN_copy(z, temp);
    }

    for( int i = 1; i < length; i++ ) {
        BN_mod_mul(temp, temp, temp, n, bn_ctx);
        if( BN_is_bit_set( a, i ) ) {
            BN_mod_mul(z, z, temp, n, bn_ctx);
        }
    }

    BN_free(temp);
}
void Find_safe_prime_generator(BIGNUM *g, BIGNUM *p)
{
	// p = 2*q + 1
	int i;
	BN_CTX * ctx = BN_CTX_new();
	BN_CTX_start(ctx);
	BIGNUM * q = BN_CTX_get(ctx);
	BIGNUM * tmp = BN_CTX_get(ctx);
	BIGNUM * h[2];
	h[0] = BN_CTX_get(ctx);
	BN_set_word(g,2);
	BN_set_word(h[0],2);
	BN_copy(tmp, p);
	BN_sub_word(tmp,1);//tmp = p - 1
	BN_div(q,NULL,tmp,g,ctx);//q = (p - 1)/2
	//(p-1) = 2 * q    
	h[1] = q; 
	while(BN_cmp(g,p) != 0)
	{
		for(i=0; i<2; i++)
		{	
			BN_mod_exp(tmp, g, h[i],p,ctx);
			if(BN_is_one(tmp)) break;
		}
		if(i == 2)
			break;
		BN_add_word(g,1);
	}
	BN_CTX_end(ctx);
	BN_CTX_free(ctx);
}
void DH_setup(PK * pub, SK * priv)
{
	BN_CTX * bn_ctx = BN_CTX_new();
	BN_generate_prime_ex(pub->p,1024,1,NULL,NULL,NULL);
	//printf("Complete the select of safe prime\n");
	Find_safe_prime_generator(pub->g,pub->p);
	BN_copy(priv->p, pub->p);
	BN_copy(priv->g, pub->g);
	//printf("Complete the select of generator\n");
	BN_rand_range(priv->key,pub->p);
	BN_Square_Multi(pub->key, pub->g, priv->key, pub->p, bn_ctx);
	BN_CTX_free(bn_ctx);
}

int main(int argc, char *argv[])
{
	char *IP = argv[1];
	in_port_t PORT = atoi(argv[2]);

	if (argc != 3)
	{
		printf("Useage : ./client [IP] [PORT]\n");
		exit(0);
	}

	client_fd = socket(PF_INET, SOCK_STREAM, 0);

	client_addr.sin_addr.s_addr = inet_addr(IP);
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(PORT);

	if (connect(client_fd, (struct sockaddr *)&client_addr, sizeof(client_addr)) == -1)
	{
		printf("Can't Connect\n");
		close(client_fd);
		return -1;
	}

	PK* pub = PK3_new();
	SK* priv = SK3_new();

	DH_setup(pub, priv);
	BN_CTX* bn_ctx = BN_CTX_new();


	/* code */
			
	close(client_fd);
	PK3_free(pub); SK3_free(priv);
	BN_CTX_free(bn_ctx);

	return 0;
}
