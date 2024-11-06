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
        tree = bplus_tree_create(ORDER);
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
    printf("Current B+ Tree structure:\n");
    bplus_tree_print(tree);
}

void run_interactive_mode() {
    initialize_tree();
    char command[256];
    int value;
    
    printf("B+ Tree Interactive Mode\n");
    printf("Commands: insert <value>, search <value>, display, exit\n");
    
    while (1) {
        printf("> ");
        if (fgets(command, sizeof(command), stdin) == NULL) {
            break;
        }
        
        char* cmd = strtok(command, " \n");
        if (!cmd) continue;
        
        if (strcmp(cmd, "exit") == 0) {
            break;
        } else if (strcmp(cmd, "insert") == 0) {
            char* val = strtok(NULL, " \n");
            if (val) {
                value = atoi(val);
                handle_insert(value);
            }
        } else if (strcmp(cmd, "search") == 0) {
            char* val = strtok(NULL, " \n");
            if (val) {
                value = atoi(val);
                handle_search(value);
            }
        } else if (strcmp(cmd, "display") == 0) {
            handle_display();
        } else {
            printf("Unknown command: %s\n", cmd);
        }
    }
    
    cleanup_tree();
}

void run_cli(int argc, char* argv[]) {
    if (strcmp(argv[1], "interactive") == 0) {
        run_interactive_mode();
        return;
    }
    
    initialize_tree();
    
    if (strcmp(argv[1], "insert") == 0 && argc == 3) {
        handle_insert(atoi(argv[2]));
    } else if (strcmp(argv[1], "search") == 0 && argc == 3) {
        handle_search(atoi(argv[2]));
    } else if (strcmp(argv[1], "display") == 0) {
        handle_display();
    } else {
        printf("Invalid command or arguments\n");
    }
    
    cleanup_tree();
}