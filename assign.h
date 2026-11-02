#ifndef ASSIGN_H
#define ASSIGN_H

#include "common.h"
#include "professional.h"
#include "request.h"

Professional* assignFindProfessional(ProfessionalBPTree* pro_tree, Request* req);

Request* assignFindPendingRequest(RequestBPTree* req_tree, Professional* pro);

status_code assignNewRequest(ProfessionalBPTree* pro_tree, Request* req);

status_code assignFreedProfessional(RequestBPTree* req_tree, Professional* pro);

#endif
