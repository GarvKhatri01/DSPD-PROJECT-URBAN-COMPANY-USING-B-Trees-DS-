#include "service.h"


status_code serviceIsValid(int id, char name[], float base_price) {
    status_code ret_val=FAILURE;

    if (id > 0 && nameIsValid(name, MAX_SERVICE_NAME_LEN) == SUCCESS && base_price > 0.0) {
        ret_val = SUCCESS;
    }

    return ret_val;
}


Service* serviceFind(ServiceBPTree* tree, int id) {
    return serviceTreeSearch(tree, id);
}


boolean serviceExists(ServiceBPTree* tree, int id) {
    boolean ret_val=FALSE;

    if (serviceFind(tree, id) != NULL) {
        ret_val = TRUE;
    }

    return ret_val;
}



status_code serviceAdd(ServiceBPTree* tree, int id, char name[], float base_price) {
    status_code ret_val=FAILURE;
    Service* newNode=NULL;

    if (serviceIsValid(id, name, base_price) == FAILURE) {
        printf("ERROR!! Invalid input\n");
        printf("Required Input:\n");

        printf("ID must be positive\n");
        printf("Name Non Empty, less than %d characters, no '|'\n", MAX_SERVICE_NAME_LEN);
        printf("Base price must be greater than 0\n");
    }
    else if (serviceExists(tree, id) == TRUE) {
        printf("ERROR!!! Service with ID %d already exists.\n", id);
    }
    else {
        newNode = (Service*)malloc(sizeof(Service));

        if (newNode == NULL) {
            printf("ERROR!!! Memory allocation failed for service ID %d.\n", id);
        }
        else {
            newNode->service_id = id;
            strcpy(newNode->name, name);
            newNode->base_price = base_price;

            serviceTreeInsert(tree, id, newNode);

            printf("Service '%s' (ID: %d, Price: %.2f) added successfully.\n", name, id, base_price);
            ret_val = SUCCESS;
        }
    }

    return ret_val;
}



void serviceDisplayOne(Service* sptr) {
    if (sptr == NULL) {
        printf("Service record not found\n");
    }
    else {
        printf("ID: %d\nName: %s\nBase Price: Rs. %.2f\n", sptr->service_id, sptr->name, sptr->base_price);
    }
}


void serviceDisplayAll(ServiceBPTree* tree) {
    Service** all;
    int total;
    int i;

    all = serviceTreeToArray(tree, &total);

    if (total == 0) {
        printf("No services are currently available.\n");
    }
    else {
        printf("_*_*_*_*_*_*_*_*_ SERVICES _*_*_*_*_*_*_*_*_*_\n");
        i = 0;
        while (i < total) {
            serviceDisplayOne(all[i]);
            i++;
        }
        printf("_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_\n");
    }

    free(all);
}


void serviceFreeAll(ServiceBPTree* tree) {
    Service** all;
    int total;
    int i=0;

    all = serviceTreeToArray(tree, &total);

    while (i < total) {
        free(all[i]);
        i++;
    }

    free(all);
    serviceTreeFree(tree);
}
