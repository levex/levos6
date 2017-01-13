#ifndef __SCHED_H
#define __SCHED_H

#include <levos/vfs.h>

struct thread;

struct process {
    char *comm;
    int pid;
    struct thread *main_thread;
    int no_threads;

    struct file *file_table[128];
    /* 
     * This, ladies and gentleman, is the definition of "ugly hack".
     * (IOW, DON'T MODIFY OR LET THE AUTHOR DIE AND/OR LEAVE)
     */
    ARCH_PROCESS_FIELDS;
};

struct thread {
    int tid;
    int state;
    int time_used;

    struct process *owner;

    /* Architecture-specific */
    struct pt_regs r;
};

#define THREAD_QUEUE_SIZE 32

extern struct thread *thread_queue[];
struct process *current_process;
struct thread *current;

#define THREAD_NULL         0 /* thread was never in sched queue */
#define THREAD_RUNNING      1 /* thread is running at the moment */
#define THREAD_PREEMPTED    2 /* thread was preempted and waits for its time */
#define THREAD_READY        3 /* thread is ready to scheduled to for the first time */
#define THREAD_ZOMBIE       4 /* thread was killed and awaits removal from queue */
#define THREAD_REMOVED      5 /* thread is no longer is sched queue */


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

#endif /* __SCHED_H */
