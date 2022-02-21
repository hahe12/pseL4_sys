
#include <stdio.h>
#include <serviceLib.h>

extern void init_sys (void);
extern void procLibInit(void);
extern void netLibInit(void);
extern void vfsLibInit(void);

void print_log()
{
    printf("hello pseL4!!!!\n");
}

int main(int argc, char *argv[]) {
    init_sys();
    print_log();
    NameServerInit();

    procLibInit();
    netLibInit();
    vfsLibInit();


    return 0;
}


