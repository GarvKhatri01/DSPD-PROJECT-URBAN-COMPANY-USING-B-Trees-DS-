#ifndef PROFESSIONAL_TREE_H
#define PROFESSIONAL_TREE_H

#include "common.h"

struct professional_tag;

typedef struct professional_dnode_tag {
    int keys[C + 1];
    struct professional_tag* records[C + 1];
    int occupancy;
    struct professional_dnode_tag* prev;
    struct professional_dnode_tag* next;
    struct professional_bnode_tag* parent;
} ProfessionalDataNode;

typedef struct professional_bnode_tag {
    int DArray[K];
    struct professional_bnode_tag* PArray[K + 1];
    ProfessionalDataNode* DPArray[K + 1];
    boolean childIsData;
    int occupancy;
    struct professional_bnode_tag* parent;
} ProfessionalBTreeNode;

typedef struct {
    ProfessionalBTreeNode* root;
    ProfessionalDataNode* first;
} ProfessionalBPTree;

void professionalTreeInit(ProfessionalBPTree* tree);

status_code professionalTreeInsert(ProfessionalBPTree* tree, int key, struct professional_tag* record);

struct professional_tag* professionalTreeSearch(ProfessionalBPTree* tree, int key);

int professionalTreeCount(ProfessionalBPTree* tree);

struct professional_tag** professionalTreeToArray(ProfessionalBPTree* tree, int* total);

struct professional_tag** professionalTreeRangeToArray(ProfessionalBPTree* tree, int low, int high, int* total);

void professionalTreeFree(ProfessionalBPTree* tree);

#endif
