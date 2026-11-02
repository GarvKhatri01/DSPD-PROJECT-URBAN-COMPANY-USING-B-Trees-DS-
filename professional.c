#include "professional.h"


status_code professionalIsValid(int id, char name[], int pincode, char contact[]) {
    status_code ret_val=FAILURE;

    if (id > 0 && nameIsValid(name, MAX_NAME_LEN) == SUCCESS && countDigits(pincode) == PINCODE_DIGITS && contactIsValid(contact) == SUCCESS) {
        ret_val = SUCCESS;
    }

    return ret_val;
}


Professional* professionalFind(ProfessionalBPTree* tree, int id) {
    return professionalTreeSearch(tree, id);
}


boolean professionalExists(ProfessionalBPTree* tree, int id) {
    boolean ret_val=FALSE;

    if (professionalFind(tree, id) != NULL) {
        ret_val = TRUE;
    }

    return ret_val;
}


boolean professionalOffers(Professional* ptr, int svc_id) {
    boolean ret_val=FALSE;
    int i=0;

    if (ptr != NULL) {
        while (i < ptr->service_count && ret_val == FALSE) {
            if (ptr->service_ids[i] == svc_id) {
                ret_val = TRUE;
            }
            i++;
        }
    }

    return ret_val;
}


boolean professionalIsAvailable(Professional* ptr) {
    boolean ret_val=FALSE;

    if (ptr != NULL && ptr->availability == AVAILABLE) {
        ret_val = TRUE;
    }

    return ret_val;
}


void professionalSetAvailability(Professional* ptr, AvailabilityStatus status) {
    if (ptr != NULL) {
        ptr->availability = status;
    }
}


status_code validateServiceList(ServiceBPTree* service_tree, int svc_ids[], int svc_count) {
    status_code ret_val=FAILURE;
    int i;
    int j;
    boolean flag=TRUE;
    boolean hasDuplicates=FALSE;

    if (svc_count <= 0 || svc_count > MAX_SERVICES_PER_PROFESSIONAL) {
        printf("ERROR!!! Service count must be between 1 and %d\n", MAX_SERVICES_PER_PROFESSIONAL);
    }
    else {
        i = 0;

        while (i < svc_count && flag == TRUE) {
            if (serviceExists(service_tree, svc_ids[i]) == FALSE) {
                flag = FALSE;
                printf("ERROR!!! Service ID %d does not exist.\n", svc_ids[i]);
            }
            i++;
        }

        if (flag == TRUE) {
            i = 0;

            while (i < svc_count - 1 && hasDuplicates == FALSE) {
                j = i + 1;

                while (j < svc_count && hasDuplicates == FALSE) {
                    if (svc_ids[j] == svc_ids[i]) {
                        hasDuplicates = TRUE;
                        printf("ERROR!!! Service ID %d appears more than once in the list.\n", svc_ids[i]);
                    }
                    j++;
                }
                i++;
            }

            if (hasDuplicates == FALSE) {
                ret_val = SUCCESS;
            }
        }
    }

    return ret_val;
}


status_code professionalRegister(ProfessionalBPTree* tree, ServiceBPTree* service_tree, int id, char name[],
                                 int pincode, char contact[], int svc_ids[], int svc_count) {
    status_code ret_val=FAILURE;
    Professional* newNode=NULL;
    int i;

    if (professionalIsValid(id, name, pincode, contact) == FAILURE) {
        printf("ERROR!! Invalid input\n");
        printf("Required Input:\n");

        printf("ID must be positive\n");
        printf("Name Non Empty, less than %d characters, no '|'\n", MAX_NAME_LEN);
        printf("Pincode must be %d digits\n",PINCODE_DIGITS);
        printf("Contact must be 10 digits\n");
    }
    else if (professionalExists(tree, id) == TRUE) {
        printf("ERROR!! Professional with ID %d already exists.\n", id);
    }
    else if (validateServiceList(service_tree, svc_ids, svc_count) == SUCCESS) {
        newNode = (Professional*)malloc(sizeof(Professional));

        if (newNode == NULL) {
            printf("ERROR!!! Memory allocation failed for professional\n");
        }
        else {
            newNode->professional_id = id;
            strcpy(newNode->name, name);
            newNode->pincode = pincode;
            strcpy(newNode->contact, contact);
            newNode->availability = AVAILABLE;
            newNode->service_count = svc_count;

            i = 0;
            while (i < svc_count) {
                newNode->service_ids[i] = svc_ids[i];
                i++;
            }

            professionalTreeInsert(tree, id, newNode);

            printf("Professional '%s' (ID: %d) registered successfully.\n", name, id);
            ret_val = SUCCESS;
        }
    }

    return ret_val;
}


void professionalDisplayServices(Professional* ptr, ServiceBPTree* service_tree) {
    int i=0;
    Service* found_service;

    if (ptr == NULL) {
        printf("Professional record not found\n");
    }
    else {
        while (i < ptr->service_count) {
            found_service = serviceFind(service_tree, ptr->service_ids[i]);

            if (found_service == NULL) {
                printf("Service ID %d not found in database\n", ptr->service_ids[i]);
            }
            else {
                serviceDisplayOne(found_service);
            }
            i++;
        }
    }
}


void professionalDisplayOne(Professional* ptr, ServiceBPTree* service_tree) {
    if (ptr == NULL) {
        printf("Professional record not found\n");
    }
    else {
        printf("ID: %d\nName: %s\nPincode: %d\nContact: %s\n",
               ptr->professional_id, ptr->name, ptr->pincode, ptr->contact);

        if (ptr->availability == AVAILABLE) {
            printf("Status: AVAILABLE\n");
        }
        else {
            printf("Status: BUSY\n");
        }

        printf("Services offered:\n");
        professionalDisplayServices(ptr, service_tree);
        printf("\n");
    }
}

void professionalDisplayAll(ProfessionalBPTree* tree, ServiceBPTree* service_tree) {
    Professional** all;
    int total;
    int i;

    all = professionalTreeToArray(tree, &total);

    if (total == 0) {
        printf("No professionals are currently registered.\n");
    }
    else {
        printf("_*_*_*_*_*_*_*_ Professional List _*_*_*_*_*_*_*_\n");
        i = 0;
        while (i < total) {
            professionalDisplayOne(all[i], service_tree);
            i++;
        }
        printf("_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_\n");
    }

    free(all);
}

void professionalFreeAll(ProfessionalBPTree* tree) {
    Professional** all;
    int total;
    int i=0;

    all = professionalTreeToArray(tree, &total);

    while (i < total) {
        free(all[i]);
        i++;
    }

    free(all);
    professionalTreeFree(tree);
}
