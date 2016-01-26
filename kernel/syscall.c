#include <levos/levos.h>

int num_syscalls = 0;

int syscall_null(struct pt_regs *r)
{
    return -ENOSYS;
}

int syscall_exit(struct pt_regs *r)
{
    printk("pid %d: Bye cruel world!\n", current->pid);
    sched_rm_process(current);
    return 0x13371337;
}

int syscall_write(struct pt_regs *r)
{
    int fd       = SYSCALL_ARG1(r);
    char *buf    = (char *) SYSCALL_ARG2(r);
    size_t count = SYSCALL_ARG3(r);

    /* FIXME: check out of bounds */
    struct file *fp = current->file_table[fd];

    if (!fp)
        return -EBADF;

    return fp->fops->write(fp, buf, count);
}

int syscall_stat(struct pt_regs *r)
{
    char *path = (char *) SYSCALL_ARG1(r);
    struct stat *st = (void *) SYSCALL_ARG2(r);

    return vfs_stat(path, st);
}

int syscall_open(struct pt_regs *r)
{
    char *path = (void *) SYSCALL_ARG1(r);
    int flags = SYSCALL_ARG2(r);
    int mode = SYSCALL_ARG4(r);
    struct file *filp;

    filp = vfs_open(path);

    for (int i = 3; i < 128; i++) {
        if (current->file_table[i] == 0) {
            current->file_table[i] = filp;
            return i;
        }
    }

    return -EMFILE;
}

int syscall_read(struct pt_regs *r)
{
    int fd = SYSCALL_ARG1(r);
    char *buf = (void *) SYSCALL_ARG2(r);
    size_t size = SYSCALL_ARG3(r);
    struct file *filp;

    filp = current->file_table[fd];
    if (filp == 0)
        return -EBADF;

    return filp->fops->read(filp, buf, size);
}

int syscall_close(struct pt_regs *r)
{
    int fd = SYSCALL_ARG1(r);

    if (current->file_table[fd] == 0)
        return -EBADF;

    /* FIXME free structures */
    current->file_table[fd] = 0;

    return 0;
}

void *syscalls[256] = {
    0,
    syscall_exit,
    0, /* fork */
    syscall_read,
    syscall_write,
    syscall_open,
    syscall_close,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    syscall_stat,
    0,
    0,
    0,
    0,
};

DEF_IRQ_HANDLER(0x80, syscall_hub)
{
    struct pt_regs *r = &current->r;
    int (*f)(struct pt_regs *);

    printk("Process \"%s\" called system call %d\n",
        current->comm, SYSCALL_NUMBER(r));

    f = syscalls[SYSCALL_NUMBER(r)];
    if (!f)
        f = syscall_null;
    SYSCALL_RETVAL(r) = f(r);
    memcpy(&current->r, r, sizeof(struct pt_regs));
    arch_sched_setup_stack(current);
    ARCH_SWITCH_CONTEXT();
}

int call_syscall(int sys, int a1, int a2, int a3)
{
    int ret = -ENOSYS;

    asm volatile("int $0x80":"=a"(ret)
            :"a"(sys), "b"(a1), "c"(a2), "d"(a3));

    return ret;
}

int syscall_init()
{
    for (int i = 0; i < 256; i++)
        if (syscalls[i])
            num_syscalls ++;

    printk("syscall: there are %d system calls\n", num_syscalls);
    irq_set(0x80, syscall_hub);    
}

