#include <stdio.h>
#include "bplus/tree.h"
#include "bplus/cli.h"

// Declare the test suite functions
void test_tree_suite(void);
void test_operations_suite(void);
void test_cli_suite(void);

int main() {
    printf("\n=== Running All B+ Tree Tests ===\n\n");
    
    printf("Running Tree Tests...\n");
    printf("------------------------\n");
    test_tree_suite();
    
    printf("\nRunning Operations Tests...\n");
    printf("---------------------------\n");
    test_operations_suite();
    
    printf("\nRunning CLI Tests...\n");
    printf("-------------------\n");
    test_cli_suite();
    
    printf("\n=== All Tests Completed Successfully ===\n\n");
    return 0;
}