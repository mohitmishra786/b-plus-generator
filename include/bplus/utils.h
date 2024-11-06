// include/bplus/utils.h
#ifndef BPLUS_UTILS_H
#define BPLUS_UTILS_H

#include "tree.h"

bool save_tree_state(BPlusTree* tree, const char* filename);
BPlusTree* load_tree_state(const char* filename);
void serialize_node(FILE* fp, BPlusNode* node);
BPlusNode* deserialize_node(FILE* fp, int order);

#endif // BPLUS_UTILS_H