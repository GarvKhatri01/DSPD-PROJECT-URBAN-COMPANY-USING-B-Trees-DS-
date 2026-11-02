#include "professional.h"
#include <limits.h>

void professionalTreeSplitBNode(ProfessionalBPTree* tree, ProfessionalBTreeNode* node);

ProfessionalDataNode* professionalTreeNewDataNode() {
    ProfessionalDataNode* node;
    int i=0;

    node = (ProfessionalDataNode*)malloc(sizeof(ProfessionalDataNode));

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

ProfessionalBTreeNode* professionalTreeNewBTreeNode(boolean childIsData) {
    ProfessionalBTreeNode* node;
    int i=0;

    node = (ProfessionalBTreeNode*)malloc(sizeof(ProfessionalBTreeNode));

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

void professionalTreeInit(ProfessionalBPTree* tree) {
    ProfessionalDataNode* first;
    ProfessionalBTreeNode* root;

    first = professionalTreeNewDataNode();
    root = professionalTreeNewBTreeNode(TRUE);

    root->DPArray[0] = first;
    first->parent = root;

    tree->root = root;
    tree->first = first;
}

int professionalTreeChildIndex(ProfessionalBTreeNode* node, int key) {
    int i=0;

    while (i < node->occupancy && key >= node->DArray[i]) {
        i++;
    }

    return i;
}

ProfessionalDataNode* professionalTreeFindDataNode(ProfessionalBPTree* tree, int key) {
    ProfessionalDataNode* ret_val=NULL;
    ProfessionalBTreeNode* curr=tree->root;
    int i;

    while (ret_val == NULL) {
        i = professionalTreeChildIndex(curr, key);

        if (curr->childIsData == TRUE) {
            ret_val = curr->DPArray[i];
        }
        else {
            curr = curr->PArray[i];
        }
    }

    return ret_val;
}

Professional* professionalTreeSearch(ProfessionalBPTree* tree, int key) {
    Professional* ret_val=NULL;
    ProfessionalDataNode* dnode;
    int i=0;
    boolean found=FALSE;

    dnode = professionalTreeFindDataNode(tree, key);

    while (i < dnode->occupancy && found == FALSE) {
        if (dnode->keys[i] == key) {
            ret_val = dnode->records[i];
            found = TRUE;
        }
        i++;
    }

    return ret_val;
}

void professionalTreeDataInsert(ProfessionalDataNode* dnode, int key, Professional* record) {
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

void professionalTreeSetParent(ProfessionalBTreeNode* node, int i) {
    if (node->childIsData == TRUE) {
        node->DPArray[i]->parent = node;
    }
    else {
        node->PArray[i]->parent = node;
    }
}

void professionalTreeInsertInBNode(ProfessionalBPTree* tree, ProfessionalBTreeNode* node, int key, ProfessionalBTreeNode* bchild, ProfessionalDataNode* dchild) {
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

    professionalTreeSetParent(node, i + 1);

    if (node->occupancy > K - 1) {
        professionalTreeSplitBNode(tree, node);
    }
}

void professionalTreeSplitDataNode(ProfessionalBPTree* tree, ProfessionalDataNode* dnode) {
    ProfessionalDataNode* right;
    int mid;
    int i;
    int j=0;

    right = professionalTreeNewDataNode();
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

    professionalTreeInsertInBNode(tree, dnode->parent, right->keys[0], NULL, right);
}

void professionalTreeSplitBNode(ProfessionalBPTree* tree, ProfessionalBTreeNode* node) {
    ProfessionalBTreeNode* right;
    ProfessionalBTreeNode* newRoot;
    int mid;
    int upKey;
    int i;
    int j=0;

    right = professionalTreeNewBTreeNode(node->childIsData);
    mid = node->occupancy / 2;
    upKey = node->DArray[mid];

    i = mid + 1;
    while (i < node->occupancy) {
        right->DArray[j] = node->DArray[i];
        right->PArray[j] = node->PArray[i];
        right->DPArray[j] = node->DPArray[i];
        professionalTreeSetParent(right, j);
        j++;
        i++;
    }

    right->PArray[j] = node->PArray[node->occupancy];
    right->DPArray[j] = node->DPArray[node->occupancy];
    professionalTreeSetParent(right, j);

    right->occupancy = j;
    node->occupancy = mid;

    if (node->parent == NULL) {
        newRoot = professionalTreeNewBTreeNode(FALSE);
        newRoot->DArray[0] = upKey;
        newRoot->PArray[0] = node;
        newRoot->PArray[1] = right;
        newRoot->occupancy = 1;

        node->parent = newRoot;
        right->parent = newRoot;
        tree->root = newRoot;
    }
    else {
        professionalTreeInsertInBNode(tree, node->parent, upKey, right, NULL);
    }
}

status_code professionalTreeInsert(ProfessionalBPTree* tree, int key, Professional* record) {
    status_code ret_val=FAILURE;
    ProfessionalDataNode* dnode;

    if (professionalTreeSearch(tree, key) == NULL) {
        dnode = professionalTreeFindDataNode(tree, key);
        professionalTreeDataInsert(dnode, key, record);

        if (dnode->occupancy > C) {
            professionalTreeSplitDataNode(tree, dnode);
        }

        ret_val = SUCCESS;
    }

    return ret_val;
}

int professionalTreeCount(ProfessionalBPTree* tree) {
    int ret_val=0;
    ProfessionalDataNode* curr=tree->first;

    while (curr != NULL) {
        ret_val = ret_val + curr->occupancy;
        curr = curr->next;
    }

    return ret_val;
}

int professionalTreeFillRange(ProfessionalBPTree* tree, int low, int high, Professional* out[]) {
    int ret_val=0;
    ProfessionalDataNode* dnode;
    int i;
    boolean done=FALSE;

    dnode = professionalTreeFindDataNode(tree, low);

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

Professional** professionalTreeRangeToArray(ProfessionalBPTree* tree, int low, int high, int* total) {
    Professional** out;
    int size;

    size = professionalTreeCount(tree);
    if (size == 0) {
        size = 1;
    }

    out = (Professional**)malloc(size * sizeof(Professional*));

    if (out == NULL) {
        printf("ERROR!!! Memory allocation failed.\n");
        exit(1);
    }

    *total = professionalTreeFillRange(tree, low, high, out);

    return out;
}

Professional** professionalTreeToArray(ProfessionalBPTree* tree, int* total) {
    return professionalTreeRangeToArray(tree, INT_MIN, INT_MAX, total);
}

void professionalTreeFreeBNode(ProfessionalBTreeNode* node) {
    int i=0;

    if (node != NULL) {
        if (node->childIsData == FALSE) {
            while (i <= node->occupancy) {
                professionalTreeFreeBNode(node->PArray[i]);
                i++;
            }
        }
        free(node);
    }
}

void professionalTreeFree(ProfessionalBPTree* tree) {
    ProfessionalDataNode* curr=tree->first;
    ProfessionalDataNode* nptr;

    professionalTreeFreeBNode(tree->root);

    while (curr != NULL) {
        nptr = curr->next;
        free(curr);
        curr = nptr;
    }

    tree->root = NULL;
    tree->first = NULL;
}
