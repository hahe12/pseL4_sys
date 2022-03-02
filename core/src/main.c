
#include <stdio.h>
#include <serviceLib.h>
#include <psel4_lib.h>

extern void procLibInit(void);
extern void netLibInit(void);
extern void vfsLibInit(void);

// print logo
void print_logo()
{
    printf("hello pseL4!!!!\n");
}

// start init program
void init_start (void) {
        /* spawn first program, sh */
    psel4_process process;
    
    seL4_Word argc = 1;
    char string_args[argc][WORD_STRING_SIZE];
    char *argv[argc];

    /* start shell program */
    psel4_process_start (&process, "init", seL4_MaxPrio, argc, argv);

}

// root task entry 
int main(int argc, char *argv[]) {
    psel4_sys_init();
    print_logo();

    NameServerInit();

    procLibInit();
    netLibInit();
    vfsLibInit();

    init_start();

    /* endpoint or notification handle */
    psel4_handle();

    return 0;
}


