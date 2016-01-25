#ifndef __SCHED_H
#define __SCHED_H

#include <levos/vfs.h>

struct process {
    char *comm;
    int pid;
    int time_used;
    int state;

    struct file *file_table[128];

    /* Architecture-specific */
    struct pt_regs r;
    /* 
     * This, ladies and gentleman, is the definition of "ugly hack".
     * (IOW, DON'T MODIFY OR LET THE AUTHOR DIE AND/OR LEAVE)
     */
    ARCH_PROCESS_FIELDS;
};

#define PROC_QUEUE_SIZE 32

extern struct process *process_queue[];
extern struct process *current;

#define PROCESS_NULL         0 /* process was never in sched queue */
#define PROCESS_RUNNING      1 /* process is running at the moment */
#define PROCESS_PREEMPTED    2 /* process was preempted and waits for its time */
#define PROCESS_READY        3 /* process is ready to scheduled to for the first time */
#define PROCESS_ZOMBIE       4 /* process was killed and awaits removal from queue */
#define PROCESS_REMOVED      5 /* process is no longer is sched queue */


#define MAX_PROC_TIME 10

extern void schedule_noirq(void);

/* Setup scheduler */
extern int sched_init();
/* Start scheduling and jump to idle task */
extern void sched_start_idle();
/* Add a process to the scheduling queue */
extern int sched_add_process(struct process *p);
/* Create a bare process structure */
extern struct process *sched_mk_process(char *comm, uint32_t entry);

extern int sched_rm_process(struct process *);

extern void sched_schedule();

extern struct process *current;

#endif /* __SCHED_H */
