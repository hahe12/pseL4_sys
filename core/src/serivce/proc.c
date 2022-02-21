#include <serviceLib.h>
#include <sel4/sel4.h>

void procHandle(Service *service, seL4_MessageInfo_t msg);

static Service proc_service = {
    .name = "proc",
    .handle = procHandle
};

void procLibInit(void) {
    NameServerRegister (&proc_service);
}

void procHandle(Service *service, seL4_MessageInfo_t msg){

}