#include "request.h"

#define MIN_YEAR 2020
#define MAX_YEAR 2100
#define MIN_MONTH 1
#define MAX_MONTH 12
#define MIN_HOUR 0
#define MAX_HOUR 23
#define MIN_MINUTE 0
#define MAX_MINUTE 59
#define DAYS_LONG_MONTH 31
#define DAYS_SHORT_MONTH 30
#define DAYS_FEB_LEAP 29
#define DAYS_FEB_COMMON 28

boolean isLeapYear(int year) {
    boolean ret_val=FALSE;

    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        ret_val = TRUE;
    }

    return ret_val;
}


int daysInMonth(int month, int year) {
    int ret_val=DAYS_LONG_MONTH;

    if (month == 4 || month == 6 || month == 9 || month == 11) {
        ret_val = DAYS_SHORT_MONTH;
    }
    else if (month == 2) {
        if (isLeapYear(year) == TRUE) {
            ret_val = DAYS_FEB_LEAP;
        }
        else {
            ret_val = DAYS_FEB_COMMON;
        }
    }

    return ret_val;
}


status_code requestIsValidDateTime(DateTime dt) {
    status_code ret_val=FAILURE;
    Date d=dt.date;
    Time t=dt.time;
    int max_day;

    if (d.year >= MIN_YEAR && d.year <= MAX_YEAR &&
        d.month >= MIN_MONTH && d.month <= MAX_MONTH) {

        max_day = daysInMonth(d.month, d.year);

        if (d.day >= 1 && d.day <= max_day &&
            t.hour >= MIN_HOUR && t.hour <= MAX_HOUR &&
            t.minute >= MIN_MINUTE && t.minute <= MAX_MINUTE) {
            ret_val = SUCCESS;
        }
    }

    return ret_val;
}


int requestCompareDateTime(DateTime a, DateTime b) {
    int ret_val=0;
    Date da=a.date;
    Date db=b.date;
    Time ta=a.time;
    Time tb=b.time;

    if (da.year != db.year) {
        ret_val = da.year - db.year;
    }
    
    else if (da.month != db.month) {
        ret_val = da.month - db.month;
    }

    else if (da.day != db.day) {
        ret_val = da.day - db.day;
    }

    else if (ta.hour != tb.hour) {
        ret_val = ta.hour - tb.hour;
    }

    else if (ta.minute != tb.minute) {
        ret_val = ta.minute - tb.minute;
    }

    return ret_val;
}


boolean requestIsEarlier(Request* a, Request* b) {
    boolean ret_val=FALSE;
    int cmp;

    cmp = requestCompareDateTime(a->reqDateTime, b->reqDateTime);

    if (cmp < 0) {
        ret_val = TRUE;
    }
    else if (cmp == 0 && a->reqID < b->reqID) {
        ret_val = TRUE;
    }

    return ret_val;
}


//HEAP SORT PENDING
void requestSiftDown(Request* arr[], int root, int n) {
    int child;
    Request* temp;
    boolean done=FALSE;

    while (2 * root + 1 < n && done == FALSE) {
        child = 2 * root + 1;

        if (child + 1 < n && requestIsEarlier(arr[child], arr[child+1]) == TRUE) {
            child = child + 1;
        }

        if (requestIsEarlier(arr[root], arr[child]) == TRUE) {
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

void requestSortChronological(Request* arr[], int n) {
    int i;
    Request* temp;

    i = n / 2 - 1;
    while (i >= 0) {
        requestSiftDown(arr, i, n);
        i--;
    }

    i = n - 1;
    while (i > 0) {
        temp = arr[0];
        arr[0] = arr[i];
        arr[i] = temp;

        requestSiftDown(arr, 0, i);
        i--;
    }
}



Request* requestFind(RequestBPTree* tree, int id) {
    return requestTreeSearch(tree, id);
}

boolean requestExists(RequestBPTree* tree, int id) {
    boolean ret_val=FALSE;

    if (requestFind(tree, id) != NULL) {
        ret_val = TRUE;
    }

    return ret_val;
}


status_code requestCreate(RequestBPTree* req_tree, CustomerBPTree* customer_tree, ServiceBPTree* service_tree, int id,
                          int cust_id, int svc_id, DateTime dt) {
    status_code ret_val=FAILURE;
    Request* newNode=NULL;
    Customer* cptr=NULL;
    Service* sptr=NULL;

    if (id <= 0) {
        printf("ERROR!! Request ID must be > 0\n");
    }
    else if (requestExists(req_tree, id) == TRUE) {
        printf("ERROR!! Request with ID %d already exists.\n", id);
    }
    else {
        cptr = customerFind(customer_tree, cust_id);
        sptr = serviceFind(service_tree, svc_id);

        if (cptr == NULL) {
            printf("ERROR!!! Customer with ID %d does not exist.\n", cust_id);
        }
        else if (sptr == NULL) {
            printf("ERROR!! Service with ID %d does not exist.\n", svc_id);
        }
        else if (requestIsValidDateTime(dt) == FAILURE) {
            printf("ERROR!! Invalid date or time supplied for request\n");
        }
        else {
            newNode = (Request*)malloc(sizeof(Request));

            if (newNode == NULL) {
                printf("ERROR!!! Memory allocation failed for request\n");
            }
            else {
                newNode->reqID = id;
                newNode->reqCustomer = cptr;
                newNode->reqService = sptr;
                newNode->reqProfessional = NULL;
                newNode->reqDateTime = dt;
                newNode->reqStatus = PENDING;

                requestTreeInsert(req_tree, id, newNode);

                printf("Request ID %d created (PENDING)\n  "
                       "Customer: %d, Service: %d.\n",
                       id, cust_id, svc_id);

                ret_val = SUCCESS;
            }
        }
    }

    return ret_val;
}


status_code requestAssign(Request* req, Professional* pro) {
    status_code ret_val=FAILURE;
    int svc_id;

    if (req == NULL || pro == NULL) {
        printf("ERROR!!! null pointer.\n");
    }
    else if (req->reqStatus != PENDING) {
        printf("ERROR!! Request ID %d is not PENDING (status: %s).\n", req->reqID, requestStatustoString(req->reqStatus));
    }
    else if (professionalIsAvailable(pro) == FALSE) {
        printf("ERROR!! Professional ID %d is not available.\n", pro->professional_id);
    }
    else {
        svc_id = req->reqService->service_id;

        if (professionalOffers(pro, svc_id) == FALSE) {
            printf("ERROR!! Professional ID %d does not offer service ID %d.\n", pro->professional_id, svc_id);
        }
        else {
            req->reqProfessional = pro;
            req->reqStatus = ASSIGNED;

            professionalSetAvailability(pro, BUSY);

            printf("Request ID %d assigned to professional ID %d\n", req->reqID, pro->professional_id);
            ret_val = SUCCESS;
        }
    }

    return ret_val;
}


status_code requestComplete(RequestBPTree* tree, int id, Professional** pptr) {
    status_code ret_val=FAILURE;
    Request* req=NULL;
    Professional* pro;

    if (pptr != NULL) {
        *pptr = NULL;
    }

    req = requestFind(tree, id);

    if (req == NULL) {
        printf("ERROR!! Request ID %d not found\n", id);
    }
    else if (req->reqStatus == PENDING) {
        printf("ERROR!! Request ID %d is PENDING\nIt has no assigned professional and cannot be completed\n", id);
    }
    else if (req->reqStatus == COMPLETED) {
        printf("ERROR!! Request ID %d is already COMPLETED.\n", id);
    }
    else if (req->reqStatus == CANCELLED) {
        printf("ERROR!! Request ID %d is CANCELLED\n", id);
    }
    else {
        pro = req->reqProfessional;
        req->reqStatus = COMPLETED;

        professionalSetAvailability(pro, AVAILABLE);

        if (pptr != NULL) {
            *pptr = pro;
        }

        printf("Request ID %d marked COMPLETED\nProfessional ID %d is now available\n", id, pro->professional_id);
        ret_val = SUCCESS;
    }

    return ret_val;
}


status_code requestCancel(RequestBPTree* tree, int id, Professional** pptr) {
    status_code ret_val=FAILURE;
    Request* req=NULL;
    Professional* pro=NULL;

    if (pptr != NULL) {
        *pptr = NULL;
    }

    req = requestFind(tree, id);

    if (req == NULL) {
        printf("ERROR!! Request ID %d not found\n", id);
    }
    else if (req->reqStatus == COMPLETED) {
        printf("ERROR!! Request ID %d is already COMPLETED\n", id);
    }
    else if (req->reqStatus == CANCELLED) {
        printf("ERROR!! Request ID %d is already CANCELLED\n", id);
    }
    else {
        if (req->reqStatus == ASSIGNED) {
            pro = req->reqProfessional;
            professionalSetAvailability(pro, AVAILABLE);

            if (pptr != NULL) {
                *pptr = pro;
            }

            req->reqProfessional = NULL;

            printf("Request ID %d CANCELLED\nProfessional ID %d is now available\n", id, pro->professional_id);
        }
        else {
            printf("Request ID %d CANCELLED\n", id);
        }

        req->reqStatus = CANCELLED;
        ret_val = SUCCESS;
    }

    return ret_val;
}


char* requestStatustoString(RequestStatus status) {
    char* ret_val="Unknown";

    if (status == PENDING) {
        ret_val = "Pending";
    }
    else if (status == ASSIGNED) {
        ret_val = "Assigned";
    }
    else if (status == COMPLETED) {
        ret_val = "Completed";
    }
    else if (status == CANCELLED) {
        ret_val = "Cancelled";
    }

    return ret_val;
}


void requestDisplayOne(Request* req) {
    Customer* cptr;
    Service* sptr;
    Professional* pro;
    Date d;
    Time t;

    if (req == NULL) {
        printf("Request record not found\n");
    }
    else {
        cptr = req->reqCustomer;
        sptr = req->reqService;
        pro = req->reqProfessional;
        d = req->reqDateTime.date;
        t = req->reqDateTime.time;

        printf("Request ID: %d\n", req->reqID);
        printf("Customer: ID %d - %s\n", cptr->customer_id, cptr->name);
        printf("Service: ID %d - %s\n", sptr->service_id, sptr->name);

        if (pro != NULL) {
            printf("Professional: ID %d - %s\n", pro->professional_id, pro->name);
        }
        else {
            printf("Professional: None\n");
        }

        printf("Date / Time: %02d/%02d/%04d %02d:%02d\n", d.day, d.month, d.year, t.hour, t.minute);
        printf("Status: %s\n\n", requestStatustoString(req->reqStatus));
    }
}

void requestDisplayAll(RequestBPTree* tree) {
    Request** all;
    int total;
    int i;

    all = requestTreeToArray(tree, &total);

    if (total == 0) {
        printf("No requests yet.\n");
    }
    else {
        printf("_*_*_*_*_*_*_*_ Request List _*_*_*_*_*_*_*_\n");
        i = 0;
        while (i < total) {
            requestDisplayOne(all[i]);
            i++;
        }
    }

    free(all);
}

void requestFreeAll(RequestBPTree* tree) {
    Request** all;
    int total;
    int i=0;

    all = requestTreeToArray(tree, &total);

    while (i < total) {
        free(all[i]);
        i++;
    }

    free(all);
    requestTreeFree(tree);
}
