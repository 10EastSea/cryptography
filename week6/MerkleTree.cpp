#include <iostream>
#include <vector>
#include <cmath>
#include <openssl/bn.h>
#include <openssl/sha.h>

#define DEPTH 4
using namespace std;
typedef unsigned char U8;

void BN_printf(const BIGNUM* input) {
	char* c = BN_bn2hex(input);
	printf("%s", c);
}

class MerkleTree {
    BIGNUM* root;
    vector<BIGNUM*> tx_list;
public:
    MerkleTree() {
        random_tx_generate(); // tx_list 생성
        update(); // root 생성
    }
    ~MerkleTree() {
        tx_list.clear();
        tx_list.shrink_to_fit();
        free(root);
    }

    // tx을 랜덤으로 생성해서 tx_list에 추가해주는 함수
    void random_tx_generate() {
        BIGNUM* upper_bound = BN_new();
        BN_generate_prime_ex(upper_bound, 256, 0, NULL, NULL, NULL);
        for(int i=(int)pow(2, DEPTH); i>0; i--) {
            BIGNUM* random_tx = BN_new();
            BN_rand_range(random_tx, upper_bound);
            tx_list.push_back(random_tx);
        }

        printf("[Transaction list]\n");
        int tx_idx = 1;
        for(auto tx=tx_list.begin(); tx!=tx_list.end(); tx++) {
            printf("tx%3d:  ", tx_idx); BN_printf(*tx); printf("\n");
            tx_idx++;
        }
    }

    void update() {
        root = BN_new();
        
        vector<BIGNUM*> internal_node_list; // tx_list 값 deep copy
        for(auto tx=tx_list.begin(); tx!=tx_list.end(); tx++) {
            BIGNUM* tmp_tx = BN_new();
            BN_copy(tmp_tx, *tx);
            internal_node_list.push_back(tmp_tx);
        }

        int level_cnt = (int)pow(2, DEPTH-1); int tmp_level_cnt = level_cnt; int rev_cnt = 1;
        printf("\n[Internal node level_cnt = %d]\n", level_cnt);
        while(internal_node_list.size() != 1) { // 1개만 남을때 까지 반복 -> 그 한개가 root가 됨
            BIGNUM* left_node = internal_node_list.front(); internal_node_list.erase(internal_node_list.begin());
            BIGNUM* right_node = internal_node_list.front(); internal_node_list.erase(internal_node_list.begin());
            BIGNUM* new_node = BN_new();

            SHA256_CTX sha_ctx = {0};
            U8 digest[SHA256_DIGEST_LENGTH] = {0};
            U8* left_str = (U8*)calloc(BN_num_bytes(left_node), sizeof(U8)); BN_bn2bin(left_node, left_str);
            U8* right_str = (U8*)calloc(BN_num_bytes(right_node), sizeof(U8)); BN_bn2bin(right_node, right_str);
            
            SHA256_Init(&sha_ctx);
            SHA256_Update(&sha_ctx, left_str, BN_num_bytes(left_node));
            SHA256_Update(&sha_ctx, right_str, BN_num_bytes(right_node));
            SHA256_Final(digest, &sha_ctx);

            BN_bin2bn(digest, SHA256_DIGEST_LENGTH, new_node); // new_node = H(left_node | right_node)
            internal_node_list.push_back(new_node);

            // internal node 출력 코드
            if(tmp_level_cnt == 0) {
                level_cnt /= 2; tmp_level_cnt = level_cnt; rev_cnt = 1;
                printf("\n[Internal node level_cnt = %d]\n", level_cnt);
            }
            printf("node%2d: ", rev_cnt); BN_printf(new_node); printf("\n");
            tmp_level_cnt--; rev_cnt++;

            // 메모리 free 코드
            free(left_node); free(left_str);
            free(right_node); free(right_str);
        }

        root = internal_node_list.front(); internal_node_list.erase(internal_node_list.begin()); // root 세팅
        printf("\n[Root node]\n"); BN_printf(root); printf("\n");
    }
};

int main() {
    MerkleTree mt = MerkleTree();
    return 0;
}
