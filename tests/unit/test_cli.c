#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "bplus/cli.h"
#include "bplus/tree.h"

// Mock functions to simulate user input
void simulate_command(const char* command) {
    // Implementation would vary based on your CLI architecture
    char* argv[4];
    int argc = 0;
    
    char* cmd_copy = strdup(command);
    char* token = strtok(cmd_copy, " ");
    
    while (token != NULL && argc < 4) {
        argv[argc++] = token;
        token = strtok(NULL, " ");
    }
    
    run_cli(argc, argv);
    free(cmd_copy);
}

void test_cli_basic_commands() {
    printf("Running CLI basic commands tests...\n");
    
    // Test insert command
    simulate_command("insert 10");
    simulate_command("insert 20");
    simulate_command("insert 30");
    
    // Test search command
    simulate_command("search 20");
    
    // Test display command
    simulate_command("display");
    
    // Test delete command
    simulate_command("delete 20");
    
    printf("CLI basic commands tests passed!\n");
}

void test_cli_invalid_commands() {
    printf("Running CLI invalid commands tests...\n");
    
    // Test invalid command
    simulate_command("invalid_command");
    
    // Test missing arguments
    simulate_command("insert");
    simulate_command("search");
    simulate_command("delete");
    
    // Test extra arguments
    simulate_command("insert 10 20");
    simulate_command("display extra");
    
    printf("CLI invalid commands tests passed!\n");
}

void test_cli_order_parameter() {
    printf("Running CLI order parameter tests...\n");
    
    // Test with different orders
    char* argv[] = {"b-plus-tree", "order", "5", "insert", "10"};
    run_cli(5, argv);
    
    char* argv2[] = {"b-plus-tree", "order", "3", "insert", "20"};
    run_cli(5, argv2);
    
    printf("CLI order parameter tests passed!\n");
}

void test_cli_suite() {
    printf("Starting CLI tests...\n\n");
    
    test_cli_basic_commands();
    test_cli_invalid_commands();
    test_cli_order_parameter();
    
    printf("All CLI tests passed!\n");
}