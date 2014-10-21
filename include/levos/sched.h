#ifndef __SCHED_H
#define __SCHED_H

struct process {
	char *comm;
	int pid;
	int time_used;
	/* Architecture-specific */
	struct pt_regs r;
};

#define MAX_PROC_TIME 10

/* Setup scheduler */
extern int sched_init();
/* Start scheduling and jump to idle task */
extern void sched_start_idle();
/* Add a process to the scheduling queue */
extern int sched_add_process(struct process *p);
/* Create a bare process structure */
extern struct process *sched_mk_process(char *comm, uint32_t entry);

#endif /* __SCHED_H */
