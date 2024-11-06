// src/core/tree.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bplus/tree.h"

BPlusNode* create_node(int order, bool is_leaf) {
    BPlusNode* node = (BPlusNode*)malloc(sizeof(BPlusNode));
    node->keys = (int*)malloc(sizeof(int) * (order - 1));
    node->children = (BPlusNode**)malloc(sizeof(BPlusNode*) * order);
    node->is_leaf = is_leaf;
    node->num_keys = 0;
    node->next = NULL;
    
    // Initialize all children pointers to NULL
    for (int i = 0; i < order; i++) {
        node->children[i] = NULL;
    }
    
    return node;
}


void destroy_node(BPlusNode* node) {
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
    int order = parent->num_keys + 2; // Calculate order from parent's max keys + 1
    BPlusNode* new_node = create_node(order, child->is_leaf);
    
    int mid = (order - 1) / 2;
    
    // Move the latter half of child's keys to new_node
    for (int i = 0; i < child->num_keys - mid - (child->is_leaf ? 0 : 1); i++) {
        new_node->keys[i] = child->keys[i + mid + (child->is_leaf ? 0 : 1)];
    }
    
    // If not a leaf, move the corresponding children
    if (!child->is_leaf) {
        for (int i = 0; i <= child->num_keys - mid - 1; i++) {
            new_node->children[i] = child->children[i + mid + 1];
        }
    }
    
    new_node->num_keys = child->num_keys - mid - (child->is_leaf ? 0 : 1);
    child->num_keys = mid;
    
    // Shift parent's keys and children to make room
    for (int i = parent->num_keys; i > index; i--) {
        parent->children[i + 1] = parent->children[i];
        parent->keys[i] = parent->keys[i - 1];
    }
    
    // Set the parent's new key and child
    parent->children[index + 1] = new_node;
    parent->keys[index] = child->keys[mid];
    parent->num_keys++;
    
    // If these are leaf nodes, update the linked list
    if (child->is_leaf) {
        new_node->next = child->next;
        child->next = new_node;
    }
}

static void insert_non_full(BPlusNode* node, int key) {
    int i = node->num_keys - 1;
    
    if (node->is_leaf) {
        // Find the position to insert
        while (i >= 0 && node->keys[i] > key) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key;
        node->num_keys++;
    } else {
        // Find the child where key should go
        while (i >= 0 && node->keys[i] > key) {
            i--;
        }
        i++;
        
        // If child is full, split it
        if (node->children[i]->num_keys == (2 * node->children[i]->num_keys)) {
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
    
    // If root is full, create new root
    if (root->num_keys == tree->order - 1) {
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
    if (!node) return;
    
    printf("Level %d: ", level);
    
    // Print keys
    for (int i = 0; i < node->num_keys; i++) {
        printf("%d ", node->keys[i]);
    }
    printf("(%s)", node->is_leaf ? "leaf" : "internal");
    printf("\n");
    
    // If not a leaf, print all children
    if (!node->is_leaf) {
        for (int i = 0; i <= node->num_keys; i++) {
            if (node->children[i]) {
                print_node(node->children[i], level + 1);
            }
        }
    }
}

void bplus_tree_print(BPlusTree* tree) {
    if (!tree || !tree->root) {
        printf("Empty tree\n");
        return;
    }
    
    printf("B+ Tree (Order %d):\n", tree->order);
    print_node(tree->root, 0);
}

static BPlusNode* find_leaf(BPlusNode* root, int key) {
    BPlusNode* node = root;
    while (!node->is_leaf) {
        int i = 0;
        while (i < node->num_keys && key >= node->keys[i]) {
            i++;
        }
        node = node->children[i];
    }
    return node;
}

static int find_key_index(BPlusNode* node, int key) {
    int i = 0;
    while (i < node->num_keys && node->keys[i] < key) {
        i++;
    }
    return i;
}

static void merge_nodes(BPlusNode* left, BPlusNode* right, BPlusNode* parent, int index) {
    // Copy keys and children from right to left
    for (int i = 0; i < right->num_keys; i++) {
        left->keys[left->num_keys + i] = right->keys[i];
        if (!left->is_leaf) {
            left->children[left->num_keys + i + 1] = right->children[i + 1];
        }
    }
    
    if (!left->is_leaf) {
        left->children[left->num_keys] = right->children[0];
    } else {
        left->next = right->next;
    }
    
    left->num_keys += right->num_keys;
    
    // Remove the separator from parent
    for (int i = index; i < parent->num_keys - 1; i++) {
        parent->keys[i] = parent->keys[i + 1];
        parent->children[i + 1] = parent->children[i + 2];
    }
    parent->num_keys--;
    
    destroy_node(right);
}

static void redistribute_nodes(BPlusNode* left, BPlusNode* right, BPlusNode* parent, int index, bool is_left_sibling) {
    if (is_left_sibling) {
        // Move the last key from left to right
        for (int i = right->num_keys; i > 0; i--) {
            right->keys[i] = right->keys[i - 1];
            if (!right->is_leaf) {
                right->children[i + 1] = right->children[i];
            }
        }
        if (!right->is_leaf) {
            right->children[1] = right->children[0];
            right->children[0] = left->children[left->num_keys];
        }
        right->keys[0] = left->keys[left->num_keys - 1];
        parent->keys[index] = right->keys[0];
        left->num_keys--;
    } else {
        // Move the first key from right to left
        left->keys[left->num_keys] = right->keys[0];
        if (!left->is_leaf) {
            left->children[left->num_keys + 1] = right->children[0];
        }
        parent->keys[index] = right->keys[1];
        for (int i = 0; i < right->num_keys - 1; i++) {
            right->keys[i] = right->keys[i + 1];
            if (!right->is_leaf) {
                right->children[i] = right->children[i + 1];
            }
        }
        if (!right->is_leaf) {
            right->children[right->num_keys - 1] = right->children[right->num_keys];
        }
        right->num_keys--;
    }
}

bool bplus_tree_delete(BPlusTree* tree, int key) {
    BPlusNode* leaf = find_leaf(tree->root, key);
    int min_keys = (tree->order - 1) / 2;
    
    // Find the key in leaf node
    int pos = find_key_index(leaf, key);
    if (pos >= leaf->num_keys || leaf->keys[pos] != key) {
        return false;  // Key not found
    }
    
    // Remove key from leaf
    for (int i = pos; i < leaf->num_keys - 1; i++) {
        leaf->keys[i] = leaf->keys[i + 1];
    }
    leaf->num_keys--;
    
    // If root is leaf and empty, tree becomes empty
    if (leaf == tree->root && leaf->num_keys == 0) {
        destroy_node(leaf);
        tree->root = create_node(tree->order, true);
        return true;
    }
    
    // If leaf has enough keys, we're done
    if (leaf->num_keys >= min_keys || leaf == tree->root) {
        return true;
    }
    
    // Need to rebalance tree
    BPlusNode* parent = tree->root;
    BPlusNode* current = tree->root;
    int parent_pos = 0;
    
    // Find parent of the leaf
    while (!current->is_leaf) {
        parent = current;
        int i = 0;
        while (i < current->num_keys && key >= current->keys[i]) {
            i++;
        }
        parent_pos = i;
        current = current->children[i];
    }
    
    // Try to borrow from siblings
    if (parent_pos > 0 && parent->children[parent_pos - 1]->num_keys > min_keys) {
        redistribute_nodes(parent->children[parent_pos - 1], leaf, parent, parent_pos - 1, true);
    } else if (parent_pos < parent->num_keys && parent->children[parent_pos + 1]->num_keys > min_keys) {
        redistribute_nodes(leaf, parent->children[parent_pos + 1], parent, parent_pos, false);
    } else {
        // Merge with a sibling
        if (parent_pos > 0) {
            merge_nodes(parent->children[parent_pos - 1], leaf, parent, parent_pos - 1);
        } else {
            merge_nodes(leaf, parent->children[parent_pos + 1], parent, parent_pos);
        }
        
        // If root becomes empty, make its only child the new root
        if (parent == tree->root && parent->num_keys == 0) {
            tree->root = parent->children[0];
            destroy_node(parent);
        }
    }
    
    return true;
}