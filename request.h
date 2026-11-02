#ifndef REQUEST_H
#define REQUEST_H

#include "common.h"
#include "request_tree.h"
#include "customer.h"
#include "service.h"
#include "professional.h"

typedef struct request_tag {
    int reqID;
    Customer* reqCustomer;
    Service* reqService;
    Professional* reqProfessional;
    DateTime reqDateTime;
    RequestStatus reqStatus;
} Request;


status_code requestIsValidDateTime(DateTime dt);

status_code requestCreate(RequestBPTree* req_tree, CustomerBPTree* customer_tree, ServiceBPTree* service_tree, int id,
                          int cust_id, int svc_id, DateTime dt);

Request* requestFind(RequestBPTree* tree, int id);

boolean requestExists(RequestBPTree* tree, int id);

int requestCompareDateTime(DateTime a, DateTime b);

boolean requestIsEarlier(Request* a, Request* b);

void requestSortChronological(Request* arr[], int n);

status_code requestAssign(Request* req, Professional* pro);

status_code requestComplete(RequestBPTree* tree, int id, Professional** pptr);

status_code requestCancel(RequestBPTree* tree, int id, Professional** pptr);

char* requestStatustoString(RequestStatus status);

void requestDisplayOne(Request* req);

void requestDisplayAll(RequestBPTree* tree);

void requestFreeAll(RequestBPTree* tree);

#endif
