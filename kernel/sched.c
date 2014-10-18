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
	sched_sync();
	return 0;
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
		return -ENOMEM;

	p->comm = comm;
	p->pid = global_pid ++;
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

struct process *sched_select()
{
	struct process *next;


retry:
	if (sched_next >= PROC_QUEUE_SIZE)
		sched_next = 0;

	next = process_queue[sched_next];
	sched_next ++;
	if (!next)
		goto retry;
	
	return next;
}

void idle_task()
{
	sched_online = 1;
	can_schedule = 1;
	printk("idle: idling...\n");
	while(1) {
		printk("1\n");
	}
}

void idle2_task()
{
	while (1)
		printk("2\n");
}


void sched_start_idle()
{
	printk("sched: starting idle task\n");
	struct process *idle = sched_mk_process("idle", idle_task);
	sched_add_process(idle);
	current = idle;
	struct process *idle2 = sched_mk_process("idle2", idle2_task);
	sched_add_process(idle2);
	arch_sched_setup_stack(current);
	ARCH_SWITCH_CONTEXT();
}

/* FIXME: this shouldn't be here */
extern uint32_t pre_irq_esp;
void sched_schedule()
{
	if (!can_schedule)
		return;
	
	/* alright we can schedule, so let's save the preIRQ registers to the
	 * process
	 */
	if (current) {
		struct pt_regs *irq_regs = GRAB_PRE_IRQ_REGS();
//		DUMP_REGISTERS(irq_regs);
//		DUMP_AREA_AT(pre_irq_esp, 12);
		if (irq_regs->magic != 0x13371337) {
			panic("Wrong IRQ register magic! Stack corrupted!\n");
		}
		memcpy(&current->r, irq_regs, sizeof(struct pt_regs));
//		printk("debug: ESP 0x%x EIP 0x%x\n", STACK_PTR(irq_regs), INS_PTR(irq_regs));
	} else panic("Scheduler has no current process\n");

	/* select next process */
	current = sched_select();
	printk("next process: %d\n", current->pid);
	arch_sched_setup_stack(current);
	ARCH_SWITCH_CONTEXT();
}
