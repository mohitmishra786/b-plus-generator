#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bplus/tree.h"

// Node creation and destruction functions
BPlusNode* create_node(int order, bool is_leaf) {
    BPlusNode* node = (BPlusNode*)malloc(sizeof(BPlusNode));
    node->keys = (int*)malloc(sizeof(int) * (order - 1));
    node->children = (BPlusNode**)malloc(sizeof(BPlusNode*) * order);
    node->is_leaf = is_leaf;
    node->num_keys = 0;
    node->next = NULL;
    
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

// Tree creation and destruction
BPlusTree* bplus_tree_create(int order) {
    BPlusTree* tree = (BPlusTree*)malloc(sizeof(BPlusTree));
    tree->root = create_node(order, true);
    tree->order = order;
    return tree;
}

static void destroy_tree_recursive(BPlusNode* node) {
    if (!node->is_leaf) {
        for (int i = 0; i <= node->num_keys; i++) {
            if (node->children[i]) {
                destroy_tree_recursive(node->children[i]);
            }
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

// Helper functions for insertion
static void insert_into_leaf(BPlusNode* leaf, int key) {
    int i = leaf->num_keys - 1;
    
    while (i >= 0 && leaf->keys[i] > key) {
        leaf->keys[i + 1] = leaf->keys[i];
        i--;
    }
    
    leaf->keys[i + 1] = key;
    leaf->num_keys++;
}

static void split_leaf_node(BPlusNode* parent, int index, BPlusNode* leaf) {
    int order = parent->num_keys + 2;
    BPlusNode* new_leaf = create_node(order, true);
    
    int mid = (leaf->num_keys + 1) / 2;
    
    for (int i = mid; i < leaf->num_keys; i++) {
        new_leaf->keys[new_leaf->num_keys] = leaf->keys[i];
        new_leaf->num_keys++;
    }
    
    leaf->num_keys = mid;
    
    new_leaf->next = leaf->next;
    leaf->next = new_leaf;
    
    for (int i = parent->num_keys; i > index; i--) {
        parent->children[i + 1] = parent->children[i];
        parent->keys[i] = parent->keys[i - 1];
    }
    
    parent->keys[index] = new_leaf->keys[0];
    parent->children[index + 1] = new_leaf;
    parent->num_keys++;
}

static void split_internal_node(BPlusNode* parent, int index, BPlusNode* node) {
    int order = parent->num_keys + 2;
    BPlusNode* new_node = create_node(order, false);
    
    int mid = node->num_keys / 2;
    
    for (int i = mid + 1; i < node->num_keys; i++) {
        new_node->keys[new_node->num_keys] = node->keys[i];
        new_node->children[new_node->num_keys] = node->children[i];
        new_node->num_keys++;
    }
    new_node->children[new_node->num_keys] = node->children[node->num_keys];
    
    node->num_keys = mid;
    
    for (int i = parent->num_keys; i > index; i--) {
        parent->children[i + 1] = parent->children[i];
        parent->keys[i] = parent->keys[i - 1];
    }
    
    parent->keys[index] = node->keys[mid];
    parent->children[index + 1] = new_node;
    parent->num_keys++;
}

static void insert_non_full(BPlusNode* node, int key) {
    int i = node->num_keys - 1;
    
    if (node->is_leaf) {
        insert_into_leaf(node, key);
    } else {
        while (i >= 0 && key < node->keys[i]) {
            i--;
        }
        i++;
        
        if (node->children[i]->num_keys == node->num_keys + 1) {
            if (node->children[i]->is_leaf) {
                split_leaf_node(node, i, node->children[i]);
            } else {
                split_internal_node(node, i, node->children[i]);
            }
            
            if (key > node->keys[i]) {
                i++;
            }
        }
        
        insert_non_full(node->children[i], key);
    }
}

// Insert operation
bool bplus_tree_insert(BPlusTree* tree, int key) {
    BPlusNode* root = tree->root;
    
    if (root->num_keys == tree->order - 1) {
        BPlusNode* new_root = create_node(tree->order, false);
        new_root->children[0] = root;
        tree->root = new_root;
        
        if (root->is_leaf) {
            split_leaf_node(new_root, 0, root);
        } else {
            split_internal_node(new_root, 0, root);
        }
        
        insert_non_full(new_root, key);
    } else {
        insert_non_full(root, key);
    }
    
    return true;
}

// Helper functions for deletion
static int find_key_index(BPlusNode* node, int key) {
    int index = 0;
    while (index < node->num_keys && node->keys[index] < key) {
        ++index;
    }
    return index;
}

static BPlusNode* find_leaf(BPlusNode* root, int key, BPlusNode** parent, int* parent_index) {
    BPlusNode* current = root;
    *parent = NULL;
    
    while (!current->is_leaf) {
        int i = find_key_index(current, key);
        if (i < current->num_keys && current->keys[i] == key) {
            i++;
        }
        *parent = current;
        *parent_index = i;
        current = current->children[i];
    }
    
    return current;
}

static void update_parent_key(BPlusNode* parent, int index, int new_key) {
    if (index > 0) {
        parent->keys[index - 1] = new_key;
    }
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
    
    // Remove the separator key from parent
    for (int i = index; i < parent->num_keys - 1; i++) {
        parent->keys[i] = parent->keys[i + 1];
        parent->children[i + 1] = parent->children[i + 2];
    }
    parent->num_keys--;
    
    destroy_node(right);
}

static void redistribute_nodes(BPlusNode* left, BPlusNode* right, BPlusNode* parent, int index, bool from_left) {
    if (from_left) {
        if (right->is_leaf) {
            // Shift right's keys to make room
            for (int i = right->num_keys; i > 0; i--) {
                right->keys[i] = right->keys[i - 1];
            }
            right->keys[0] = left->keys[left->num_keys - 1];
            parent->keys[index] = right->keys[0];
        } else {
            // For internal nodes
            for (int i = right->num_keys; i > 0; i--) {
                right->keys[i] = right->keys[i - 1];
                right->children[i + 1] = right->children[i];
            }
            right->children[1] = right->children[0];
            right->keys[0] = parent->keys[index];
            parent->keys[index] = left->keys[left->num_keys - 1];
            right->children[0] = left->children[left->num_keys];
        }
        right->num_keys++;
        left->num_keys--;
    } else {
        if (left->is_leaf) {
            left->keys[left->num_keys] = right->keys[0];
            parent->keys[index] = right->keys[1];
            // Shift right's keys
            for (int i = 0; i < right->num_keys - 1; i++) {
                right->keys[i] = right->keys[i + 1];
            }
        } else {
            left->keys[left->num_keys] = parent->keys[index];
            left->children[left->num_keys + 1] = right->children[0];
            parent->keys[index] = right->keys[0];
            // Shift right's keys and children
            for (int i = 0; i < right->num_keys - 1; i++) {
                right->keys[i] = right->keys[i + 1];
                right->children[i] = right->children[i + 1];
            }
            right->children[right->num_keys - 1] = right->children[right->num_keys];
        }
        left->num_keys++;
        right->num_keys--;
    }
}

static bool delete_from_node(BPlusTree* tree, BPlusNode* node, int key, BPlusNode* parent, int parent_index) {
    int min_keys = (tree->order - 1) / 2;
    
    if (node->is_leaf) {
        // Find key in leaf node
        int key_index = find_key_index(node, key);
        if (key_index >= node->num_keys || node->keys[key_index] != key) {
            return false;  // Key not found
        }
        
        // Remove key
        for (int i = key_index; i < node->num_keys - 1; i++) {
            node->keys[i] = node->keys[i + 1];
        }
        node->num_keys--;
        
        // Handle underflow
        if (node != tree->root && node->num_keys < min_keys) {
            // Try to borrow from siblings
            if (parent_index > 0 && parent->children[parent_index - 1]->num_keys > min_keys) {
                redistribute_nodes(parent->children[parent_index - 1], node, parent, parent_index - 1, true);
            } else if (parent_index < parent->num_keys && 
                      parent->children[parent_index + 1]->num_keys > min_keys) {
                redistribute_nodes(node, parent->children[parent_index + 1], parent, parent_index, false);
            } else {
                // Merge with a sibling
                if (parent_index > 0) {
                    merge_nodes(parent->children[parent_index - 1], node, parent, parent_index - 1);
                } else {
                    merge_nodes(node, parent->children[parent_index + 1], parent, parent_index);
                }
            }
        }
        
        // Update parent's key if necessary
        if (parent && key_index == 0 && parent_index > 0) {
            parent->keys[parent_index - 1] = node->keys[0];
        }
    } else {
        // Internal node
        int key_index = find_key_index(node, key);
        BPlusNode* child = node->children[key_index];
        
        bool deleted = delete_from_node(tree, child, key, node, key_index);
        if (!deleted) return false;
        
        // Handle child underflow if necessary
        if (child->num_keys < min_keys) {
            if (key_index > 0 && node->children[key_index - 1]->num_keys > min_keys) {
                redistribute_nodes(node->children[key_index - 1], child, node, key_index - 1, true);
            } else if (key_index < node->num_keys && 
                      node->children[key_index + 1]->num_keys > min_keys) {
                redistribute_nodes(child, node->children[key_index + 1], node, key_index, false);
            } else {
                if (key_index > 0) {
                    merge_nodes(node->children[key_index - 1], child, node, key_index - 1);
                } else {
                    merge_nodes(child, node->children[key_index + 1], node, key_index);
                }
            }
        }
        
        // If root becomes empty, make its only child the new root
        if (node == tree->root && node->num_keys == 0) {
            tree->root = node->children[0];
            destroy_node(node);
        }
    }
    
    return true;
}

// Delete operation
bool bplus_tree_delete(BPlusTree* tree, int key) {
    if (!tree || !tree->root) return false;
    
    BPlusNode* parent;
    int parent_index;
    BPlusNode* leaf = find_leaf(tree->root, key, &parent, &parent_index);
    
    return delete_from_node(tree, leaf, key, parent, parent_index);
}

// Search operation
bool bplus_tree_search(BPlusTree* tree, int key) {
    if (!tree || !tree->root) return false;
    
    BPlusNode* node = tree->root;
    
    // Traverse to leaf node
    while (!node->is_leaf) {
        int i = 0;
        while (i < node->num_keys && key >= node->keys[i]) {
            i++;
        }
        node = node->children[i];
    }
    
    // Search in leaf node
    for (int i = 0; i < node->num_keys; i++) {
        if (node->keys[i] == key) {
            return true;
        }
    }
    return false;
}

// Helper functions for printing
static void print_node(BPlusNode* node, int level) {
    if (!node) return;
    
    // Print indentation
    for (int i = 0; i < level; i++) {
        printf("  ");
    }
    
    // Print keys
    printf("[");
    for (int i = 0; i < node->num_keys; i++) {
        printf("%d", node->keys[i]);
        if (i < node->num_keys - 1) printf(" ");
    }
    printf("] (%s)\n", node->is_leaf ? "leaf" : "internal");
    
    // Recursively print children if not leaf
    if (!node->is_leaf) {
        for (int i = 0; i <= node->num_keys; i++) {
            print_node(node->children[i], level + 1);
        }
    }
}

// Print the entire tree
void bplus_tree_print(BPlusTree* tree) {
    if (!tree || !tree->root) {
        printf("Empty tree\n");
        return;
    }
    
    printf("B+ Tree (order %d):\n", tree->order);
    print_node(tree->root, 0);
    
    // Print leaf node chain
    printf("\nLeaf node chain: ");
    BPlusNode* leaf = tree->root;
    while (!leaf->is_leaf) {
        leaf = leaf->children[0];
    }
    
    while (leaf) {
        printf("[");
        for (int i = 0; i < leaf->num_keys; i++) {
            printf("%d", leaf->keys[i]);
            if (i < leaf->num_keys - 1) printf(" ");
        }
        printf("] -> ");
        leaf = leaf->next;
    }
    printf("NULL\n");
}

// Range search operation
void bplus_tree_range_search(BPlusTree* tree, int start_key, int end_key) {
    if (!tree || !tree->root) return;
    
    BPlusNode* node = tree->root;
    
    // Find the leaf node containing the start key
    while (!node->is_leaf) {
        int i = 0;
        while (i < node->num_keys && start_key >= node->keys[i]) {
            i++;
        }
        node = node->children[i];
    }
    
    // Traverse the leaf nodes and print keys in range
    while (node) {
        for (int i = 0; i < node->num_keys; i++) {
            if (node->keys[i] >= start_key && node->keys[i] <= end_key) {
                printf("%d ", node->keys[i]);
            } else if (node->keys[i] > end_key) {
                return;
            }
        }
        node = node->next;
    }
    printf("\n");
}

// Helper function to validate B+ tree properties
bool bplus_tree_validate(BPlusTree* tree) {
    if (!tree || !tree->root) return true;
    
    // Check if root has at least one key when it's not a leaf
    if (!tree->root->is_leaf && tree->root->num_keys == 0) {
        printf("Validation failed: Root node is empty\n");
        return false;
    }
    
    int min_keys = (tree->order - 1) / 2;
    
    // Helper function to validate a node
    bool validate_node(BPlusNode* node, bool is_root, int* min_key, int* max_key) {
        // Check number of keys
        if (!is_root && node->num_keys < min_keys) {
            printf("Validation failed: Node has too few keys\n");
            return false;
        }
        
        if (node->num_keys >= tree->order) {
            printf("Validation failed: Node has too many keys\n");
            return false;
        }
        
        // Check key ordering
        for (int i = 1; i < node->num_keys; i++) {
            if (node->keys[i] <= node->keys[i-1]) {
                printf("Validation failed: Keys not in order\n");
                return false;
            }
        }
        
        *min_key = node->keys[0];
        *max_key = node->keys[node->num_keys - 1];
        
        // For internal nodes, recursively validate children
        if (!node->is_leaf) {
            for (int i = 0; i <= node->num_keys; i++) {
                if (!node->children[i]) {
                    printf("Validation failed: Missing child pointer\n");
                    return false;
                }
                
                int child_min, child_max;
                if (!validate_node(node->children[i], false, &child_min, &child_max)) {
                    return false;
                }
                
                // Verify key relationships between parent and children
                if (i > 0 && child_min <= node->keys[i-1]) {
                    printf("Validation failed: Child key less than parent separator\n");
                    return false;
                }
                if (i < node->num_keys && child_max >= node->keys[i]) {
                    printf("Validation failed: Child key greater than parent separator\n");
                    return false;
                }
            }
        }
        
        return true;
    }
    
    int min_key, max_key;
    return validate_node(tree->root, true, &min_key, &max_key);
}

// Test function
void bplus_tree_test() {
    BPlusTree* tree = bplus_tree_create(4); // Create a B+ tree of order 4
    
    // Test insertions
    int test_values[] = {5, 8, 1, 7, 3, 12, 9, 6};
    int num_values = sizeof(test_values) / sizeof(test_values[0]);
    
    printf("Inserting values: ");
    for (int i = 0; i < num_values; i++) {
        printf("%d ", test_values[i]);
        bplus_tree_insert(tree, test_values[i]);
    }
    printf("\n\n");
    
    // Print tree structure
    printf("Tree structure after insertions:\n");
    bplus_tree_print(tree);
    printf("\n");
    
    // Test deletions
    printf("Deleting values 3 and 7:\n");
    bplus_tree_delete(tree, 3);
    bplus_tree_delete(tree, 7);
    bplus_tree_print(tree);
    printf("\n");
    
    // Test search
    int search_key = 8;
    printf("Searching for key %d: %s\n", 
           search_key, 
           bplus_tree_search(tree, search_key) ? "Found" : "Not found");
    
    // Test range search
    printf("Range search [5-9]: ");
    bplus_tree_range_search(tree, 5, 9);
    
    // Validate tree
    printf("Tree validation: %s\n", 
           bplus_tree_validate(tree) ? "Passed" : "Failed");
    
    // Clean up
    bplus_tree_destroy(tree);
}