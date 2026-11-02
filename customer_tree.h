#ifndef CUSTOMER_TREE_H
#define CUSTOMER_TREE_H

#include "common.h"

struct customer_tag;

typedef struct customer_dnode_tag {
    int keys[C + 1];
    struct customer_tag* records[C + 1];
    int occupancy;
    struct customer_dnode_tag* prev;
    struct customer_dnode_tag* next;
    struct customer_bnode_tag* parent;
} CustomerDataNode;


typedef struct customer_bnode_tag {
    int DArray[K];
    struct customer_bnode_tag* PArray[K + 1];
    CustomerDataNode* DPArray[K + 1];
    boolean childIsData;
    int occupancy;
    struct customer_bnode_tag* parent;
} CustomerBTreeNode;


typedef struct {
    CustomerBTreeNode* root;
    CustomerDataNode* first;
} CustomerBPTree;


void customerTreeInit(CustomerBPTree* tree);

status_code customerTreeInsert(CustomerBPTree* tree, int key, struct customer_tag* record);

struct customer_tag* customerTreeSearch(CustomerBPTree* tree, int key);

int customerTreeCount(CustomerBPTree* tree);

int customerTreeRangeCount(CustomerBPTree* tree, int low, int high);

struct customer_tag** customerTreeToArray(CustomerBPTree* tree, int* total);

struct customer_tag** customerTreeRangeToArray(CustomerBPTree* tree, int low, int high, int* total);

void customerTreeFree(CustomerBPTree* tree);

#endif
