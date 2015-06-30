#include <levos/levos.h>

#define PROC_QUEUE_SIZE 32

struct process *process_queue[PROC_QUEUE_SIZE];

int sched_next = 0;

int sched_current_processes = 0;

int can_schedule = 0;
int sched_online = 0;

int global_pid = 0;

struct process *current = 0;

int sched_init()
{
	printk("sched: beta version\n");
	
	memset(process_queue, 0, sizeof(struct process *) * PROC_QUEUE_SIZE);
	sched_current_processes = 0;
	current = 0;
	return 0;
}

void sched_desync()
{
}

void sched_sync()
{
}

int sched_add_process(struct process *p)
{
	if (!p)
		return -EINVAL;
	
	if (sched_current_processes >= PROC_QUEUE_SIZE)
		return -ENOSPC;

	sched_desync();
	int i;
	for (i = 0; i < PROC_QUEUE_SIZE; i++) {
		if (!process_queue[i]) {
			process_queue[i] = p;
			break;
		}
	}
	sched_current_processes ++;
	printk("sched: process \"%s\" added as PID %d (proc_queue_pos=%d)\n",
		p->comm, p->pid, i);
	p->state = PROCESS_READY;
	sched_sync();
	return 0;
}

int sched_mark_zombie(struct process *p)
{
	p->state = PROCESS_ZOMBIE;
	return 0;
}

int sched_rm_process(struct process *p)
{
	int success = 0;

	if (!p)
		return -EINVAL;
	
	if (p == current) {
		if (current->pid == 1)
			panic("Attempted to kill init process!\n");
		sched_mark_zombie(current);
		schedule_noirq();
	}

	for (int i = 0; i < PROC_QUEUE_SIZE; i++) {
		if (process_queue[i]->pid == p->pid) {
			process_queue[i] = 0;
			success = 1;
			break;
		}
	}

	if (!success)
		return -EINVAL;

	sched_current_processes --;

	return sched_destroy_process(p);
}

int sched_create_stack(struct process *p)
{
	if (!p)
		return -EINVAL;

	/* grab a new page in userspace */
	uint32_t page_addr = mm_alloc_pages(1);
	if (!page_addr)
		return -ENOMEM;

	memset(page_addr, 0, 4096);
	uint32_t *s = (uint32_t *)(page_addr + 4096);

	PROCESS_STACK_SET(p, s);

	return 0;
}

struct process *sched_mk_process(char *comm, uint32_t entry)
{
	if (!comm || !entry)
		return 0;

	struct process *p = kmalloc(sizeof(*p));
	if (!p)
		return 0;

	p->comm = comm;
	p->time_used = 0;
	p->pid = global_pid ++;
	p->state = PROCESS_NULL;
	arch_sched_mk_initial_regs(&p->r);
	INS_PTR(&p->r) = entry;
	int r = sched_create_stack(p);
	if (r) {
		new_free(p);
		return 0;
	}

	printk("sched: newproc: IP: 0x%x SP: 0x%x\n", INS_PTR(&p->r), STACK_PTR(&p->r));

	return p;
}

struct process *get_process_by_pid(int pid)
{
	for (int i = 0; i < PROC_QUEUE_SIZE; i++) {
		if (!process_queue[i])
			continue;
		if (process_queue[i]->pid == pid)
			return process_queue[i];
	}
	return 0;
}

int sched_destroy_pid(int pid)
{
	struct process *p = get_process_by_pid(pid);
	if (!p)
		return -EINVAL;

	/* a running process cannot be destroyed */
	if (p->state == PROCESS_RUNNING)
		return -EAGAIN;

	return sched_destroy_process(p);
}

int sched_destroy_process(struct process *p)
{
	/* p->comm is not free'd, since we doN't know if
	 * it was actually allocated by liballoc or is it just
	 * a plain .data string...
	 */
//	mm_free_pages(PROCESS_STACK_GET(p), 1);
//	new_free(p);
	/* FIXME */
	return 0;
}

struct process *sched_select()
{
	struct process *next;


retry:
	if (sched_next >= PROC_QUEUE_SIZE)
		sched_next = 0;

	next = process_queue[sched_next];
	sched_next ++;
	if (!next || next->state == PROCESS_ZOMBIE
		/* only to schedule to idle if there are no processes */
		  || (next->pid == 0 && sched_current_processes > 1))
		goto retry;
	
	return next;
}

void idle_task()
{
	sched_online = 1;
	can_schedule = 1;
	printk("idle: idling...\n");
	ENABLE_IRQ();
	while(1)
		schedule_noirq();
}

void kinit_task()
{
	int ret;
	printk("kinit: setting up userspace\n");
	asm volatile("mov $0x00, %%eax\n"
			"int $0x80\n"
			"mov %%eax, %0":"=r"(ret)::"eax");
	if (ret == -ENOSYS)
		printk("kinit: system calls are working!\n");
	else panic("System calls are broken!\n");

	ret = main_init();
	if (ret)
		panic("failed to find a suitable init file (0x%x)\n", ret);

	panic("well, that's messed up\n");

	while(1) schedule_noirq();
}

void schedule_noirq()
{
	current->time_used = MAX_PROC_TIME;
	ARCH_VOLUNTEER_SCHEDULE();
}

DEF_IRQ_HANDLER(0x2F, __post_schedule_noirq)
{
	sched_schedule();
}

void post_schedule_noirq()
{
	__post_schedule_noirq();
}

void sched_start_idle()
{
	printk("sched: starting idle task\n");
	DISABLE_IRQ();
	struct process *idle = sched_mk_process("idle", idle_task);
	sched_add_process(idle);
	current = idle;
	struct process *kinit = sched_mk_process("init", kinit_task);
	sched_add_process(kinit);
	arch_sched_setup_stack(current);
	ARCH_SWITCH_CONTEXT();
	panic("sched: couldn't start idle task\n");
}

/* FIXME: this shouldn't be here */
extern uint32_t pre_irq_esp;
void sched_schedule()
{
	struct process *prev = 0;	
	
	if (!can_schedule)
		return;
	
	/* alright we can schedule, so let's save the preIRQ registers to the
	 * process
	 */
	if (current) {
		struct pt_regs *irq_regs = GRAB_PRE_IRQ_REGS();

		if (current->state == PROCESS_ZOMBIE) {
			prev = current;
			goto next;
		}

		current->time_used ++;
		if (current->time_used < MAX_PROC_TIME)
			return;
		
		current->state = PROCESS_PREEMPTED;
	} else panic("Scheduler has no current process\n");

	/* select next process */
next:	
	if (prev)
		sched_destroy_process(prev);
	current = sched_select();
	current->time_used = 0;
	current->state = PROCESS_RUNNING;
	arch_sched_setup_stack(current);
	ARCH_SWITCH_CONTEXT();
}
