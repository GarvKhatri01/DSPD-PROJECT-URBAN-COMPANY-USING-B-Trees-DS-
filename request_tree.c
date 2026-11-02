#include "request.h"
#include <limits.h>

void requestTreeSplitBNode(RequestBPTree* tree, RequestBTreeNode* node);

RequestDataNode* requestTreeNewDataNode() {
    RequestDataNode* node;
    int i=0;

    node = (RequestDataNode*)malloc(sizeof(RequestDataNode));

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

RequestBTreeNode* requestTreeNewBTreeNode(boolean childIsData) {
    RequestBTreeNode* node;
    int i=0;

    node = (RequestBTreeNode*)malloc(sizeof(RequestBTreeNode));

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

void requestTreeInit(RequestBPTree* tree) {
    RequestDataNode* first;
    RequestBTreeNode* root;

    first = requestTreeNewDataNode();
    root = requestTreeNewBTreeNode(TRUE);

    root->DPArray[0] = first;
    first->parent = root;

    tree->root = root;
    tree->first = first;
}

int requestTreeChildIndex(RequestBTreeNode* node, int key) {
    int i=0;

    while (i < node->occupancy && key >= node->DArray[i]) {
        i++;
    }

    return i;
}

RequestDataNode* requestTreeFindDataNode(RequestBPTree* tree, int key) {
    RequestDataNode* ret_val=NULL;
    RequestBTreeNode* curr=tree->root;
    int i;

    while (ret_val == NULL) {
        i = requestTreeChildIndex(curr, key);

        if (curr->childIsData == TRUE) {
            ret_val = curr->DPArray[i];
        }
        else {
            curr = curr->PArray[i];
        }
    }

    return ret_val;
}

Request* requestTreeSearch(RequestBPTree* tree, int key) {
    Request* ret_val=NULL;
    RequestDataNode* dnode;
    int i=0;
    boolean found=FALSE;

    dnode = requestTreeFindDataNode(tree, key);

    while (i < dnode->occupancy && found == FALSE) {
        if (dnode->keys[i] == key) {
            ret_val = dnode->records[i];
            found = TRUE;
        }
        i++;
    }

    return ret_val;
}

void requestTreeDataInsert(RequestDataNode* dnode, int key, Request* record) {
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

void requestTreeSetParent(RequestBTreeNode* node, int i) {
    if (node->childIsData == TRUE) {
        node->DPArray[i]->parent = node;
    }
    else {
        node->PArray[i]->parent = node;
    }
}

void requestTreeInsertInBNode(RequestBPTree* tree, RequestBTreeNode* node, int key, RequestBTreeNode* bchild, RequestDataNode* dchild) {
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

    requestTreeSetParent(node, i + 1);

    if (node->occupancy > K - 1) {
        requestTreeSplitBNode(tree, node);
    }
}

void requestTreeSplitDataNode(RequestBPTree* tree, RequestDataNode* dnode) {
    RequestDataNode* right;
    int mid;
    int i;
    int j=0;

    right = requestTreeNewDataNode();
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

    requestTreeInsertInBNode(tree, dnode->parent, right->keys[0], NULL, right);
}

void requestTreeSplitBNode(RequestBPTree* tree, RequestBTreeNode* node) {
    RequestBTreeNode* right;
    RequestBTreeNode* newRoot;
    int mid;
    int upKey;
    int i;
    int j=0;

    right = requestTreeNewBTreeNode(node->childIsData);
    mid = node->occupancy / 2;
    upKey = node->DArray[mid];

    i = mid + 1;
    while (i < node->occupancy) {
        right->DArray[j] = node->DArray[i];
        right->PArray[j] = node->PArray[i];
        right->DPArray[j] = node->DPArray[i];
        requestTreeSetParent(right, j);
        j++;
        i++;
    }

    right->PArray[j] = node->PArray[node->occupancy];
    right->DPArray[j] = node->DPArray[node->occupancy];
    requestTreeSetParent(right, j);

    right->occupancy = j;
    node->occupancy = mid;

    if (node->parent == NULL) {
        newRoot = requestTreeNewBTreeNode(FALSE);
        newRoot->DArray[0] = upKey;
        newRoot->PArray[0] = node;
        newRoot->PArray[1] = right;
        newRoot->occupancy = 1;

        node->parent = newRoot;
        right->parent = newRoot;
        tree->root = newRoot;
    }
    else {
        requestTreeInsertInBNode(tree, node->parent, upKey, right, NULL);
    }
}

status_code requestTreeInsert(RequestBPTree* tree, int key, Request* record) {
    status_code ret_val=FAILURE;
    RequestDataNode* dnode;

    if (requestTreeSearch(tree, key) == NULL) {
        dnode = requestTreeFindDataNode(tree, key);
        requestTreeDataInsert(dnode, key, record);

        if (dnode->occupancy > C) {
            requestTreeSplitDataNode(tree, dnode);
        }

        ret_val = SUCCESS;
    }

    return ret_val;
}

int requestTreeCount(RequestBPTree* tree) {
    int ret_val=0;
    RequestDataNode* curr=tree->first;

    while (curr != NULL) {
        ret_val = ret_val + curr->occupancy;
        curr = curr->next;
    }

    return ret_val;
}

int requestTreeFillRange(RequestBPTree* tree, int low, int high, Request* out[]) {
    int ret_val=0;
    RequestDataNode* dnode;
    int i;
    boolean done=FALSE;

    dnode = requestTreeFindDataNode(tree, low);

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

Request** requestTreeRangeToArray(RequestBPTree* tree, int low, int high, int* total) {
    Request** out;
    int size;

    size = requestTreeCount(tree);
    if (size == 0) {
        size = 1;
    }

    out = (Request**)malloc(size * sizeof(Request*));

    if (out == NULL) {
        printf("ERROR!!! Memory allocation failed.\n");
        exit(1);
    }

    *total = requestTreeFillRange(tree, low, high, out);

    return out;
}

Request** requestTreeToArray(RequestBPTree* tree, int* total) {
    return requestTreeRangeToArray(tree, INT_MIN, INT_MAX, total);
}

void requestTreeFreeBNode(RequestBTreeNode* node) {
    int i=0;

    if (node != NULL) {
        if (node->childIsData == FALSE) {
            while (i <= node->occupancy) {
                requestTreeFreeBNode(node->PArray[i]);
                i++;
            }
        }
        free(node);
    }
}

void requestTreeFree(RequestBPTree* tree) {
    RequestDataNode* curr=tree->first;
    RequestDataNode* nptr;

    requestTreeFreeBNode(tree->root);

    while (curr != NULL) {
        nptr = curr->next;
        free(curr);
        curr = nptr;
    }

    tree->root = NULL;
    tree->first = NULL;
}
