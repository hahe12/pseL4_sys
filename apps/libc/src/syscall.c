
#include <bits/syscall.h>
#include <stdarg.h>

extern void *__sysinfo;

long pseL4_vsyscall(long sysnum, ...)
{
    va_list al;
    va_start(al, sysnum);
    long ret = -1;
    switch (sysnum)
    {
    default:
        break;
    }
    
    va_end(al);
    return ret;
}


void initMuslSysCalls(void)
{
    __sysinfo = pseL4_vsyscall;
}