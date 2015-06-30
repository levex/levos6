#include <levos/levos.h>

int num_syscalls = 0;

int syscall_null(struct pt_regs *r)
{
	SYSCALL_RETVAL(r) = -ENOSYS;
	return 0;
}

int syscall_exit(struct pt_regs *r)
{
	printk("pid %d: Bye cruel world!\n", current->pid);
	sched_rm_process(current);
	SYSCALL_RETVAL(r) = 0x13371337;
	return 0;
}

void *syscalls[256] = {
	0,
	syscall_exit,
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
	f(r);
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

