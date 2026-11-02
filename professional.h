#ifndef PROFESSIONAL_H
#define PROFESSIONAL_H

#include "common.h"
#include "professional_tree.h"
#include "service.h"

typedef struct professional_tag {
    int professional_id;
    char name[MAX_NAME_LEN];
    int pincode;
    char contact[MAX_CONTACT_LEN];
    AvailabilityStatus availability;
    int service_ids[MAX_SERVICES_PER_PROFESSIONAL];
    int service_count;
} Professional;


status_code professionalIsValid(int id, char name[], int pincode, char contact[]);

status_code professionalRegister(ProfessionalBPTree* tree, ServiceBPTree* service_tree, int id,
                                char name[], int pincode, char contact[], int svc_ids[], int svc_count);

Professional* professionalFind(ProfessionalBPTree* tree, int id);

boolean professionalExists(ProfessionalBPTree* tree, int id);

boolean professionalOffers(Professional* ptr, int svc_id);

boolean professionalIsAvailable(Professional* ptr);

void professionalSetAvailability(Professional* ptr, AvailabilityStatus status);

void professionalDisplayOne(Professional* ptr, ServiceBPTree* service_tree);

void professionalDisplayServices(Professional* ptr, ServiceBPTree* service_tree);

void professionalDisplayAll(ProfessionalBPTree* tree, ServiceBPTree* service_tree);

void professionalFreeAll(ProfessionalBPTree* tree);

#endif
