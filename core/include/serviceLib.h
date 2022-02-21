
#pragma once

#include <sel4/sel4.h>
#include <utils/uthash.h>
typedef struct _Service
{
    char* name;
    void (*handle)(struct _Service* service, seL4_MessageInfo_t msg);
    UT_hash_handle hh;
} Service;


int NameServerInit();
int NameServerRegister(Service* s);
void NameServiceStart();