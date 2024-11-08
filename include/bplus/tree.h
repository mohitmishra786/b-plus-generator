// include/bplus/tree.h
#ifndef BPLUS_TREE_H
#define BPLUS_TREE_H

#include <stdbool.h>

typedef struct BPlusNode {
    int* keys;
    struct BPlusNode** children;
    struct BPlusNode* next;
    bool is_leaf;
    int num_keys;
} BPlusNode;

typedef struct {
    BPlusNode* root;
    int order;
} BPlusTree;

// Node operations
BPlusNode* create_node(int order, bool is_leaf);
void destroy_node(BPlusNode* node);

// Tree operations
BPlusTree* bplus_tree_create(int order);
void bplus_tree_destroy(BPlusTree* tree);
bool bplus_tree_insert(BPlusTree* tree, int key);
bool bplus_tree_delete(BPlusTree* tree, int key);
bool bplus_tree_search(BPlusTree* tree, int key);
void bplus_tree_print(BPlusTree* tree);

#endif // BPLUS_TREE_H