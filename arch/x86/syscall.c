#include <levos/levos.h>
#include <levos/arch.h>

int call_syscall(int sys, int a1, int a2, int a3)
{
    int ret = -ENOSYS;

    asm volatile("int $0x80":"=a"(ret)
            :"a"(sys), "b"(a1), "c"(a2), "d"(a3));

    return ret;
}

