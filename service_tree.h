#ifndef SERVICE_TREE_H
#define SERVICE_TREE_H

#include "common.h"

struct service_tag;

typedef struct service_dnode_tag {
    int keys[C + 1];
    struct service_tag* records[C + 1];
    int occupancy;
    struct service_dnode_tag* prev;
    struct service_dnode_tag* next;
    struct service_bnode_tag* parent;
} ServiceDataNode;

typedef struct service_bnode_tag {
    int DArray[K];
    struct service_bnode_tag* PArray[K + 1];
    ServiceDataNode* DPArray[K + 1];
    boolean childIsData;
    int occupancy;
    struct service_bnode_tag* parent;
} ServiceBTreeNode;

typedef struct {
    ServiceBTreeNode* root;
    ServiceDataNode* first;
} ServiceBPTree;

void serviceTreeInit(ServiceBPTree* tree);

status_code serviceTreeInsert(ServiceBPTree* tree, int key, struct service_tag* record);

struct service_tag* serviceTreeSearch(ServiceBPTree* tree, int key);

int serviceTreeCount(ServiceBPTree* tree);

struct service_tag** serviceTreeToArray(ServiceBPTree* tree, int* total);

struct service_tag** serviceTreeRangeToArray(ServiceBPTree* tree, int low, int high, int* total);

void serviceTreeFree(ServiceBPTree* tree);

#endif
