#include "service.h"
#include <limits.h>

void serviceTreeSplitBNode(ServiceBPTree* tree, ServiceBTreeNode* node);

ServiceDataNode* serviceTreeNewDataNode() {
    ServiceDataNode* node;
    int i=0;

    node = (ServiceDataNode*)malloc(sizeof(ServiceDataNode));

    if (node == NULL) {
        printf("ERROR!!! Memory allocation failed for data node.\n");
        exit(1);
    }

    while (i < C + 1) {
        node->keys[i] = 0;
        node->records[i] = NULL;
        i++;
    }

    node->occupancy = 0;
    node->prev = NULL;
    node->next = NULL;
    node->parent = NULL;

    return node;
}


ServiceBTreeNode* serviceTreeNewBTreeNode(boolean childIsData) {
    ServiceBTreeNode* node;
    int i=0;

    node = (ServiceBTreeNode*)malloc(sizeof(ServiceBTreeNode));

    if (node == NULL) {
        printf("ERROR!!! Memory allocation failed for tree node.\n");
        exit(1);
    }

    while (i < K) {
        node->DArray[i] = 0;
        i++;
    }

    i = 0;
    while (i < K + 1) {
        node->PArray[i] = NULL;
        node->DPArray[i] = NULL;
        i++;
    }

    node->childIsData = childIsData;
    node->occupancy = 0;
    node->parent = NULL;

    return node;
}


void serviceTreeInit(ServiceBPTree* tree) {
    ServiceDataNode* first;
    ServiceBTreeNode* root;

    first = serviceTreeNewDataNode();
    root = serviceTreeNewBTreeNode(TRUE);

    root->DPArray[0] = first;
    first->parent = root;

    tree->root = root;
    tree->first = first;
}


int serviceTreeChildIndex(ServiceBTreeNode* node, int key) {
    int i=0;

    while (i < node->occupancy && key >= node->DArray[i]) {
        i++;
    }

    return i;
}


ServiceDataNode* serviceTreeFindDataNode(ServiceBPTree* tree, int key) {
    ServiceDataNode* ret_val=NULL;
    ServiceBTreeNode* curr=tree->root;
    int i;

    while (ret_val == NULL) {
        i = serviceTreeChildIndex(curr, key);

        if (curr->childIsData == TRUE) {
            ret_val = curr->DPArray[i];
        }
        else {
            curr = curr->PArray[i];
        }
    }

    return ret_val;
}


Service* serviceTreeSearch(ServiceBPTree* tree, int key) {
    Service* ret_val=NULL;
    ServiceDataNode* dnode;
    int i=0;
    boolean found=FALSE;

    dnode = serviceTreeFindDataNode(tree, key);

    while (i < dnode->occupancy && found == FALSE) {
        if (dnode->keys[i] == key) {
            ret_val = dnode->records[i];
            found = TRUE;
        }
        i++;
    }

    return ret_val;
}


void serviceTreeDataInsert(ServiceDataNode* dnode, int key, Service* record) {
    int pos=dnode->occupancy;

    while (pos > 0 && dnode->keys[pos-1] > key) {
        dnode->keys[pos] = dnode->keys[pos-1];
        dnode->records[pos] = dnode->records[pos-1];
        pos--;
    }

    dnode->keys[pos] = key;
    dnode->records[pos] = record;
    dnode->occupancy++;
}


void serviceTreeSetParent(ServiceBTreeNode* node, int i) {
    if (node->childIsData == TRUE) {
        node->DPArray[i]->parent = node;
    }
    else {
        node->PArray[i]->parent = node;
    }
}


void serviceTreeInsertInBNode(ServiceBPTree* tree, ServiceBTreeNode* node, int key, ServiceBTreeNode* bchild, ServiceDataNode* dchild) {
    int i=node->occupancy;

    while (i > 0 && node->DArray[i-1] > key) {
        node->DArray[i] = node->DArray[i-1];
        node->PArray[i+1] = node->PArray[i];
        node->DPArray[i+1] = node->DPArray[i];
        i--;
    }

    node->DArray[i] = key;
    node->PArray[i+1] = bchild;
    node->DPArray[i+1] = dchild;
    node->occupancy++;

    serviceTreeSetParent(node, i + 1);

    if (node->occupancy > K - 1) {
        serviceTreeSplitBNode(tree, node);
    }
}


void serviceTreeSplitDataNode(ServiceBPTree* tree, ServiceDataNode* dnode) {
    ServiceDataNode* right;
    int mid;
    int i;
    int j=0;

    right = serviceTreeNewDataNode();
    mid = dnode->occupancy / 2;

    i = mid;
    while (i < dnode->occupancy) {
        right->keys[j] = dnode->keys[i];
        right->records[j] = dnode->records[i];
        j++;
        i++;
    }

    right->occupancy = j;
    dnode->occupancy = mid;

    right->next = dnode->next;
    if (dnode->next != NULL) {
        dnode->next->prev = right;
    }
    right->prev = dnode;
    dnode->next = right;

    serviceTreeInsertInBNode(tree, dnode->parent , right->keys[0], NULL, right);
}


void serviceTreeSplitBNode(ServiceBPTree* tree, ServiceBTreeNode* node) {
    ServiceBTreeNode* right;
    ServiceBTreeNode* newRoot;
    int mid;
    int upKey;
    int i;
    int j=0;

    right = serviceTreeNewBTreeNode(node->childIsData);
    mid = node->occupancy / 2;
    upKey = node->DArray[mid];

    i = mid + 1;
    while (i < node->occupancy) {
        right->DArray[j] = node->DArray[i];
        right->PArray[j] = node->PArray[i];
        right->DPArray[j] = node->DPArray[i];
        serviceTreeSetParent(right, j);
        j++;
        i++;
    }

    right->PArray[j] = node->PArray[node->occupancy];
    right->DPArray[j] = node->DPArray[node->occupancy];
    serviceTreeSetParent(right, j);

    right->occupancy = j;
    node->occupancy = mid;

    if (node->parent == NULL) {
        newRoot = serviceTreeNewBTreeNode(FALSE);
        newRoot->DArray[0] = upKey;
        newRoot->PArray[0] = node;
        newRoot->PArray[1] = right;
        newRoot->occupancy = 1;

        node->parent = newRoot;
        right->parent = newRoot;
        tree->root = newRoot;
    }
    else {
        serviceTreeInsertInBNode(tree, node->parent, upKey, right, NULL);
    }
}


status_code serviceTreeInsert(ServiceBPTree* tree, int key, Service* record) {
    status_code ret_val=FAILURE;
    ServiceDataNode* dnode;

    if (serviceTreeSearch(tree, key) == NULL) {
        dnode = serviceTreeFindDataNode(tree, key);
        serviceTreeDataInsert(dnode, key, record);

        if (dnode->occupancy > C) {
            serviceTreeSplitDataNode(tree, dnode);
        }

        ret_val = SUCCESS;
    }

    return ret_val;
}


int serviceTreeCount(ServiceBPTree* tree) {
    int ret_val=0;
    ServiceDataNode* curr=tree->first;

    while (curr != NULL) {
        ret_val = ret_val + curr->occupancy;
        curr = curr->next;
    }

    return ret_val;
}


int serviceTreeFillRange(ServiceBPTree* tree, int low, int high, Service* out[]) {
    int ret_val=0;
    ServiceDataNode* dnode;
    int i;
    boolean done=FALSE;

    dnode = serviceTreeFindDataNode(tree, low);

    while (dnode != NULL && done == FALSE) {
        i = 0;

        while (i < dnode->occupancy && done == FALSE) {
            if (dnode->keys[i] > high) {
                done = TRUE;
            }
            else if (dnode->keys[i] >= low) {
                out[ret_val] = dnode->records[i];
                ret_val++;
            }
            i++;
        }

        dnode = dnode->next;
    }

    return ret_val;
}


Service** serviceTreeRangeToArray(ServiceBPTree* tree, int low, int high, int* total) {
    Service** out;
    int size;

    size = serviceTreeCount(tree);
    if (size == 0) {
        size = 1;
    }

    out = (Service**)malloc(size * sizeof(Service*));

    if (out == NULL) {
        printf("ERROR!!! Memory allocation failed.\n");
        exit(1);
    }

    *total = serviceTreeFillRange(tree, low, high, out);

    return out;
}


Service** serviceTreeToArray(ServiceBPTree* tree, int* total) {
    return serviceTreeRangeToArray(tree, INT_MIN, INT_MAX, total);
}


void serviceTreeFreeBNode(ServiceBTreeNode* node) {
    int i=0;

    if (node != NULL) {
        if (node->childIsData == FALSE) {
            while (i <= node->occupancy) {
                serviceTreeFreeBNode(node->PArray[i]);
                i++;
            }
        }
        free(node);
    }
}


void serviceTreeFree(ServiceBPTree* tree) {
    ServiceDataNode* curr=tree->first;
    ServiceDataNode* nptr;

    serviceTreeFreeBNode(tree->root);

    while (curr != NULL) {
        nptr = curr->next;
        free(curr);
        curr = nptr;
    }

    tree->root = NULL;
    tree->first = NULL;
}
