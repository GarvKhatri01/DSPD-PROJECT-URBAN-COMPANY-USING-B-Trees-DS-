#ifndef SERVICE_H
#define SERVICE_H

#include "common.h"
#include "service_tree.h"

typedef struct service_tag {
    int service_id;
    char name[MAX_SERVICE_NAME_LEN];
    float base_price;
} Service;


status_code serviceIsValid(int id, char name[], float base_price);

status_code serviceAdd(ServiceBPTree* tree, int id, char name[], float base_price);

Service* serviceFind(ServiceBPTree* tree, int id);

boolean serviceExists(ServiceBPTree* tree, int id);

void serviceDisplayOne(Service* sptr);

void serviceDisplayAll(ServiceBPTree* tree);

void serviceFreeAll(ServiceBPTree* tree);

#endif
