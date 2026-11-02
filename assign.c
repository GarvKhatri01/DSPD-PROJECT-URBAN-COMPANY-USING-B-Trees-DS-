#include "assign.h"
#include <limits.h>

boolean isSuitableProfessional(Professional* pro, int svc_id) {
    boolean ret_val=FALSE;

    if (pro != NULL && professionalIsAvailable(pro) == TRUE && professionalOffers(pro, svc_id) == TRUE) {
        ret_val = TRUE;
    }

    return ret_val;
}


Professional* assignFindProfessional(ProfessionalBPTree* pro_tree, Request* req) {
    Professional* ans_pro=NULL;
    Professional* curr;
    ProfessionalDataNode* dnode;
    int i;
    int cust_pin;
    int svc_id;
    int min_diff=INT_MAX;
    int diff;

    if (req != NULL && req->reqCustomer != NULL && req->reqService != NULL) {
        
        cust_pin = req->reqCustomer->pincode;
        svc_id = req->reqService->service_id;

        dnode = pro_tree->first;
        
        while (dnode != NULL) {
            i = 0;
            while (i < dnode->occupancy) {
                curr = dnode->records[i];

                if (isSuitableProfessional(curr, svc_id) == TRUE) {
                    diff = abs(curr->pincode - cust_pin);

                    if (diff < min_diff) {
                        min_diff = diff;
                        ans_pro = curr;
                    }
                }
                i++;
            }
            dnode = dnode->next;
        }
    }

    return ans_pro;
}


Request* assignFindPendingRequest(RequestBPTree* req_tree, Professional* pro) {
    Request* ans_req=NULL;
    Request* curr;
    RequestDataNode* dnode;
    int i;
    int min_diff=INT_MAX;
    int diff;

    if (pro != NULL) {
        dnode = req_tree->first;
        while (dnode != NULL) {
            i = 0;
            while (i < dnode->occupancy) {
                curr = dnode->records[i];

                if (curr->reqStatus == PENDING && professionalOffers(pro, curr->reqService->service_id) == TRUE) {
                    diff = abs(pro->pincode - curr->reqCustomer->pincode);

                    if (diff < min_diff) {
                        min_diff = diff;
                        ans_req = curr;
                    }

                    else if (diff == min_diff && requestIsEarlier(curr, ans_req) == TRUE) {
                        ans_req = curr;
                    }
                }
                i++;
            }
            dnode = dnode->next;
        }
    }

    return ans_req;
}


status_code assignNewRequest(ProfessionalBPTree* pro_tree, Request* req) {
    status_code ret_val=FAILURE;
    Professional* ans_pro;

    if (req == NULL) {
        printf("ERROR!! Request pointer is NULL\n");
    }
    else {
        ans_pro = assignFindProfessional(pro_tree, req);

        if (ans_pro == NULL) {
            printf("No available professional right now, request is still PENDING\n");
        }
        else {
            ret_val = requestAssign(req, ans_pro);

            if (ret_val == SUCCESS) {
                printf("Customer Pincode: %d\n", req->reqCustomer->pincode);
                printf("Professional Pincode: %d\n", ans_pro->pincode);
            }
        }
    }

    return ret_val;
}


status_code assignFreedProfessional(RequestBPTree* req_tree, Professional* pro) {
    status_code ret_val=FAILURE;
    Request* chosen_req;

    if (pro == NULL) {
        printf("ERROR!! Professional pointer is NULL.\n");
    }
    else if (professionalIsAvailable(pro) == FALSE) {
        printf("ERROR!! Professional is not available for assignment.\n");
    }
    else {
        chosen_req = assignFindPendingRequest(req_tree, pro);

        if (chosen_req == NULL) {
            printf("No pending request for the professional\n");
        }
        else {
            ret_val = requestAssign(chosen_req, pro);

            if (ret_val == SUCCESS) {
                printf("Pending Request %d assigned to Professional %d\n", chosen_req->reqID, pro->professional_id);
            }
        }
    }

    return ret_val;
}
