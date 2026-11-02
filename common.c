#include "common.h"

int countDigits(int n) {
    int ret_val;

    ret_val=0;

    while (n>0) {
        ret_val++;
        n/=10;
    }

    return ret_val;
}


status_code contactIsValid(char contact[]) {
    status_code ret_val=FAILURE;
    int contact_len;
    int i=0;
    boolean flag=TRUE;

    if (contact != NULL) {
        contact_len=strlen(contact);

        while (i<contact_len && flag==TRUE) {
            if (contact[i]<'0' || contact[i]>'9') {
                flag=FALSE;
            }
            i++;
        }

        if (contact_len==10 && flag==TRUE) {
            ret_val=SUCCESS;
        }
    }

    return ret_val;
}

status_code nameIsValid(char name[], int max_len) {
    status_code ret_val=FAILURE;

    if (name != NULL && name[0] != '\0' && (int)strlen(name) < max_len && strchr(name, '|') == NULL) {
        ret_val=SUCCESS;
    }

    return ret_val;
}
