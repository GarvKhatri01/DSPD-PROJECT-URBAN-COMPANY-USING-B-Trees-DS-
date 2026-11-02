#ifndef REQUEST_TREE_H
#define REQUEST_TREE_H

#include "common.h"

struct request_tag;

typedef struct request_dnode_tag {
    int keys[C + 1];
    struct request_tag* records[C + 1];
    int occupancy;
    struct request_dnode_tag* prev;
    struct request_dnode_tag* next;
    struct request_bnode_tag* parent;
} RequestDataNode;

typedef struct request_bnode_tag {
    int DArray[K];
    struct request_bnode_tag* PArray[K + 1];
    RequestDataNode* DPArray[K + 1];
    boolean childIsData;
    int occupancy;
    struct request_bnode_tag* parent;
} RequestBTreeNode;

typedef struct {
    RequestBTreeNode* root;
    RequestDataNode* first;
} RequestBPTree;

void requestTreeInit(RequestBPTree* tree);

status_code requestTreeInsert(RequestBPTree* tree, int key, struct request_tag* record);

struct request_tag* requestTreeSearch(RequestBPTree* tree, int key);

int requestTreeCount(RequestBPTree* tree);

struct request_tag** requestTreeToArray(RequestBPTree* tree, int* total);

struct request_tag** requestTreeRangeToArray(RequestBPTree* tree, int low, int high, int* total);

void requestTreeFree(RequestBPTree* tree);

#endif
