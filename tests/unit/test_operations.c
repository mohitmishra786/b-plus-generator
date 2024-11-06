#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "bplus/tree.h"
#include "bplus/utils.h"

void test_node_operations() {
    printf("Running node operations tests...\n");
    
    // Test node creation
    BPlusNode* leaf = create_node(4, true);
    assert(leaf != NULL);
    assert(leaf->is_leaf == true);
    assert(leaf->num_keys == 0);
    
    BPlusNode* internal = create_node(4, false);
    assert(internal != NULL);
    assert(internal->is_leaf == false);
    assert(internal->num_keys == 0);
    
    // Test node destruction
    destroy_node(leaf);
    destroy_node(internal);
    
    printf("Node operations tests passed!\n");
}

void test_tree_persistence() {
    printf("Running tree persistence tests...\n");
    
    // Create and populate a tree
    BPlusTree* original = bplus_tree_create(4);
    int values[] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; i++) {
        bplus_tree_insert(original, values[i]);
    }
    
    // Save tree state
    assert(save_tree_state(original, "test_tree.bin") == true);
    
    // Load tree state
    BPlusTree* loaded = load_tree_state("test_tree.bin");
    assert(loaded != NULL);
    
    // Verify loaded tree has same values
    for (int i = 0; i < 5; i++) {
        assert(bplus_tree_search(loaded, values[i]) == true);
    }
    
    // Cleanup
    bplus_tree_destroy(original);
    bplus_tree_destroy(loaded);
    remove("test_tree.bin");  // Delete test file
    
    printf("Tree persistence tests passed!\n");
}

void test_tree_operations() {
    printf("Running tree operations tests...\n");
    
    BPlusTree* tree = bplus_tree_create(4);
    
    // Test empty tree properties
    assert(bplus_tree_search(tree, 10) == false);
    assert(bplus_tree_delete(tree, 10) == false);
    
    // Test operations sequence
    assert(bplus_tree_insert(tree, 10) == true);
    assert(bplus_tree_search(tree, 10) == true);
    assert(bplus_tree_delete(tree, 10) == true);
    assert(bplus_tree_search(tree, 10) == false);
    
    bplus_tree_destroy(tree);
    printf("Tree operations tests passed!\n");
}

void test_operations_suite() {
    printf("Starting operations tests...\n\n");
    
    test_node_operations();
    test_tree_persistence();
    test_tree_operations();
    
    printf("All operations tests passed!\n");
}