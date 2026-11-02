#ifndef CUSTOMER_H
#define CUSTOMER_H

#include "common.h"
#include "customer_tree.h"

typedef struct customer_tag {
    int customer_id;
    char name[MAX_NAME_LEN];
    int pincode;
    char contact[MAX_CONTACT_LEN];
} Customer;


status_code customerIsValid(int id, char name[], int pincode, char contact[]);

status_code customerRegister(CustomerBPTree* tree, int id, char name[], int pincode, char contact[]);

Customer* customerFind(CustomerBPTree* tree, int id);

boolean customerExists(CustomerBPTree* tree, int id);

void customerDisplayOne(Customer* cptr);

void customerDisplayAll(CustomerBPTree* tree);

void customerFreeAll(CustomerBPTree* tree);

#endif
