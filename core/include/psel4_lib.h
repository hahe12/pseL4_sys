
#include <allocman/bootstrap.h>
#include <sel4platsupport/bootinfo.h>
#include <simple-default/simple-default.h>
#include <simple/simple_helpers.h>
#include <allocman/vka.h>
#include <sel4platsupport/io.h>
#include <sel4utils/vspace.h>
#include <sel4utils/process.h>
#include <sel4utils/thread.h>
#include <vka/capops.h>

typedef struct _Process {
    sel4utils_process_t sel4_process;
} psel4_process;

typedef struct _Thread{
    psel4_process      *process;
    sel4utils_thread_t sel4_thread;
} psel4_thread;

void psel4_sys_init();
void psel4_handle (void);
int psel4_process_start (psel4_process *process, const char *name, uint8_t prio, int argc, char *argv[]);