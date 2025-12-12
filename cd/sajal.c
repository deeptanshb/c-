#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <openssl/sha.h>
#include <openssl/evp.h>

#define MAX_NODES 100
#define MAX_TRANSACTIONS 10
#define HASH_SIZE 65
#define DIFFICULTY 4
#define MAX_CHAIN_LENGTH 100
#define COOPERATION_REWARD 2
#define DEFECTION_REWARD 3
#define COOPERATION_COST 1
#define SUCKER_PENALTY 0
#define TRUST_THRESHOLD 0.7

typedef struct {
    int sender;
    int receiver;
    int interaction_type; // 0: cooperative, 1: defective
    float amount;
    time_t timestamp;
} Transaction;

typedef struct {
    int index;
    Transaction transactions[MAX_TRANSACTIONS];
    int transaction_count;
    time_t timestamp;
    char previous_hash[HASH_SIZE];
    char hash[HASH_SIZE];
    int nonce;
} Block;

typedef struct {
    int node_id;
    float trust_score;
    int total_interactions;
    int cooperative_actions;
} Node;

typedef struct {
    Block blocks[MAX_CHAIN_LENGTH];
    int length;
} Blockchain;

Node nodes[MAX_NODES];
int node_count = 0;
Blockchain blockchain;
float trust_matrix[MAX_NODES][MAX_NODES];

void init_blockchain();
void init_nodes(int count);
void calculate_hash(Block *block, char *hash_output);
int mine_block(Block *block, int difficulty);
void add_transaction(Block *block, int sender, int receiver, int interaction_type, float amount);
void create_new_block();
void update_trust_scores();
float calculate_payoff(int player1_action, int player2_action, int player_position);
void print_blockchain();
void print_nodes();
void print_trust_matrix();
void simulate_interactions(int num_rounds);
int decide_action(int node1, int node2);
void play_game(int node1, int node2);
char* hash_to_string(unsigned char* hash, char* output);

int main() {
    srand(time(NULL));
    init_blockchain();
    int num_nodes, num_rounds;

    printf("Enter number of nodes in the network (max %d): ", MAX_NODES);
    scanf("%d", &num_nodes);
    if (num_nodes > MAX_NODES || num_nodes < 2) {
        printf("Invalid number of nodes. Setting to default of 10.\n");
        num_nodes = 10;
    }

    printf("Enter number of interaction rounds: ");
    scanf("%d", &num_rounds);

    init_nodes(num_nodes);
    create_new_block();
    simulate_interactions(num_rounds);

    printf("\n--- Final Blockchain State ---\n");
    print_blockchain();
    printf("\n--- Final Node Trust Scores ---\n");
    print_nodes();
    printf("\n--- Trust Matrix ---\n");
    print_trust_matrix();

    return 0;
}

void init_blockchain() {
    blockchain.length = 0;
    for (int i = 0; i < MAX_NODES; i++) {
        for (int j = 0; j < MAX_NODES; j++) {
            trust_matrix[i][j] = 0.5;
        }
    }
}

void init_nodes(int count) {
    node_count = count;
    for (int i = 0; i < count; i++) {
        nodes[i].node_id = i;
        nodes[i].trust_score = 0.5;
        nodes[i].total_interactions = 0;
        nodes[i].cooperative_actions = 0;
    }
}

char* hash_to_string(unsigned char* hash, char* output) {
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(output + (i * 2), "%02x", hash[i]);
    }
    output[64] = '\0';
    return output;
}

#include <openssl/evp.h>  // Required for EVP API

void calculate_hash(Block *block, char *hash_output) {
    EVP_MD_CTX *mdctx;
    unsigned char hash[SHA256_DIGEST_LENGTH];
    unsigned int hash_len;
    char input[2048] = "";

    // Construct input string from block data
    sprintf(input, "%d", block->index);
    for (int i = 0; i < block->transaction_count; i++) {
        char trans_str[256];
        sprintf(trans_str, "%d%d%d%.2f%ld",
                block->transactions[i].sender,
                block->transactions[i].receiver,
                block->transactions[i].interaction_type,
                block->transactions[i].amount,
                block->transactions[i].timestamp);
        strcat(input, trans_str);
    }

    char timestamp_str[20];
    sprintf(timestamp_str, "%ld", block->timestamp);
    strcat(input, timestamp_str);
    strcat(input, block->previous_hash);
    char nonce_str[20];
    sprintf(nonce_str, "%d", block->nonce);
    strcat(input, nonce_str);

    // Modern OpenSSL hash calculation
    mdctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL);
    EVP_DigestUpdate(mdctx, input, strlen(input));
    EVP_DigestFinal_ex(mdctx, hash, &hash_len);
    EVP_MD_CTX_free(mdctx);

    // Convert to string
    hash_to_string(hash, hash_output);
}


int mine_block(Block *block, int difficulty) {
    char target[DIFFICULTY + 1];
    memset(target, '0', difficulty);
    target[difficulty] = '\0';

    block->nonce = 0;
    calculate_hash(block, block->hash);
    while (strncmp(block->hash, target, difficulty) != 0) {
        block->nonce++;
        calculate_hash(block, block->hash);
    }
    printf("Block mined with hash: %s\n", block->hash);
    return 1;
}

void add_transaction(Block *block, int sender, int receiver, int interaction_type, float amount) {
    if (block->transaction_count < MAX_TRANSACTIONS) {
        Transaction transaction = { sender, receiver, interaction_type, amount, time(NULL) };
        block->transactions[block->transaction_count++] = transaction;
        printf("Transaction added: Node %d -> Node %d, Type: %s, Amount: %.2f\n",
            sender, receiver, (interaction_type == 0) ? "Cooperative" : "Defective", amount);
    } else {
        printf("Transaction pool full. Mine a block first.\n");
    }
}

void create_new_block() {
    Block new_block = {0};
    new_block.index = blockchain.length;
    new_block.timestamp = time(NULL);
    strcpy(new_block.previous_hash, blockchain.length > 0 ? blockchain.blocks[blockchain.length - 1].hash : "0");
    mine_block(&new_block, DIFFICULTY);
    blockchain.blocks[blockchain.length++] = new_block;
    printf("New block created with index: %d\n", new_block.index);
}

float calculate_payoff(int a1, int a2, int p) {
    if (p == 1) {
        if (a1 == 0 && a2 == 0) return COOPERATION_REWARD - COOPERATION_COST;
        if (a1 == 0 && a2 == 1) return SUCKER_PENALTY - COOPERATION_COST;
        if (a1 == 1 && a2 == 0) return DEFECTION_REWARD;
        return 0;
    } else {
        if (a1 == 0 && a2 == 0) return COOPERATION_REWARD - COOPERATION_COST;
        if (a1 == 0 && a2 == 1) return DEFECTION_REWARD;
        if (a1 == 1 && a2 == 0) return SUCKER_PENALTY - COOPERATION_COST;
        return 0;
    }
}

void update_trust_scores() {
    for (int i = 0; i < node_count; i++) {
        nodes[i].total_interactions = 0;
        nodes[i].cooperative_actions = 0;
    }

    for (int b = 0; b < blockchain.length; b++) {
        Block *block = &blockchain.blocks[b];
        for (int t = 0; t < block->transaction_count; t++) {
            Transaction *trans = &block->transactions[t];
            nodes[trans->sender].total_interactions++;
            if (trans->interaction_type == 0) {
                nodes[trans->sender].cooperative_actions++;
            }
            float delta = (trans->interaction_type == 0) ? 0.05 : -0.1;
            trust_matrix[trans->receiver][trans->sender] += delta;
            if (trust_matrix[trans->receiver][trans->sender] > 1.0)
                trust_matrix[trans->receiver][trans->sender] = 1.0;
            if (trust_matrix[trans->receiver][trans->sender] < 0.0)
                trust_matrix[trans->receiver][trans->sender] = 0.0;
        }
    }

    for (int i = 0; i < node_count; i++) {
        if (nodes[i].total_interactions > 0) {
            nodes[i].trust_score = (float)nodes[i].cooperative_actions / nodes[i].total_interactions;
        }
    }
}

int decide_action(int node1, int node2) {
    float trust = trust_matrix[node1][node2];
    if (trust > TRUST_THRESHOLD || (rand() % 100) < (int)(trust * 100))
        return 0;
    return 1;
}

void play_game(int node1, int node2) {
    int action1 = decide_action(node1, node2);
    int action2 = decide_action(node2, node1);
    float payoff1 = calculate_payoff(action1, action2, 1);
    float payoff2 = calculate_payoff(action1, action2, 2);
    Block *current_block = &blockchain.blocks[blockchain.length - 1];
    add_transaction(current_block, node1, node2, action1, payoff1);
    add_transaction(current_block, node2, node1, action2, payoff2);
    if (current_block->transaction_count >= MAX_TRANSACTIONS - 1) {
        create_new_block();
    }
}

void simulate_interactions(int num_rounds) {
    printf("\nSimulating %d rounds of interactions...\n", num_rounds);
    for (int round = 0; round < num_rounds; round++) {
        printf("\n--- Round %d ---\n", round + 1);
        for (int i = 0; i < node_count / 2; i++) {
            int node1 = rand() % node_count;
            int node2;
            do {
                node2 = rand() % node_count;
            } while (node2 == node1);
            printf("Interaction between Node %d and Node %d:\n", node1, node2);
            play_game(node1, node2);
        }
        update_trust_scores();
        if ((round + 1) % 5 == 0 || round == num_rounds - 1) {
            printf("\n--- Current Trust Scores ---\n");
            print_nodes();
        }
    }
}

void print_blockchain() {
    printf("Blockchain with %d blocks:\n", blockchain.length);
    for (int i = 0; i < blockchain.length; i++) {
        Block *b = &blockchain.blocks[i];
        printf("Block #%d | Hash: %s | Previous: %s | Transactions: %d\n",
               b->index, b->hash, b->previous_hash, b->transaction_count);
    }
}

void print_nodes() {
    for (int i = 0; i < node_count; i++) {
        printf("Node %d | Trust Score: %.2f | Interactions: %d | Cooperative: %d\n",
               nodes[i].node_id, nodes[i].trust_score,
               nodes[i].total_interactions, nodes[i].cooperative_actions);
    }
}

void print_trust_matrix() {
    for (int i = 0; i < node_count; i++) {
        for (int j = 0; j < node_count; j++) {
            printf("%.2f ", trust_matrix[i][j]);
        }
        printf("\n");
    }
}

