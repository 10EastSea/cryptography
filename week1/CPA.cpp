#include <iostream>
#include <string>
#include <openssl/bn.h>
#include <openssl/aes.h>
#include <malloc.h>
#include <random>

typedef unsigned char U8;
typedef unsigned int U32;

using namespace std;

int BN_xor(BIGNUM *b_r, int bits, const BIGNUM *b_a, const BIGNUM *b_b)
{
	//error
	if(b_r==NULL || b_a == NULL || b_b == NULL) 
		return 0;
	//bytes = bits / 8
	int i, bytes = bits >> 3;
	//calloc for type casting(BIGNUM to U8)
	U8 *r = (U8*)calloc(bytes,sizeof(U8));
	U8 *a = (U8*)calloc(bytes,sizeof(U8));
	U8 *b = (U8*)calloc(bytes,sizeof(U8));
	//BN_num_bytes(a) : return a's bytes 
	int byte_a = BN_num_bytes(b_a);
	int byte_b = BN_num_bytes(b_b);
	//difference between A and B
	int dif = abs(byte_a-byte_b);
	//minimum
	int byte_min = (byte_a < byte_b)? byte_a : byte_b;
	//type casting(BIGNUM to U8)
	BN_bn2bin(b_a,a);
	BN_bn2bin(b_b,b);
	//xor compute
	for(i=1;i<=byte_min;i++)
		r[bytes - i] = a[byte_a - i] ^ b[byte_b - i];
	for(i=1;i<=dif;i++)
		r[bytes - byte_min - i] = (byte_a>byte_b)? a[dif-i] : b[dif-i];
	//type casting(U8 to BIGNUM)
	BN_bin2bn(r,bytes,b_r);
	//Free memory
	free(a);
	free(b);
	free(r);
	return 1;//correct
} 

class CPA {
    uint bitsize = 128; // 128 로 고정
    uint byteSize = 16;
    AES_KEY encKey;
    int Gen(){
		// TODO
    }
public:
    CPA(){
        Gen();
    }
    
    U8** Enc(U8* msg){
        int i;
	    U8 **c = (U8 **)calloc(2, sizeof(U8*)); // C = [r, F_k(r)]
	    for (i = 0; i < 2; i++)
		    c[i] = (U8 *)calloc(byteSize, sizeof(U8)); // 16 bytes cipher text
        
		// TODO

        return c;
    }

    U8* Dec(U8** c){
	    U8 *M = (U8*)calloc(byteSize, sizeof(U8));

		// TODO

        return M;
    } 
};

int main(){
    U8 msg[16] = "CPA-secure";
    CPA cpa = CPA();

    cout << "msg\t: " << msg << endl;

    U8** enc = cpa.Enc(msg);

    cout << "C1\t: ";
    for(int i=0 ; i<16; i++)
        cout << hex << (int)enc[0][i];

    cout <<endl<< "C2\t: ";
    for(int i=0 ; i<16; i++)
        cout << hex << (int)enc[1][i];

    U8* dec = cpa.Dec(enc);
    for(int i=0; i<16 ;i++)
        cout << dec[i];
    cout << endl;

    return 0;
}