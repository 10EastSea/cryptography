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
    void Gen() {
		unsigned char user_key[byteSize];
        random_device rd; mt19937 gen(rd());
        uniform_int_distribution<int> dis(0, 255); // byte 단위라서 0000 0000 ~ 1111 1111 이 중 랜덤 값

        for(int i=0 ; i<byteSize; i++) user_key[i] = dis(gen) & 0xff;
    	AES_set_encrypt_key(user_key, (int)bitsize, &encKey); // encKey만 생성
    }
public:
    CPA() { Gen(); }
    
    U8** Enc(U8* msg) {
        int i;
	    U8 **c = (U8**)calloc(2, sizeof(U8*)); // C = [r, F_k(r)]
	    for(i = 0; i < 2; i++) c[i] = (U8*)calloc(byteSize, sizeof(U8)); // 16 bytes cipher text
        
		// TODO
        random_device rd; mt19937 gen(rd());
        uniform_int_distribution<int> dis(0, 255);
		
        for(int i=0 ; i<byteSize; i++) c[0][i] = dis(gen) & 0xff; // c[0] = random r
		AES_encrypt(c[0], c[1], &encKey); // r: c[0] -> fkr: c[1]

		cout << "r\t: ";
    	for(int i=0 ; i<16; i++) cout << hex << (int)c[0][i];
		cout << endl << "fkr\t: ";
    	for(int i=0 ; i<16; i++) cout << hex << (int)c[1][i];
		cout << endl;

		BIGNUM *result = BN_new();
		BIGNUM *r = BN_new(); BN_bin2bn(c[0], (int)byteSize, r);
		BIGNUM *fkr = BN_new(); BN_bin2bn(c[1], (int)byteSize, fkr);
		BIGNUM *m = BN_new(); BN_bin2bn(msg, (int)byteSize, m);
		BN_xor(result, bitsize, fkr, m); 
		BN_bn2bin(result, c[1]); // c[1] = fkr XOR m

        return c;
    }

    U8* Dec(U8** c) {
	    U8 *M = (U8*)calloc(byteSize, sizeof(U8));

		// TODO
		AES_encrypt(c[0], c[0], &encKey); // c1: c[0] -> fkc: c[0]

		cout << "fkc\t: ";
    	for(int i=0 ; i<16; i++) cout << hex << (int)c[0][i];
		cout << endl;

		BIGNUM *result = BN_new();
		BIGNUM *c2 = BN_new(); BN_bin2bn(c[1], (int)byteSize, c2);
		BIGNUM *fkc = BN_new(); BN_bin2bn(c[0], (int)byteSize, fkc);
		BN_xor(result, bitsize, c2, fkc);
		BN_bn2bin(result, M); // M = c2 XOR fkc

        return M;
    } 
};

int main(){
    U8 msg[16] = "CPA-secure";
    CPA cpa = CPA();

    cout << "msg\t: " << msg << endl;

	// Encryption
    U8** enc = cpa.Enc(msg);

    cout << "C1\t: ";
    for(int i=0 ; i<16; i++) cout << hex << (int)enc[0][i];
    cout << endl << "C2\t: ";
    for(int i=0 ; i<16; i++) cout << hex << (int)enc[1][i];
	cout << endl;

	// Decryption
    U8* dec = cpa.Dec(enc);
	
	cout << "Dec\t: ";
    for(int i=0; i<16 ;i++) cout << dec[i];
    cout << endl;

    return 0;
}