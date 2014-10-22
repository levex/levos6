#ifndef __SCHED_H
#define __SCHED_H

struct process {
	char *comm;
	int pid;
	int time_used;
	int state;

	struct device *stdin;
	struct device *stdout;

	/* Architecture-specific */
	struct pt_regs r;
};

#define PROCESS_NULL 		0 /* process was never in sched queue */
#define PROCESS_RUNNING 	1 /* process is running at the moment */
#define PROCESS_PREEMPTED 	2 /* process was preempted and waits for its time */
#define PROCESS_READY 		3 /* process is ready to scheduled to for the first time */
#define PROCESS_ZOMBIE 		4 /* process was killed and awaits removal from queue */
#define PROCESS_REMOVED 	5 /* process is no longer is sched queue */


#define MAX_PROC_TIME 10

/* Setup scheduler */
extern int sched_init();
/* Start scheduling and jump to idle task */
extern void sched_start_idle();
/* Add a process to the scheduling queue */
extern int sched_add_process(struct process *p);
/* Create a bare process structure */
extern struct process *sched_mk_process(char *comm, uint32_t entry);

extern void sched_schedule();

#endif /* __SCHED_H */
