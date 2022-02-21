#include <serviceLib.h>
#include <sel4/sel4.h>

void vfsHandle(Service *service, seL4_MessageInfo_t msg);

static Service proc_service = {
    .name = "vfs",
    .handle = vfsHandle
};

void vfsLibInit(void) {
    NameServerRegister (&proc_service);
}

void vfsHandle(Service *service, seL4_MessageInfo_t msg){

}