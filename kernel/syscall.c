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
	int fd = SYSCALL_ARG1(r);
	char *buf = SYSCALL_ARG2(r);
	size_t count = SYSCALL_ARG3(r);
	/* FIXME: check out of bounds */
	struct file *fp = current->file_table[fd];

	if (!fp)
		return -EBADF;

	return fp->fops->write(fp, buf, count);
}

void *syscalls[256] = {
	0,
	syscall_exit,
	0, /* fork */
	0, /* read */
	syscall_write,
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

int syscall_init()
{
	for (int i = 0; i < 256; i++)
		if (syscalls[i])
			num_syscalls ++;

	printk("syscall: there are %d system calls\n", num_syscalls);
	irq_set(0x80, syscall_hub);	
}

