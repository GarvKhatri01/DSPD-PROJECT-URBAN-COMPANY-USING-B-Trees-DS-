#ifndef REPORT_H
#define REPORT_H

#include "common.h"
#include "request.h"

status_code reportAvailableProfessionals(ProfessionalBPTree* pro_tree, ServiceBPTree* service_tree, int svc_id);

status_code reportServices(ProfessionalBPTree* pro_tree, ServiceBPTree* service_tree, int pro_id);

status_code reportCustomerRequests(RequestBPTree* req_tree, CustomerBPTree* customer_tree, int cust_id);

status_code reportCustomerRangeRequests(RequestBPTree* req_tree, CustomerBPTree* customer_tree, int low, int high);

status_code reportServiceDemand(RequestBPTree* req_tree, ServiceBPTree* service_tree);

status_code reportProfessionalActivity(RequestBPTree* req_tree, ProfessionalBPTree* pro_tree);

void reportPendingRequests(RequestBPTree* req_tree);

status_code reportServiceHistory(RequestBPTree* req_tree, ProfessionalBPTree* pro_tree, int pro_id);

#endif
