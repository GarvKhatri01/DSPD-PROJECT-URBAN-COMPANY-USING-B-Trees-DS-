#include "customer.h"

status_code customerIsValid(int id, char name[], int pincode, char contact[]) {
    status_code ret_val=FAILURE;

    if (id > 0 && nameIsValid(name, MAX_NAME_LEN) == SUCCESS && countDigits(pincode) == PINCODE_DIGITS && contactIsValid(contact) == SUCCESS) {
        ret_val = SUCCESS;
    }

    return ret_val;
}


Customer* customerFind(CustomerBPTree* tree, int id) {
    return customerTreeSearch(tree, id);
}


boolean customerExists(CustomerBPTree* tree, int id) {
    boolean ret_val=FALSE;

    if (customerFind(tree, id) != NULL) {
        ret_val = TRUE;
    }

    return ret_val;
}


status_code customerRegister(CustomerBPTree* tree, int id, char name[], int pincode, char contact[]) {
    status_code ret_val=FAILURE;
    Customer* newNode=NULL;

    if (customerIsValid(id, name, pincode, contact) == FAILURE) {
        printf("ERROR!! Invalid input\n");
        printf("Required Input:\n");

        printf("ID must be positive\n");
        printf("Name Non Empty, less than %d characters, no '|'\n", MAX_NAME_LEN);
        printf("Pincode must be %d digits\n",PINCODE_DIGITS);
        printf("Contact must be 10 digits\n");
    }
    else if (customerExists(tree, id) == TRUE) {
        printf("ERROR!! Customer with ID %d already exists.\n", id);
    }
    else {
        newNode = (Customer*)malloc(sizeof(Customer));

        if (newNode == NULL) {
            printf("ERROR!!! Memory allocation failed for customer ID %d.\n", id);
        }
        else {
            newNode->customer_id = id;
            strcpy(newNode->name, name);
            newNode->pincode = pincode;
            strcpy(newNode->contact, contact);

            customerTreeInsert(tree, id, newNode);

            printf("SUCCESS!! Customer '%s' (ID: %d) registered successfully.\n", name, id);
            ret_val = SUCCESS;
        }
    }

    return ret_val;
}


void customerDisplayOne(Customer* cptr) {
    if (cptr == NULL) {
        printf("SORRY!! Customer record not found\n");
    }
    else {
        printf("ID: %d\nName: %s\nPincode: %d\nContact: %s\n", cptr->customer_id, cptr->name, cptr->pincode, cptr->contact);
    }
}


void customerDisplayAll(CustomerBPTree* tree) {
    Customer** all;
    int total;
    int i;

    all = customerTreeToArray(tree, &total);

    if (total == 0) {
        printf("No customers are currently registered.\n");
    }
    else {
        printf("_*_*_*_*_*_*_*_  Customer List  _*_*_*_*_*_*_*_\n");
        i = 0;
        while (i < total) {
            customerDisplayOne(all[i]);
            i++;
        }
        printf("*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_\n");
    }

    free(all);
}


void customerFreeAll(CustomerBPTree* tree) {
    Customer** all;
    int total;
    int i=0;

    all = customerTreeToArray(tree, &total);

    while (i < total) {
        free(all[i]);
        i++;
    }

    free(all);
    customerTreeFree(tree);
}
