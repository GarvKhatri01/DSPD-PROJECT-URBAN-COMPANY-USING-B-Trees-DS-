#include "customer.h"
#include <limits.h>

void customerTreeSplitBNode(CustomerBPTree* tree, CustomerBTreeNode* node);

CustomerDataNode* customerTreeNewDataNode() {
    CustomerDataNode* node;
    int i=0;

    node = (CustomerDataNode*)malloc(sizeof(CustomerDataNode));

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

CustomerBTreeNode* customerTreeNewBTreeNode(boolean childIsData) {
    CustomerBTreeNode* node;
    int i=0;

    node = (CustomerBTreeNode*)malloc(sizeof(CustomerBTreeNode));

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

void customerTreeInit(CustomerBPTree* tree) {
    CustomerDataNode* first;
    CustomerBTreeNode* root;

    first = customerTreeNewDataNode();
    root = customerTreeNewBTreeNode(TRUE);

    root->DPArray[0] = first;
    first->parent = root;

    tree->root = root;
    tree->first = first;
}

int customerTreeChildIndex(CustomerBTreeNode* node, int key) {
    int i=0;

    while (i < node->occupancy && key >= node->DArray[i]) {
        i++;
    }

    return i;
}

CustomerDataNode* customerTreeFindDataNode(CustomerBPTree* tree, int key) {
    CustomerDataNode* ret_val=NULL;
    CustomerBTreeNode* curr=tree->root;
    int i;

    while (ret_val == NULL) {
        i = customerTreeChildIndex(curr, key);

        if (curr->childIsData == TRUE) {
            ret_val = curr->DPArray[i];
        }
        else {
            curr = curr->PArray[i];
        }
    }

    return ret_val;
}

Customer* customerTreeSearch(CustomerBPTree* tree, int key) {
    Customer* ret_val=NULL;
    CustomerDataNode* dnode;
    int i=0;
    boolean found=FALSE;

    dnode = customerTreeFindDataNode(tree, key);

    while (i < dnode->occupancy && found == FALSE) {
        if (dnode->keys[i] == key) {
            ret_val = dnode->records[i];
            found = TRUE;
        }
        i++;
    }

    return ret_val;
}

void customerTreeDataInsert(CustomerDataNode* dnode, int key, Customer* record) {
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

void customerTreeSetParent(CustomerBTreeNode* node, int i) {
    if (node->childIsData == TRUE) {
        node->DPArray[i]->parent = node;
    }
    else {
        node->PArray[i]->parent = node;
    }
}

void customerTreeInsertInBNode(CustomerBPTree* tree, CustomerBTreeNode* node, int key, CustomerBTreeNode* bchild, CustomerDataNode* dchild) {
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

    customerTreeSetParent(node, i + 1);

    if (node->occupancy > K - 1) {
        customerTreeSplitBNode(tree, node);
    }
}

void customerTreeSplitDataNode(CustomerBPTree* tree, CustomerDataNode* dnode) {
    CustomerDataNode* right;
    int mid;
    int i;
    int j=0;

    right = customerTreeNewDataNode();
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

    customerTreeInsertInBNode(tree, dnode->parent, right->keys[0], NULL, right);
}

void customerTreeSplitBNode(CustomerBPTree* tree, CustomerBTreeNode* node) {
    CustomerBTreeNode* right;
    CustomerBTreeNode* newRoot;
    int mid;
    int upKey;
    int i;
    int j=0;

    right = customerTreeNewBTreeNode(node->childIsData);
    mid = node->occupancy / 2;
    upKey = node->DArray[mid];

    i = mid + 1;
    while (i < node->occupancy) {
        right->DArray[j] = node->DArray[i];
        right->PArray[j] = node->PArray[i];
        right->DPArray[j] = node->DPArray[i];
        customerTreeSetParent(right, j);
        j++;
        i++;
    }

    right->PArray[j] = node->PArray[node->occupancy];
    right->DPArray[j] = node->DPArray[node->occupancy];
    customerTreeSetParent(right, j);

    right->occupancy = j;
    node->occupancy = mid;

    if (node->parent == NULL) {
        newRoot = customerTreeNewBTreeNode(FALSE);
        newRoot->DArray[0] = upKey;
        newRoot->PArray[0] = node;
        newRoot->PArray[1] = right;
        newRoot->occupancy = 1;

        node->parent = newRoot;
        right->parent = newRoot;
        tree->root = newRoot;
    }
    else {
        customerTreeInsertInBNode(tree, node->parent, upKey, right, NULL);
    }
}

status_code customerTreeInsert(CustomerBPTree* tree, int key, Customer* record) {
    status_code ret_val=FAILURE;
    CustomerDataNode* dnode;

    if (customerTreeSearch(tree, key) == NULL) {
        dnode = customerTreeFindDataNode(tree, key);
        customerTreeDataInsert(dnode, key, record);

        if (dnode->occupancy > C) {
            customerTreeSplitDataNode(tree, dnode);
        }

        ret_val = SUCCESS;
    }

    return ret_val;
}

int customerTreeCount(CustomerBPTree* tree) {
    int ret_val=0;
    CustomerDataNode* curr=tree->first;

    while (curr != NULL) {
        ret_val = ret_val + curr->occupancy;
        curr = curr->next;
    }

    return ret_val;
}

int customerTreeRangeCount(CustomerBPTree* tree, int low, int high) {
    int ret_val=0;
    CustomerDataNode* dnode;
    int i;
    boolean done=FALSE;

    dnode = customerTreeFindDataNode(tree, low);

    while (dnode != NULL && done == FALSE) {
        i = 0;

        while (i < dnode->occupancy && done == FALSE) {
            if (dnode->keys[i] > high) {
                done = TRUE;
            }
            else if (dnode->keys[i] >= low) {
                ret_val++;
            }
            i++;
        }

        dnode = dnode->next;
    }

    return ret_val;
}

int customerTreeFillRange(CustomerBPTree* tree, int low, int high, Customer* out[]) {
    int ret_val=0;
    CustomerDataNode* dnode;
    int i;
    boolean done=FALSE;

    dnode = customerTreeFindDataNode(tree, low);

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

Customer** customerTreeRangeToArray(CustomerBPTree* tree, int low, int high, int* total) {
    Customer** out;
    int size;

    size = customerTreeCount(tree);
    if (size == 0) {
        size = 1;
    }

    out = (Customer**)malloc(size * sizeof(Customer*));

    if (out == NULL) {
        printf("ERROR!!! Memory allocation failed.\n");
        exit(1);
    }

    *total = customerTreeFillRange(tree, low, high, out);

    return out;
}

Customer** customerTreeToArray(CustomerBPTree* tree, int* total) {
    return customerTreeRangeToArray(tree, INT_MIN, INT_MAX, total);
}


//Free
void customerTreeFreeBNode(CustomerBTreeNode* node) {
    int i=0;

    if (node != NULL) {
        if (node->childIsData == FALSE) {
            while (i <= node->occupancy) {
                customerTreeFreeBNode(node->PArray[i]);
                i++;
            }
        }
        free(node);
    }
}

void customerTreeFree(CustomerBPTree* tree) {
    CustomerDataNode* curr=tree->first;
    CustomerDataNode* nptr;

    customerTreeFreeBNode(tree->root);

    while (curr != NULL) {
        nptr = curr->next;
        free(curr);
        curr = nptr;
    }

    tree->root = NULL;
    tree->first = NULL;
}
