// src/core/utils.c
#include <stdio.h>
#include <stdlib.h>
#include "bplus/utils.h"
#include "bplus/tree.h"

#define TREE_STATE_FILE "tree_state.bin"

bool save_tree_state(BPlusTree* tree, const char* filename) {
    FILE* fp = fopen(filename, "wb");
    if (!fp) return false;
    
    // Write tree order
    fwrite(&tree->order, sizeof(int), 1, fp);
    
    // Write nodes recursively
    serialize_node(fp, tree->root);
    
    fclose(fp);
    return true;
}

void serialize_node(FILE* fp, BPlusNode* node) {
    if (!node) return;
    
    // Write node metadata
    fwrite(&node->is_leaf, sizeof(bool), 1, fp);
    fwrite(&node->num_keys, sizeof(int), 1, fp);
    
    // Write keys
    fwrite(node->keys, sizeof(int), node->num_keys, fp);
    
    // Write children recursively if not leaf
    if (!node->is_leaf) {
        for (int i = 0; i <= node->num_keys; i++) {
            serialize_node(fp, node->children[i]);
        }
    }
}

BPlusTree* load_tree_state(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) return NULL;
    
    int order;
    fread(&order, sizeof(int), 1, fp);
    
    BPlusTree* tree = bplus_tree_create(order);
    
    // Free the automatically created root
    destroy_node(tree->root);
    
    // Load the actual root
    tree->root = deserialize_node(fp, order);
    
    fclose(fp);
    return tree;
}

BPlusNode* deserialize_node(FILE* fp, int order) {
    bool is_leaf;
    int num_keys;
    
    // Read node metadata
    fread(&is_leaf, sizeof(bool), 1, fp);
    fread(&num_keys, sizeof(int), 1, fp);
    
    BPlusNode* node = create_node(order, is_leaf);
    node->num_keys = num_keys;
    
    // Read keys
    fread(node->keys, sizeof(int), num_keys, fp);
    
    // Read children recursively if not leaf
    if (!is_leaf) {
        for (int i = 0; i <= num_keys; i++) {
            node->children[i] = deserialize_node(fp, order);
        }
    }
    
    return node;
}