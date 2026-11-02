#include "report.h"

typedef struct {
    int count;
    Service* service;
} ServiceCount;


typedef struct {
    int count;
    Professional* professional;
} ProfessionalCount;


int countRequestsOfService(RequestBPTree* tree, Service* servPtr) {
    int count=0;
    int i;
    RequestDataNode* dnode=tree->first;

    while (dnode != NULL) {
        i = 0;
        while (i < dnode->occupancy) {
            if (dnode->records[i]->reqService == servPtr) {
                count = count + 1;
            }
            i++;
        }
        dnode = dnode->next;
    }

    return count;
}


int countCompletedByProfessional(RequestBPTree* tree, Professional* proPtr) {
    int count=0;
    int i;
    Request* req;
    RequestDataNode* dnode=tree->first;

    while (dnode != NULL) {
        i = 0;
        while (i < dnode->occupancy) {
            req = dnode->records[i];

            if (req->reqStatus == COMPLETED && req->reqProfessional == proPtr) {
                count = count + 1;
            }
            i++;
        }
        dnode = dnode->next;
    }

    return count;
}


boolean serviceCountIsBefore(ServiceCount a, ServiceCount b) {
    boolean ret_val=FALSE;

    if (a.count > b.count) {
        ret_val = TRUE;
    }
    else if (a.count == b.count && a.service->service_id < b.service->service_id) {
        ret_val = TRUE;
    }

    return ret_val;
}


void serviceCountSiftDown(ServiceCount arr[], int root, int n) {
    int child;
    ServiceCount temp;
    boolean done=FALSE;

    while (2 * root + 1 < n && done == FALSE) {
        child = 2 * root + 1;

        if (child + 1 < n && serviceCountIsBefore(arr[child], arr[child+1]) == TRUE) {
            child = child + 1;
        }

        if (serviceCountIsBefore(arr[root], arr[child]) == TRUE) {
            temp = arr[root];
            arr[root] = arr[child];
            arr[child] = temp;
            root = child;
        }
        else {
            done = TRUE;
        }
    }
}


void sortServiceCounts(ServiceCount arr[], int n) {
    int i;
    ServiceCount temp;

    i = n / 2 - 1;
    while (i >= 0) {
        serviceCountSiftDown(arr, i, n);
        i--;
    }

    i = n - 1;
    while (i > 0) {
        temp = arr[0];
        arr[0] = arr[i];
        arr[i] = temp;

        serviceCountSiftDown(arr, 0, i);
        i--;
    }
}



boolean professionalCountIsBefore(ProfessionalCount a, ProfessionalCount b) {
    boolean ret_val=FALSE;

    if (a.count > b.count) {
        ret_val = TRUE;
    }
    else if (a.count == b.count && a.professional->professional_id < b.professional->professional_id) {
        ret_val = TRUE;
    }

    return ret_val;
}


void professionalCountSiftDown(ProfessionalCount arr[], int root, int n) {
    int child;
    ProfessionalCount temp;
    boolean done=FALSE;

    while (2 * root + 1 < n && done == FALSE) {
        child = 2 * root + 1;

        if (child + 1 < n && professionalCountIsBefore(arr[child], arr[child+1]) == TRUE) {
            child = child + 1;
        }

        if (professionalCountIsBefore(arr[root], arr[child]) == TRUE) {
            temp = arr[root];
            arr[root] = arr[child];
            arr[child] = temp;
            root = child;
        }
        else {
            done = TRUE;
        }
    }
}


void sortProfessionalCounts(ProfessionalCount arr[], int n) {
    int i;
    ProfessionalCount temp;

    i = n / 2 - 1;
    while (i >= 0) {
        professionalCountSiftDown(arr, i, n);
        i--;
    }

    i = n - 1;
    while (i > 0) {
        temp = arr[0];
        arr[0] = arr[i];
        arr[i] = temp;

        professionalCountSiftDown(arr, 0, i);
        i--;
    }
}



void printRequestArray(Request* arr[], int n) {
    int i=0;

    while (i < n) {
        requestDisplayOne(arr[i]);
        i++;
    }
}



status_code reportAvailableProfessionals(ProfessionalBPTree* pro_tree, ServiceBPTree* service_tree, int svc_id) {
    status_code ret_val=FAILURE;
    Service* found_svc;
    Professional* curr;
    ProfessionalDataNode* dnode;
    int i;
    int match_count=0;

    found_svc = serviceFind(service_tree, svc_id);

    if (found_svc == NULL) {
        printf("ERROR!! Service with ID %d does not exist.\n", svc_id);
    }
    else {
        printf("\n_*_*_*_Available Professionals for Service %d - %s _*_*_*_\n", svc_id, found_svc->name);

        dnode = pro_tree->first;
        while (dnode != NULL) {
            i = 0;
            while (i < dnode->occupancy) {
                curr = dnode->records[i];

                if (professionalIsAvailable(curr) == TRUE && professionalOffers(curr, svc_id) == TRUE) {
                    professionalDisplayOne(curr, service_tree);
                    match_count = match_count + 1;
                }
                i++;
            }
            dnode = dnode->next;
        }

        if (match_count == 0) {
            printf("No available professionals found for this service.\n");
        }

        ret_val = SUCCESS;
    }

    return ret_val;
}


status_code reportServices(ProfessionalBPTree* pro_tree, ServiceBPTree* service_tree, int pro_id) {
    status_code ret_val=FAILURE;
    Professional* found_pro;

    found_pro = professionalFind(pro_tree, pro_id);

    if (found_pro == NULL) {
        printf("ERROR!!! Professional with ID %d does not exist.\n", pro_id);
    }
    else {
        printf("\nServices provided by Professional %d - %s\n", pro_id, found_pro->name);
        professionalDisplayServices(found_pro, service_tree);
        ret_val = SUCCESS;
    }

    return ret_val;
}


status_code reportCustomerRequests(RequestBPTree* req_tree, CustomerBPTree* customer_tree, int cust_id) {
    status_code ret_val=FAILURE;
    Customer* found_cust;
    Request* curr;
    RequestDataNode* dnode;
    Request** req_arr;
    int total;
    int count=0;
    int i;

    found_cust = customerFind(customer_tree, cust_id);

    if (found_cust == NULL) {
        printf("ERROR!! Customer with ID %d does not exist.\n", cust_id);
    }
    else {
        total = requestTreeCount(req_tree);
        req_arr = (Request**)malloc((total + 1) * sizeof(Request*));

        dnode = req_tree->first;
        while (dnode != NULL) {
            i = 0;
            while (i < dnode->occupancy) {
                curr = dnode->records[i];

                if (curr->reqCustomer == found_cust) {
                    req_arr[count] = curr;
                    count = count + 1;
                }
                i++;
            }
            dnode = dnode->next;
        }

        if (count == 0) {
            printf("No requests found for customer %d - %s.\n", cust_id, found_cust->name);
        }
        else {
            requestSortChronological(req_arr, count);

            printf("\nRequests for Customer %d - %s (Chronological Order)\n", cust_id, found_cust->name);
            printRequestArray(req_arr, count);
        }
        free(req_arr);
        ret_val = SUCCESS;
    }

    return ret_val;
}


status_code reportCustomerRangeRequests(RequestBPTree* req_tree, CustomerBPTree* customer_tree, int low, int high) {
    status_code ret_val=FAILURE;
    Request* curr;
    RequestDataNode* req_node;
    Request** req_arr;
    int total;
    int cust_total;
    int count=0;
    int i;
    int cid;

    if (low <= 0 || low >= high) {
        printf("ERROR!! C1 must be positive and smaller than C2.\n");
    }
    else {
        cust_total = customerTreeRangeCount(customer_tree, low, high);

        if (cust_total == 0) {
            printf("No customers found with ID between %d and %d.\n", low, high);
        }
        else {
            total = requestTreeCount(req_tree);
            req_arr = (Request**)malloc((total + 1) * sizeof(Request*));

            req_node = req_tree->first;
            while (req_node != NULL) {
                i = 0;
                while (i < req_node->occupancy) {
                    curr = req_node->records[i];
                    cid = curr->reqCustomer->customer_id;

                    if (cid >= low && cid <= high) {
                        req_arr[count] = curr;
                        count = count + 1;
                    }
                    i++;
                }
                req_node = req_node->next;
            }

            printf("\n%d customer(s) found with ID between %d and %d\n", cust_total, low, high);

            if (count == 0) {
                printf("No requests made by these customers.\n");
            }
            else {
                requestSortChronological(req_arr, count);

                printf("Requests of these customers (Chronological Order)\n\n");
                printRequestArray(req_arr, count);
            }
            free(req_arr);
        }

        ret_val = SUCCESS;
    }

    return ret_val;
}


status_code reportServiceDemand(RequestBPTree* req_tree, ServiceBPTree* service_tree) {
    status_code ret_val=FAILURE;
    ServiceCount* svc_counts;
    Service* p_svc;
    ServiceDataNode* svc_node;
    int svc_total;
    int i;
    int j;

    svc_total = serviceTreeCount(service_tree);

    if (svc_total == 0) {
        printf("No services available in the system.\n");
    }
    else {
        svc_counts = (ServiceCount*)malloc(svc_total * sizeof(ServiceCount));

        i = 0;
        svc_node = service_tree->first;
        while (svc_node != NULL) {
            j = 0;
            while (j < svc_node->occupancy) {
                p_svc = svc_node->records[j];
                svc_counts[i].service = p_svc;
                svc_counts[i].count = countRequestsOfService(req_tree, p_svc);
                i++;
                j++;
            }
            svc_node = svc_node->next;
        }

        sortServiceCounts(svc_counts, svc_total);

        printf("\n_*_*_*_*_*_*_*_*_*_*_SERVICE REPORT _*_*_*_*_*_*_*_*_\n");
        printf("%-12s %-35s %-15s %-12s\n", "Service ID", "Service Name", "Base Price", "Total Requests");
        printf("_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_\n");

        i = 0;
        while (i < svc_total) {
            p_svc = svc_counts[i].service;
            printf("%-12d %-35s Rs.%-12.2f %-15d\n", p_svc->service_id, p_svc->name, p_svc->base_price, svc_counts[i].count);
            i++;
        }

        printf("_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_\n");

        free(svc_counts);
        ret_val = SUCCESS;
    }

    return ret_val;
}


status_code reportProfessionalActivity(RequestBPTree* req_tree, ProfessionalBPTree* pro_tree) {
    status_code ret_val=FAILURE;
    ProfessionalCount* pro_counts;
    Professional* p_pro;
    ProfessionalDataNode* pro_node;
    int pro_total;
    int i;
    int j;

    pro_total = professionalTreeCount(pro_tree);

    if (pro_total == 0) {
        printf("No professionals registered in the system.\n");
    }
    else {
        pro_counts = (ProfessionalCount*)malloc(pro_total * sizeof(ProfessionalCount));

        i = 0;
        pro_node = pro_tree->first;
        while (pro_node != NULL) {
            j = 0;
            while (j < pro_node->occupancy) {
                p_pro = pro_node->records[j];
                pro_counts[i].professional = p_pro;
                pro_counts[i].count = countCompletedByProfessional(req_tree, p_pro);
                i++;
                j++;
            }
            pro_node = pro_node->next;
        }

        sortProfessionalCounts(pro_counts, pro_total);

        printf("\n_*_*_*_*_*_*_*_*_PROFESSIONAL ACTIVITY REPORT _*_*_*_*_*_*_*_*_\n");
        printf("%-15s %-30s %-12s %-20s\n", "Professional ID", "Name", "Pincode", "Completed Requests");
        printf("_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_\n");

        i = 0;
        while (i < pro_total) {
            p_pro = pro_counts[i].professional;
            printf("%-15d %-30s %-12d %-20d\n", p_pro->professional_id, p_pro->name, p_pro->pincode, pro_counts[i].count);
            i++;
        }

        printf("_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_\n");

        free(pro_counts);
        ret_val = SUCCESS;
    }

    return ret_val;
}

void reportPendingRequests(RequestBPTree* req_tree) {    
    Request* curr;
    RequestDataNode* dnode;
    Request** req_arr;
    int total;
    int count=0;
    int i;

    total = requestTreeCount(req_tree);
    req_arr = (Request**)malloc((total + 1) * sizeof(Request*));

    dnode = req_tree->first;
    while (dnode != NULL) {
        i = 0;
        while (i < dnode->occupancy) {
            curr = dnode->records[i];

            if (curr->reqStatus == PENDING) {
                req_arr[count] = curr;
                count = count + 1;
            }
            i++;
        }
        dnode = dnode->next;
    }

    printf("\n_*_*_*_*_*_*_*_*_ PENDING SERVICE REQUESTS _*_*_*_*_*_*_*_*_*_\n");

    if (count == 0) {
        printf("No pending requests in the system.\n");
    }
    else {
        requestSortChronological(req_arr, count);
        printRequestArray(req_arr, count);
    }
    printf("_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_\n");
    free(req_arr);
}

status_code reportServiceHistory(RequestBPTree* req_tree, ProfessionalBPTree* pro_tree, int pro_id) {
    status_code ret_val=FAILURE;
    Professional* found_pro;
    Request* curr;
    RequestDataNode* dnode;
    Request** req_arr;
    int total;
    int count=0;
    int i;

    found_pro = professionalFind(pro_tree, pro_id);

    if (found_pro == NULL) {
        printf("ERROR!! Professional with ID %d does not exist.\n", pro_id);
    }
    else {
        total = requestTreeCount(req_tree);
        req_arr = (Request**)malloc((total + 1) * sizeof(Request*));

        dnode = req_tree->first;
        while (dnode != NULL) {
            i = 0;
            while (i < dnode->occupancy) {
                curr = dnode->records[i];

                if (curr->reqStatus == COMPLETED && curr->reqProfessional == found_pro) {
                    req_arr[count] = curr;
                    count = count + 1;
                }
                i++;
            }
            dnode = dnode->next;
        }

        if (count == 0) {
            printf("No completed requests for professional %d - %s\n", pro_id, found_pro->name);
        }
        else {
            requestSortChronological(req_arr, count);

            printf("\nService History for Professional %d - %s [Completed Requests]\n", pro_id, found_pro->name);
            printRequestArray(req_arr, count);
        }
        free(req_arr);
        ret_val = SUCCESS;
    }

    return ret_val;
}
