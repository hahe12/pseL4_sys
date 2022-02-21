
#include <allocman/bootstrap.h>
#include <sel4utils/vspace.h>
#include <sel4platsupport/bootinfo.h>
#include <simple-default/simple-default.h>
#include <simple/simple_helpers.h>
#include <allocman/vka.h>
#include <sel4platsupport/io.h>

#include <muslcsys/vsyscall.h>

/* dimensions of virtual memory for the allocator to use */
#define ALLOCATOR_VIRTUAL_POOL_SIZE ((1 << seL4_PageBits) * 400)

/* static memory for the allocator to bootstrap with */
#define ALLOCATOR_STATIC_POOL_SIZE ((1 << seL4_PageBits) * 100)
static char allocator_mem_pool[ALLOCATOR_STATIC_POOL_SIZE];

/* static memory for virtual memory bootstrapping */
static sel4utils_alloc_data_t data;

/* root task resource */
static simple_t    simple;
static allocman_t *allocman;
static vka_t       vka;    
static vspace_t    vspace;

/* globals for malloc */
extern vspace_t *muslc_this_vspace;
extern reservation_t muslc_brk_reservation;
extern void *muslc_brk_reservation_start;
static sel4utils_res_t malloc_res;

void init_sys (void)
{
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