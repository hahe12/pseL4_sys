#include <serviceLib.h>
#include <sel4/sel4.h>

void netHandle(Service *service, seL4_MessageInfo_t msg);

static Service proc_service = {
    .name = "net",
    .handle = netHandle
};

void netLibInit(void) {
    NameServerRegister (&proc_service);
}

void netHandle(Service *service, seL4_MessageInfo_t msg){

}