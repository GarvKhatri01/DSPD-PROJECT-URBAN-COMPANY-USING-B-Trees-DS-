#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "customer.h"
#include "professional.h"
#include "service.h"
#include "request.h"
#include "assign.h"
#include "report.h"

#define BUF 100
#define DB_FILE "database.txt"

//file handling
void loadData(ServiceBPTree* sTree, CustomerBPTree* cTree, ProfessionalBPTree* pTree, RequestBPTree* rTree) {
    FILE* fp;
    char line[300];
    char name[BUF];
    char contact[BUF];
    char idList[BUF];
    char statusText[BUF];
    char* token;
    int svcIds[MAX_SERVICES_PER_PROFESSIONAL + 1];
    int svcCount;
    int id, pincode, custId, svcId, proId;
    float price;
    DateTime dt;
    Request* req;
    Professional* pro;
    Professional* freed;
    status_code result;

    fp = fopen(DB_FILE, "r");

    if (fp == NULL) {
        printf("'%s' not found, starting with empty data.\n", DB_FILE);
    }
    else {
        printf("Loading data from %s ...\n", DB_FILE);

        while (fgets(line, sizeof(line), fp) != NULL) {

            if (strncmp(line, "SERVICE|", 8) == 0) {
                if (sscanf(line, "SERVICE|%d|%99[^|]|%f", &id, name, &price) == 3) {
                    serviceAdd(sTree, id, name, price);
                }
            }

            else if (strncmp(line, "CUSTOMER|", 9) == 0) {
                if (sscanf(line, "CUSTOMER|%d|%99[^|]|%d|%99[^|\r\n]", &id, name, &pincode, contact) == 4) {
                    customerRegister(cTree, id, name, pincode, contact);
                }
            }

            else if (strncmp(line, "PROFESSIONAL|", 13) == 0) {
                if (sscanf(line, "PROFESSIONAL|%d|%99[^|]|%d|%99[^|]|%99[^\r\n]", &id, name, &pincode, contact, idList) == 5) {

                    svcCount = 0;
                    token = strtok(idList, ",");
                    while (token != NULL && svcCount < MAX_SERVICES_PER_PROFESSIONAL + 1) {
                        svcIds[svcCount] = atoi(token);
                        svcCount++;
                        token = strtok(NULL, ",");
                    }

                    professionalRegister(pTree, sTree, id, name, pincode, contact, svcIds, svcCount);
                }
            }

            else if (strncmp(line, "REQUEST|", 8) == 0) {
                if (sscanf(line, "REQUEST|%d|%d|%d|%d-%d-%d|%d:%d|%99[^|]|%d",
                           &id, &custId, &svcId,
                           &dt.date.day, &dt.date.month, &dt.date.year,
                           &dt.time.hour, &dt.time.minute,
                           statusText, &proId) == 10) {

                    result = requestCreate(rTree, cTree, sTree, id, custId, svcId, dt);

                    if (result == SUCCESS) {
                        if (strcmp(statusText, "ASSIGNED") == 0 || strcmp(statusText, "COMPLETED") == 0) {
                            req = requestFind(rTree, id);
                            pro = professionalFind(pTree, proId);
                            result = requestAssign(req, pro);

                            if (result == SUCCESS && strcmp(statusText, "COMPLETED") == 0) {
                                freed = NULL;
                                requestComplete(rTree, id, &freed);
                            }
                        }
                        else if (strcmp(statusText, "CANCELLED") == 0) {
                            freed = NULL;
                            requestCancel(rTree, id, &freed);
                        }
                    }
                }
            }
        }
        fclose(fp);
        printf("Data loaded.\n");
    }
}

char* statusToFileText(RequestStatus status) {
    char* ret_val="PENDING";

    if (status == ASSIGNED) {
        ret_val = "ASSIGNED";
    }
    else if (status == COMPLETED) {
        ret_val = "COMPLETED";
    }
    else if (status == CANCELLED) {
        ret_val = "CANCELLED";
    }

    return ret_val;
}

void saveData(ServiceBPTree* sTree, CustomerBPTree* cTree, ProfessionalBPTree* pTree, RequestBPTree* rTree) {
    FILE* fp;
    Service** sAll;
    Customer** cAll;
    Professional** pAll;
    Request** rAll;
    int total;
    int i;
    int j;
    int proId;
    Service* s;
    Customer* c;
    Professional* p;
    Request* r;

    fp = fopen(DB_FILE, "w");

    if (fp == NULL) {
        printf("ERROR!! Could not open '%s' for writing.\n", DB_FILE);
    }
    else {
        sAll = serviceTreeToArray(sTree, &total);
        i = 0;
        while (i < total) {
            s = sAll[i];
            fprintf(fp, "SERVICE|%d|%s|%.2f\n", s->service_id, s->name, s->base_price);
            i++;
        }
        free(sAll);

        cAll = customerTreeToArray(cTree, &total);
        i = 0;
        while (i < total) {
            c = cAll[i];
            fprintf(fp, "CUSTOMER|%d|%s|%d|%s\n", c->customer_id, c->name, c->pincode, c->contact);
            i++;
        }
        free(cAll);

        pAll = professionalTreeToArray(pTree, &total);
        i = 0;
        while (i < total) {
            p = pAll[i];
            fprintf(fp, "PROFESSIONAL|%d|%s|%d|%s|", p->professional_id, p->name, p->pincode, p->contact);

            j = 0;
            while (j < p->service_count) {
                fprintf(fp, "%d", p->service_ids[j]);
                if (j < p->service_count - 1) {
                    fprintf(fp, ",");
                }
                j++;
            }
            fprintf(fp, "\n");
            i++;
        }
        free(pAll);

        rAll = requestTreeToArray(rTree, &total);
        i = 0;
        while (i < total) {
            r = rAll[i];
            proId = 0;
            if (r->reqProfessional != NULL) {
                proId = r->reqProfessional->professional_id;
            }

            fprintf(fp, "REQUEST|%d|%d|%d|%02d-%02d-%04d|%02d:%02d|%s|%d\n",
                    r->reqID, r->reqCustomer->customer_id, r->reqService->service_id,
                    r->reqDateTime.date.day, r->reqDateTime.date.month, r->reqDateTime.date.year,
                    r->reqDateTime.time.hour, r->reqDateTime.time.minute,
                    statusToFileText(r->reqStatus), proId);
            i++;
        }
        free(rAll);

        fclose(fp);
        printf("Data saved to %s\n", DB_FILE);
    }
}

void printMenu(void) {
    printf("\n========================================\n");
    printf("   Urban Company Service Management\n");
    printf("========================================\n");
    printf(" 1.  Register Customer\n");
    printf(" 2.  Register Professional\n");
    printf(" 3.  Add Service\n");
    printf(" 4.  Create Service Request\n");
    printf(" 5.  Complete Service Request\n");
    printf(" 6.  Cancel Service Request\n");
    printf(" 7.  Search: available professionals for a service\n");
    printf(" 8.  Search: services of a professional\n");
    printf(" 9.  Search: requests of a customer\n");
    printf(" 10. Range Search: requests of customers between C1 and C2\n");
    printf(" 11. Sort:   service demand\n");
    printf(" 12. Sort:   professional activity\n");
    printf(" 13. Display pending requests\n");
    printf(" 14. Display service history of a professional\n");
    printf(" 15. Display all records\n");
    printf(" 16. Save data to file\n");
    printf(" 0.  Exit\n");
    printf("----------------------------------------\n");
    printf("Enter choice: ");
}



int main() {
    CustomerBPTree customerTree;
    ProfessionalBPTree professionalTree;
    ServiceBPTree serviceTree;
    RequestBPTree requestTree;

    int choice;
    int id, pincode, count, i;
    int custId, custId2, svcId, reqId, proId;
    int svcIds[MAX_SERVICES_PER_PROFESSIONAL];
    float price;
    char name[BUF];
    char contact[BUF];
    DateTime dt;
    boolean dateOk;
    status_code result;
    Request* req;
    Professional* freedPro;

    customerTreeInit(&customerTree);
    professionalTreeInit(&professionalTree);
    serviceTreeInit(&serviceTree);
    requestTreeInit(&requestTree);

    printf("Welcome to Urban Company Service Management System\n\n");
    
    loadData(&serviceTree, &customerTree, &professionalTree, &requestTree);

    choice = -1;

    while (choice != 0) {
        printMenu();
        if (scanf("%d", &choice) != 1) {
            if (feof(stdin)) {
                printf("\nInput ended. Exiting...\n");
                break;
            }
            printf("Invalid input. Please enter a number.\n");
            scanf("%*[^\n]");
            scanf("%*c");
            continue;
        }

        switch (choice) {

        case 1:
            printf("\n--- Register Customer ---\n");
            printf("Enter Customer ID: ");
            if (scanf("%d", &id) != 1) {
                printf("Invalid input.\n");
                scanf("%*[^\n]");
                scanf("%*c");
                break;
            }
            printf("Enter Name: ");
            if (scanf(" %99[^\n]", name) != 1) {
                printf("Invalid input.\n");
                break;
            }
            printf("Enter Pincode (6 digits): ");
            if (scanf("%d", &pincode) != 1) {
                printf("Invalid input.\n");
                scanf("%*[^\n]");
                scanf("%*c");
                break;
            }
            printf("Enter Contact (10 digits): ");
            if (scanf(" %99[^\n]", contact) != 1) {
                printf("Invalid input.\n");
                break;
            }

            customerRegister(&customerTree, id, name, pincode, contact);
            break;

        case 2:
            printf("\n--- Register Professional ---\n");
            printf("Enter Professional ID: ");
            if (scanf("%d", &id) != 1) {
                printf("Invalid input.\n");
                scanf("%*[^\n]");
                scanf("%*c");
                break;
            }
            printf("Enter Name: ");
            if (scanf(" %99[^\n]", name) != 1) {
                printf("Invalid input.\n");
                break;
            }
            printf("Enter Pincode (6 digits): ");
            if (scanf("%d", &pincode) != 1) {
                printf("Invalid input.\n");
                scanf("%*[^\n]");
                scanf("%*c");
                break;
            }
            printf("Enter Contact (10 digits): ");
            if (scanf(" %99[^\n]", contact) != 1) {
                printf("Invalid input.\n");
                break;
            }
            printf("How many services does he/she offer (1 to %d): ", MAX_SERVICES_PER_PROFESSIONAL);
            if (scanf("%d", &count) != 1) {
                printf("Invalid input.\n");
                scanf("%*[^\n]");
                scanf("%*c");
                break;
            }

            if (count < 1 || count > MAX_SERVICES_PER_PROFESSIONAL) {
                printf("ERROR!! A professional can offer 1 to %d services only.\n", MAX_SERVICES_PER_PROFESSIONAL);
            }
            else {
                for (i = 0; i < count; i++) {
                    printf("Enter Service ID %d: ", i + 1);
                    if (scanf("%d", &svcIds[i]) != 1) {
                        printf("Invalid input.\n");
                        scanf("%*[^\n]");
                        scanf("%*c");
                        break;
                    }
                }
                if (i < count) {
                    break;
                }
                professionalRegister(&professionalTree, &serviceTree, id, name, pincode, contact, svcIds, count);
                assignFreedProfessional(&requestTree, professionalFind(&professionalTree, id));
            }
            break;

        case 3:
            printf("\n--- Add Service ---\n");
            printf("Enter Service ID: ");
            if (scanf("%d", &id) != 1) {
                printf("Invalid input.\n");
                scanf("%*[^\n]");
                scanf("%*c");
                break;
            }
            printf("Enter Service Name: ");
            if (scanf(" %99[^\n]", name) != 1) {
                printf("Invalid input.\n");
                break;
            }
            printf("Enter Base Price: ");
            if (scanf("%f", &price) != 1) {
                printf("Invalid input.\n");
                scanf("%*[^\n]");
                scanf("%*c");
                break;
            }

            serviceAdd(&serviceTree, id, name, price);
            break;

        case 4:
            printf("\n--- Create Service Request ---\n");
            printf("Enter Request ID: ");
            if (scanf("%d", &reqId) != 1) {
                printf("Invalid input.\n");
                scanf("%*[^\n]");
                scanf("%*c");
                break;
            }
            printf("Enter Customer ID: ");
            if (scanf("%d", &custId) != 1) {
                printf("Invalid input.\n");
                scanf("%*[^\n]");
                scanf("%*c");
                break;
            }
            printf("Enter Service ID: ");
            if (scanf("%d", &svcId) != 1) {
                printf("Invalid input.\n");
                scanf("%*[^\n]");
                scanf("%*c");
                break;
            }

            dateOk = TRUE;
            printf("Enter Date (DD MM YYYY): ");
            if (scanf("%d %d %d", &dt.date.day, &dt.date.month, &dt.date.year) != 3) {
                dateOk = FALSE;
                scanf("%*[^\n]");
                scanf("%*c");
            }
            printf("Enter Time (HH MM): ");
            if (scanf("%d %d", &dt.time.hour, &dt.time.minute) != 2) {
                dateOk = FALSE;
                scanf("%*[^\n]");
                scanf("%*c");
            }

            if (dateOk == FALSE) {
                printf("ERROR!! Date and time must be numbers, e.g. 15 10 2026 and 14 30\n");
            }
            else {
                result = requestCreate(&requestTree, &customerTree, &serviceTree, reqId, custId, svcId, dt);

                if (result == SUCCESS) {
                    req = requestFind(&requestTree, reqId);
                    assignNewRequest(&professionalTree, req);
                }
            }
            break;

        case 5:
            printf("\n--- Complete Service Request ---\n");
            printf("Enter Request ID to complete: ");
            if (scanf("%d", &reqId) != 1) {
                printf("Invalid input.\n");
                scanf("%*[^\n]");
                scanf("%*c");
                break;
            }

            freedPro = NULL;
            result = requestComplete(&requestTree, reqId, &freedPro);

            if (result == SUCCESS) {
                assignFreedProfessional(&requestTree, freedPro);
            }
            break;

        case 6:
            printf("\n--- Cancel Service Request ---\n");
            printf("Enter Request ID to cancel: ");
            if (scanf("%d", &reqId) != 1) {
                printf("Invalid input.\n");
                scanf("%*[^\n]");
                scanf("%*c");
                break;
            }

            freedPro = NULL;
            requestCancel(&requestTree, reqId, &freedPro);

            if (freedPro != NULL) {
                assignFreedProfessional(&requestTree, freedPro);
            }
            break;

        case 7:
            printf("\nEnter Service ID: ");
            if (scanf("%d", &svcId) != 1) {
                printf("Invalid input.\n");
                scanf("%*[^\n]");
                scanf("%*c");
                break;
            }
            reportAvailableProfessionals(&professionalTree, &serviceTree, svcId);
            break;

        case 8:
            printf("\nEnter Professional ID: ");
            if (scanf("%d", &proId) != 1) {
                printf("Invalid input.\n");
                scanf("%*[^\n]");
                scanf("%*c");
                break;
            }
            reportServices(&professionalTree, &serviceTree, proId);
            break;

        case 9:
            printf("\nEnter Customer ID: ");
            if (scanf("%d", &custId) != 1) {
                printf("Invalid input.\n");
                scanf("%*[^\n]");
                scanf("%*c");
                break;
            }
            reportCustomerRequests(&requestTree, &customerTree, custId);
            break;

        case 10:
            printf("\nEnter Customer ID C1: ");
            if (scanf("%d", &custId) != 1) {
                printf("Invalid input.\n");
                scanf("%*[^\n]");
                scanf("%*c");
                break;
            }
            printf("Enter Customer ID C2: ");
            if (scanf("%d", &custId2) != 1) {
                printf("Invalid input.\n");
                scanf("%*[^\n]");
                scanf("%*c");
                break;
            }
            reportCustomerRangeRequests(&requestTree, &customerTree, custId, custId2);
            break;

        case 11:
            reportServiceDemand(&requestTree, &serviceTree);
            break;

        case 12:
            reportProfessionalActivity(&requestTree, &professionalTree);
            break;

        case 13:
            reportPendingRequests(&requestTree);
            break;

        case 14:
            printf("\nEnter Professional ID: ");
            if (scanf("%d", &proId) != 1) {
                printf("Invalid input.\n");
                scanf("%*[^\n]");
                scanf("%*c");
                break;
            }
            reportServiceHistory(&requestTree, &professionalTree, proId);
            break;

        case 15:
            customerDisplayAll(&customerTree);
            serviceDisplayAll(&serviceTree);
            professionalDisplayAll(&professionalTree, &serviceTree);
            requestDisplayAll(&requestTree);
            break;

        case 16:
            saveData(&serviceTree, &customerTree, &professionalTree, &requestTree);
            break;

        case 0:
            printf("Save data to %s before exit? (1 = Yes, 0 = No): ", DB_FILE);
            if (scanf("%d", &id) != 1) {
                printf("Invalid input. Exiting without saving.\n");
                break;
            }
            if (id == 1) {
                saveData(&serviceTree, &customerTree, &professionalTree, &requestTree);
            }
            printf("Exiting... Goodbye!\n");
            break;

        default:
            printf("Invalid choice. Please try again.\n");
            break;
        }
    }

    customerFreeAll(&customerTree);
    professionalFreeAll(&professionalTree);
    serviceFreeAll(&serviceTree);
    requestFreeAll(&requestTree);

    return 0;
}
