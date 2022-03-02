#include <psel4_lib.h>
#include <muslcsys/vsyscall.h>
#include <platsupport/ltimer.h>

/* dimensions of virtual memory for the allocator to use */
#define ALLOCATOR_VIRTUAL_POOL_SIZE ((1 << seL4_PageBits) * 400)

/* static memory for the allocator to bootstrap with */
#define ALLOCATOR_STATIC_POOL_SIZE ((1 << seL4_PageBits) * 100)
static char allocator_mem_pool[ALLOCATOR_STATIC_POOL_SIZE];

/* static memory for virtual memory bootstrapping */
static sel4utils_alloc_data_t data;

/* root task resource */
static simple_t      simple;
static allocman_t *  allocman;
static vka_t         vka;    
static vspace_t      vspace;
static vka_object_t  name_server_ep = {0};
static cspacepath_t  name_server_ep_path;
static ltimer_t      timer;
static vka_object_t  ntfn_object = {0};
static psel4_process root_process = {0};

/* globals for malloc */
extern vspace_t *muslc_this_vspace;
extern reservation_t muslc_brk_reservation;
extern void *muslc_brk_reservation_start;
static sel4utils_res_t malloc_res;

static void CONSTRUCTOR(MUSLCSYS_WITH_VSYSCALL_PRIORITY)  psel4_bootstrap (void) {
    int error;

    seL4_BootInfo *info = platsupport_get_bootinfo(); // seL4_GetBootInfo();
    simple_default_init_bootinfo(&simple, info);
    
    /* create allocator */
    allocman = bootstrap_use_current_simple(&simple, ALLOCATOR_STATIC_POOL_SIZE, allocator_mem_pool);
    assert(allocman);

    /* create vka */
    allocman_make_vka(&vka, allocman);

    /* create vspace */
    error = sel4utils_bootstrap_vspace_with_bootinfo_leaky(&vspace, &data, simple_get_pd(&simple),
                                                           &vka, info);

    /* set up malloc */
    error = sel4utils_reserve_range_no_alloc(&vspace, &malloc_res, seL4_LargePageBits, seL4_AllRights, 1,
                                             &muslc_brk_reservation_start);
    muslc_this_vspace = &vspace;
    muslc_brk_reservation.res = &malloc_res;
    ZF_LOGF_IF(error, "Failed to set up dynamic malloc");

    void *vaddr;
    reservation_t virtual_reservation = vspace_reserve_range(&vspace, ALLOCATOR_VIRTUAL_POOL_SIZE, seL4_AllRights,
                                               1, &vaddr);
    assert(virtual_reservation.res);
    bootstrap_configure_virtual_pool(allocman, vaddr, ALLOCATOR_VIRTUAL_POOL_SIZE, simple_get_pd(&simple));
    
}

void psel4_sys_init (void)
{
    int error;

    /* create an endpoint */
    error = vka_alloc_endpoint(&vka, &name_server_ep);
    vka_cspace_make_path(&vka, name_server_ep.cptr, &name_server_ep_path);
    assert(error == 0);

    error = vka_alloc_notification(&vka, &ntfn_object);

    ps_io_ops_t ops = {{0}};
    error = sel4platsupport_new_malloc_ops(&ops.malloc_ops);
    assert(error == 0);
    error = sel4platsupport_new_io_mapper(&vspace, &vka, &ops.io_mapper);
    assert(error == 0);
    error = sel4platsupport_new_fdt_ops(&ops.io_fdt, &simple, &ops.malloc_ops);
    assert(error == 0);
    if (ntfn_object.cptr != seL4_CapNull) {
        error = sel4platsupport_new_mini_irq_ops(&ops.irq_ops, &vka, &simple, &ops.malloc_ops,
                                                 ntfn_object.cptr, MASK(seL4_BadgeBits));
        assert(error == 0);
    }
    error = sel4platsupport_new_arch_ops(&ops, &simple, &vka);
    assert(error == 0);

    /* init timer */
    ltimer_default_init(&timer, ops, NULL, NULL);
    assert(error == 0);

    error = ltimer_set_timeout(&timer, 1, TIMEOUT_PERIODIC);
    assert(error == 0);
}


typedef enum
{
    SC_unknown = 0,
    SC_get_time,
    SC_last // Not a real ID, just here to count ids
} pseL4_syscall;

void Syscall_GetTime (psel4_thread* caller, seL4_MessageInfo_t info) {

}

typedef void (*SyscallFunc)(psel4_thread* caller, seL4_MessageInfo_t info);
static SyscallFunc syscallTable[] =
{
    NULL,
    Syscall_GetTime
};

void psel4_handle (void) {
    seL4_Word sender = 0;
    seL4_MessageInfo_t info;
    psel4_thread *caller;
    pseL4_syscall sid;

    while (1) {
        info = seL4_Recv(ntfn_object.cptr, &sender);
        switch (seL4_MessageInfo_get_label(info)) {
            case seL4_Fault_NullFault:
                caller = (psel4_thread *) sender;
                sid = seL4_GetMR(0);

                if (sid >= SC_last)
                    return;
                
                syscallTable[sid] (caller, info);

                break;
            default:
                break;
        }

        printf ("receive a timer interrupt\\n");
    }
}

int psel4_process_start (psel4_process *process, const char *name, uint8_t prio, int argc, char *argv[]) {
    int error;

    sel4utils_process_t *sel4_process = &process->sel4_process;

    /* use sel4utils to make a new process */
    sel4utils_process_config_t config = process_config_default_simple(&simple, name, prio);
    config = process_config_auth(config, simple_get_tcb(&simple));
    config = process_config_priority(config, seL4_MaxPrio);
    error = sel4utils_configure_process_custom(sel4_process, &vka, &vspace, config);
    assert(error == 0);

    seL4_CPtr new_ep_cap;

    /* copy the endpoint cap and add a badge to the new cap */
    new_ep_cap = sel4utils_mint_cap_to_process(sel4_process, name_server_ep_path,
                                               seL4_AllRights, (seL4_Word)process);
    assert(new_ep_cap != 0);

    /* spawn the process */
    error = sel4utils_spawn_process_v(sel4_process, &vka, &vspace, 0, NULL, 1);
    assert(error == 0);
}

int psel4_thread_start (psel4_process *process, psel4_thread * thread, sel4utils_thread_entry_fn entry_point,
                        uint8_t prio, void *arg0, void *arg1, int resume) {
    
    sel4utils_thread_config_t config;
    sel4utils_thread_t       *sel4_thread = &thread->sel4_thread;
    sel4utils_process_t      *sel4_process = &process->sel4_process;
    seL4_CPtr                 fault_ep;
    int                       error;

    seL4_CPtr new_ep_cap;
    cspacepath_t new_ep_cap_path;
    
    if (process == NULL) {
        vka_cspace_alloc(&vka, &new_ep_cap);
        vka_cspace_make_path(&vka, new_ep_cap, &new_ep_cap_path);
        vka_cnode_mint (&new_ep_cap_path, &name_server_ep_path, seL4_AllRights, (seL4_Word)thread);
         
        thread->process = &root_process;
    } else {
        /* copy the endpoint cap and add a badge to the new cap */
        new_ep_cap = sel4utils_mint_cap_to_process(sel4_process, name_server_ep_path,
                                                   seL4_AllRights, (seL4_Word)thread);
        thread->process = process;
    }
    
    sel4utils_thread_config_t thread_config = thread_config_new(&simple);

    thread_config = thread_config_fault_endpoint(thread_config, new_ep_cap);

    error = sel4utils_configure_thread_config(&vka, &vspace, &sel4_process->vspace, 
                                              thread_config,  sel4_thread);
    assert(error == 0);

    error = seL4_TCB_SetPriority(sel4_thread->tcb.cptr, seL4_CapInitThreadTCB, prio);
    assert(error == 0);

    return sel4utils_start_thread (sel4_thread, entry_point, arg0, arg1, resume);
}