#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bplus/tree.h"
#include "bplus/cli.h"

int main(int argc, char* argv[]) {
    if (argc == 1) {
        printf("Usage: b-plus-tree [order <value>] <command> [args]\n");
        printf("Commands:\n");
        printf(" order <value> - Set the order of the B+ tree (optional, default: 4)\n");
        printf(" insert <value> - Insert a value into the tree\n");
        printf(" delete <value> - Delete a value from the tree\n");
        printf(" search <value> - Search for a value in the tree\n");
        printf(" display - Display the current tree\n");
        printf(" interactive - Enter interactive mode\n");
        return 1;
    }
    
    run_cli(argc, argv);
    return 0;
}