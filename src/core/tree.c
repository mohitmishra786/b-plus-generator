// src/core/tree.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bplus/tree.h"

static BPlusNode* create_node(int order, bool is_leaf) {
    BPlusNode* node = (BPlusNode*)malloc(sizeof(BPlusNode));
    node->keys = (int*)malloc(sizeof(int) * (order - 1));
    node->children = (BPlusNode**)malloc(sizeof(BPlusNode*) * order);
    node->is_leaf = is_leaf;
    node->num_keys = 0;
    node->next = NULL;
    return node;
}

static void destroy_node(BPlusNode* node) {
    if (node) {
        free(node->keys);
        free(node->children);
        free(node);
    }
}

BPlusTree* bplus_tree_create(int order) {
    BPlusTree* tree = (BPlusTree*)malloc(sizeof(BPlusTree));
    tree->root = create_node(order, true);
    tree->order = order;
    return tree;
}

static void destroy_tree_recursive(BPlusNode* node) {
    if (!node->is_leaf) {
        for (int i = 0; i <= node->num_keys; i++) {
            destroy_tree_recursive(node->children[i]);
        }
    }
    destroy_node(node);
}

void bplus_tree_destroy(BPlusTree* tree) {
    if (tree) {
        destroy_tree_recursive(tree->root);
        free(tree);
    }
}

static void split_child(BPlusNode* parent, int index, BPlusNode* child) {
    BPlusNode* new_node = create_node(parent->num_keys + 1, child->is_leaf);
    
    int mid = (parent->num_keys - 1) / 2;
    
    for (int i = parent->num_keys; i > index; i--) {
        parent->children[i + 1] = parent->children[i];
        parent->keys[i] = parent->keys[i - 1];
    }
    
    parent->children[index + 1] = new_node;
    parent->keys[index] = child->keys[mid];
    parent->num_keys++;
    
    for (int i = mid + 1; i < child->num_keys; i++) {
        new_node->keys[i - (mid + 1)] = child->keys[i];
        if (!child->is_leaf) {
            new_node->children[i - mid] = child->children[i];
        }
    }
    
    if (!child->is_leaf) {
        new_node->children[0] = child->children[mid + 1];
    }
    
    new_node->num_keys = child->num_keys - (mid + 1);
    child->num_keys = mid;
    
    if (child->is_leaf) {
        new_node->next = child->next;
        child->next = new_node;
    }
}

static void insert_non_full(BPlusNode* node, int key) {
    int i = node->num_keys - 1;
    
    if (node->is_leaf) {
        while (i >= 0 && node->keys[i] > key) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key;
        node->num_keys++;
    } else {
        while (i >= 0 && node->keys[i] > key) {
            i--;
        }
        i++;
        
        if (node->children[i]->num_keys == (node->num_keys - 1)) {
            split_child(node, i, node->children[i]);
            if (key > node->keys[i]) {
                i++;
            }
        }
        insert_non_full(node->children[i], key);
    }
}

bool bplus_tree_insert(BPlusTree* tree, int key) {
    BPlusNode* root = tree->root;
    
    if (root->num_keys == (tree->order - 1)) {
        BPlusNode* new_root = create_node(tree->order, false);
        new_root->children[0] = root;
        tree->root = new_root;
        split_child(new_root, 0, root);
        insert_non_full(new_root, key);
    } else {
        insert_non_full(root, key);
    }
    
    return true;
}

bool bplus_tree_search(BPlusTree* tree, int key) {
    BPlusNode* node = tree->root;
    
    while (!node->is_leaf) {
        int i = 0;
        while (i < node->num_keys && key >= node->keys[i]) {
            i++;
        }
        node = node->children[i];
    }
    
    for (int i = 0; i < node->num_keys; i++) {
        if (node->keys[i] == key) {
            return true;
        }
    }
    return false;
}

static void print_node(BPlusNode* node, int level) {
    printf("Level %d: ", level);
    for (int i = 0; i < node->num_keys; i++) {
        printf("%d ", node->keys[i]);
    }
    printf("\n");
    
    if (!node->is_leaf) {
        for (int i = 0; i <= node->num_keys; i++) {
            print_node(node->children[i], level + 1);
        }
    }
}

void bplus_tree_print(BPlusTree* tree) {
    print_node(tree->root, 0);
}

bool bplus_tree_delete(BPlusTree* tree, int key) {
    // TODO: Implement delete operation
    return false;
}