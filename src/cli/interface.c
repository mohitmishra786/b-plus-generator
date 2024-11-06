#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bplus/tree.h"
#include "bplus/cli.h"

// Global tree instance for the CLI
static BPlusTree* tree = NULL;
static int tree_order = 4; // Default order

void initialize_tree() {
    if (!tree) {
        tree = bplus_tree_create(tree_order);
    }
}

void cleanup_tree() {
    if (tree) {
        bplus_tree_destroy(tree);
        tree = NULL;
    }
}

void handle_insert(int value) {
    initialize_tree();
    if (bplus_tree_insert(tree, value)) {
        printf("Successfully inserted %d\n", value);
    } else {
        printf("Failed to insert %d\n", value);
    }
}

void handle_search(int value) {
    initialize_tree();
    if (bplus_tree_search(tree, value)) {
        printf("Found %d in the tree\n", value);
    } else {
        printf("Value %d not found in the tree\n", value);
    }
}

void handle_display() {
    initialize_tree();
    bplus_tree_print(tree);
}

void print_help() {
    printf("\nAvailable commands:\n");
    printf("  insert <value>  - Insert a value into the tree\n");
    printf("  search <value>  - Search for a value in the tree\n");
    printf("  delete <value>  - Delete a value from the tree\n");
    printf("  display        - Show the current tree structure\n");
    printf("  help           - Show this help message\n");
    printf("  exit           - Exit the program\n\n");
}

void run_interactive_mode(int order) {
    tree_order = order;
    initialize_tree();
    
    char command[256];
    printf("\nB+ Tree Interactive Mode (Order: %d)\n", order);
    print_help();
    
    while (1) {
        printf("\n> ");
        if (fgets(command, sizeof(command), stdin) == NULL) {
            break;
        }
        
        // Remove newline
        command[strcspn(command, "\n")] = 0;
        
        char* cmd = strtok(command, " ");
        if (!cmd) continue;
        
        if (strcmp(cmd, "exit") == 0) {
            break;
        } else if (strcmp(cmd, "help") == 0) {
            print_help();
        } else if (strcmp(cmd, "insert") == 0) {
            char* val = strtok(NULL, " ");
            if (val) {
                handle_insert(atoi(val));
            } else {
                printf("Usage: insert <value>\n");
            }
        } else if (strcmp(cmd, "search") == 0) {
            char* val = strtok(NULL, " ");
            if (val) {
                handle_search(atoi(val));
            } else {
                printf("Usage: search <value>\n");
            }
        } else if (strcmp(cmd, "delete") == 0) {
            char* val = strtok(NULL, " ");
            if (val) {
                int value = atoi(val);
                if (bplus_tree_delete(tree, value)) {
                    printf("Successfully deleted %d\n", value);
                } else {
                    printf("Value %d not found or deletion failed\n", value);
                }
            } else {
                printf("Usage: delete <value>\n");
            }
        } else if (strcmp(cmd, "display") == 0) {
            handle_display();
        } else {
            printf("Unknown command: %s\n", cmd);
            printf("Type 'help' for available commands\n");
        }
    }
    
    cleanup_tree();
}

void run_cli(int argc, char* argv[]) {
    // Check for order parameter
    if (argc >= 3 && strcmp(argv[1], "order") == 0) {
        tree_order = atoi(argv[2]);
        argc -= 2;
        argv += 2;
    }
    
    // If no more arguments, run interactive mode
    if (argc == 1) {
        run_interactive_mode(tree_order);
        return;
    }
    
    // Handle command-line commands
    initialize_tree();
    
    if (strcmp(argv[1], "interactive") == 0) {
        run_interactive_mode(tree_order);
    } else if (strcmp(argv[1], "insert") == 0 && argc == 3) {
        handle_insert(atoi(argv[2]));
    } else if (strcmp(argv[1], "search") == 0 && argc == 3) {
        handle_search(atoi(argv[2]));
    } else if (strcmp(argv[1], "delete") == 0 && argc == 3) {
        int value = atoi(argv[2]);
        if (bplus_tree_delete(tree, value)) {
            printf("Successfully deleted %d\n", value);
        } else {
            printf("Value %d not found or deletion failed\n", value);
        }
    } else if (strcmp(argv[1], "display") == 0) {
        handle_display();
    } else {
        printf("Invalid command or arguments\n");
        printf("Usage: %s [order <value>] <command> [args]\n", argv[0]);
        printf("Commands: insert <value>, search <value>, delete <value>, display, interactive\n");
    }
    
    cleanup_tree();
}