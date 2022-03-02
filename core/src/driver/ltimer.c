#include <platsupport/ltimer.h>

/* fake timer routine */

static int set_timeout(void *data, uint64_t ns, timeout_type_t type)
{
    return 0;
}

int ltimer_default_init(ltimer_t *ltimer, ps_io_ops_t ops, ltimer_callback_fn_t callback, void *callback_token) {
    ltimer->set_timeout = set_timeout;
    return 0;
}
