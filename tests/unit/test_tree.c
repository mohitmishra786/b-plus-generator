// tests/unit/test_tree.c
#include <stdio.h>
#include <assert.h>
#include "bplus/tree.h"

void test_tree_creation() {
    BPlusTree* tree = bplus_tree_create(4);
    assert(tree != NULL);
    assert(tree->root != NULL);
    assert(tree->root->is_leaf == true);
    assert(tree->root->num_keys == 0);
    bplus_tree_destroy(tree);
    printf("Tree creation test passed\n");
}

void test_tree_insertion() {
    BPlusTree* tree = bplus_tree_create(4);
    assert(bplus_tree_insert(tree, 10) == true);
    assert(bplus_tree_insert(tree, 20) == true);
    assert(bplus_tree_insert(tree, 5) == true);
    
    assert(bplus_tree_search(tree, 10) == true);
    assert(bplus_tree_search(tree, 20) == true);
    assert(bplus_tree_search(tree, 5) == true);
    assert(bplus_tree_search(tree, 15) == false);
    
    bplus_tree_destroy(tree);
    printf("Tree insertion test passed\n");
}

int main() {
    test_tree_creation();
    test_tree_insertion();
    printf("All tests passed!\n");
    return 0;
}