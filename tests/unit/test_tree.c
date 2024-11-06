#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "bplus/tree.h"

// Helper function to print tree state
void print_tree_state(BPlusTree* tree) {
    printf("Tree state:\n");
    printf("Order: %d\n", tree->order);
    printf("Root node info:\n");
    printf("  Is leaf: %d\n", tree->root->is_leaf);
    printf("  Num keys: %d\n", tree->root->num_keys);
    printf("  Keys: ");
    for (int i = 0; i < tree->root->num_keys; i++) {
        printf("%d ", tree->root->keys[i]);
    }
    printf("\n");
}

void test_tree_creation() {
    printf("\nRunning tree creation tests...\n");
    
    // Test with minimum valid order (3)
    BPlusTree* tree3 = bplus_tree_create(3);
    assert(tree3 != NULL && "Failed to create tree with order 3");
    assert(tree3->root != NULL && "Root is NULL for order 3 tree");
    assert(tree3->root->is_leaf == true && "Root should be leaf for empty tree");
    assert(tree3->root->num_keys == 0 && "New tree should have 0 keys");
    assert(tree3->order == 3 && "Tree order not set correctly");
    bplus_tree_destroy(tree3);
    
    // Test with typical order (4)
    BPlusTree* tree4 = bplus_tree_create(4);
    assert(tree4 != NULL && "Failed to create tree with order 4");
    assert(tree4->root != NULL && "Root is NULL for order 4 tree");
    assert(tree4->root->is_leaf == true && "Root should be leaf for empty tree");
    assert(tree4->root->num_keys == 0 && "New tree should have 0 keys");
    assert(tree4->order == 4 && "Tree order not set correctly");
    bplus_tree_destroy(tree4);
    
    printf("Tree creation tests passed!\n");
}

void test_basic_insertion() {
    printf("\nRunning basic insertion tests...\n");
    
    BPlusTree* tree = bplus_tree_create(4);
    assert(tree != NULL && "Failed to create tree");
    
    // Test single insertion
    printf("Inserting value 10...\n");
    bool insert_result = bplus_tree_insert(tree, 10);
    assert(insert_result && "Failed to insert value 10");
    assert(tree->root->num_keys == 1 && "Root should have 1 key after insertion");
    assert(tree->root->keys[0] == 10 && "Root's first key should be 10");
    
    // Test multiple insertions
    printf("Inserting values 20 and 5...\n");
    assert(bplus_tree_insert(tree, 20) && "Failed to insert value 20");
    assert(bplus_tree_insert(tree, 5) && "Failed to insert value 5");
    
    // Verify all values are searchable
    printf("Verifying searches...\n");
    assert(bplus_tree_search(tree, 5) && "Failed to find value 5");
    assert(bplus_tree_search(tree, 10) && "Failed to find value 10");
    assert(bplus_tree_search(tree, 20) && "Failed to find value 20");
    
    // Verify non-existent value
    assert(!bplus_tree_search(tree, 15) && "Found non-existent value 15");
    
    bplus_tree_destroy(tree);
    printf("Basic insertion tests passed!\n");
}

void test_complex_insertion() {
    printf("\nRunning complex insertion tests...\n");
    
    BPlusTree* tree = bplus_tree_create(4);
    assert(tree != NULL && "Failed to create tree");
    
    // Insert enough values to cause splits
    int values[] = {10, 20, 30, 40, 50, 25, 35, 45, 5, 15};
    int num_values = sizeof(values) / sizeof(values[0]);
    
    printf("Inserting values: ");
    for (int i = 0; i < num_values; i++) {
        printf("%d ", values[i]);
        bool insert_result = bplus_tree_insert(tree, values[i]);
        if (!insert_result) {
            printf("\nFailed to insert value %d\n", values[i]);
            print_tree_state(tree);
            assert(false && "Insertion failed");
        }
    }
    printf("\n");
    
    // Verify all values are present
    printf("Verifying searches...\n");
    for (int i = 0; i < num_values; i++) {
        bool search_result = bplus_tree_search(tree, values[i]);
        if (!search_result) {
            printf("Failed to find value %d\n", values[i]);
            print_tree_state(tree);
            assert(false && "Search failed");
        }
    }
    
    // Verify structure properties
    assert(tree->root != NULL && "Root became NULL after insertions");
    assert(tree->root->num_keys > 0 && "Root has no keys after insertions");
    
    bplus_tree_destroy(tree);
    printf("Complex insertion tests passed!\n");
}

void test_deletion() {
    printf("\nRunning deletion tests...\n");
    
    BPlusTree* tree = bplus_tree_create(4);
    
    // Insert some values
    int values[] = {10, 20, 30, 40, 50};
    printf("Inserting initial values: ");
    for (int i = 0; i < 5; i++) {
        printf("%d ", values[i]);
        bool insert_result = bplus_tree_insert(tree, values[i]);
        assert(insert_result && "Failed to insert initial value");
    }
    printf("\n");
    
    // Test basic deletion
    printf("Testing deletion of value 30...\n");
    assert(bplus_tree_delete(tree, 30) && "Failed to delete value 30");
    assert(!bplus_tree_search(tree, 30) && "Value 30 still present after deletion");
    
    // Test deleting non-existent value
    printf("Testing deletion of non-existent value 35...\n");
    assert(!bplus_tree_delete(tree, 35) && "Deletion of non-existent value returned true");
    
    // Test deleting first and last values
    printf("Testing deletion of first and last values (10, 50)...\n");
    assert(bplus_tree_delete(tree, 10) && "Failed to delete value 10");
    assert(bplus_tree_delete(tree, 50) && "Failed to delete value 50");
    
    // Verify remaining values
    printf("Verifying remaining values...\n");
    assert(bplus_tree_search(tree, 20) && "Value 20 not found after deletions");
    assert(bplus_tree_search(tree, 40) && "Value 40 not found after deletions");
    
    bplus_tree_destroy(tree);
    printf("Deletion tests passed!\n");
}

void test_tree_suite() {
    printf("Starting B+ Tree unit tests...\n\n");
    
    test_tree_creation();
    test_basic_insertion();
    test_complex_insertion();
    test_deletion();
    
    printf("\nAll B+ Tree unit tests passed!\n");
}