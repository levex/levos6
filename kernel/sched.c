#include <levos/levos.h>

struct thread *thread_queue[THREAD_QUEUE_SIZE];

int sched_next = 0;

int sched_current_processes = 0;
int sched_current_threads = 0;

int can_schedule = 0;
int sched_online = 0;

int global_pid = 0;

struct thread *current = 0;

int sched_init()
{
    printk("sched: beta version\n");

    memset(thread_queue, 0, sizeof(struct thread *) * THREAD_QUEUE_SIZE);
    sched_current_threads = 0;
    current = 0;
    return 0;
}

void sched_sync()
{
    /* mutex_lock(&sched_lock); */
}

void sched_desync()
{
    /* mutex_unlock(&sched_lock); */
}


int sched_add_process(struct process *p)
{
    if (!p)
        return -EINVAL;

    if (sched_current_threads + p->no_threads >= THREAD_QUEUE_SIZE)
        return -ENOSPC;

    sched_desync();
    int i;
    for (int j = 0; j < p->no_threads; j ++)
        for (i = 0; i < THREAD_QUEUE_SIZE; i++) {
            if (!thread_queue[i]) {
                thread_queue[i] = p->threads[j];
                break;
            }
        }
    sched_current_processes ++;
    sched_current_threads += p->no_threads;
    printk("sched: process \"%s\" added as PID %d (proc_queue_pos=%d)\n",
        p->comm, p->pid, i);
    for (int j = 0; j < p->no_threads; j ++)
        p->threads[j]->state = THREAD_READY;
    sched_sync();
    return 0;
}

int sched_mark_zombie(struct process *p)
{
    for (int i = 0; i < p->no_threads; i ++)
        p->threads[i]->state = THREAD_ZOMBIE;
    return 0;
}

int sched_destroy_process(struct process *p)
{
    /* p->comm is not free'd, since we doN't know if
     * it was actually allocated by liballoc or is it just
     * a plain .data string...
     */
//    mm_free_pages(PROCESS_STACK_GET(p), 1);
//    new_free(p);
    /* FIXME */
    return 0;
}

int sched_rm_process(struct process *p)
{
    int success = 0;

    if (!p)
        return -EINVAL;

    if (p->pid == current->owner->pid) {
        if (current->owner->pid == 1)
            panic("Attempted to kill init process!\n");
        sched_mark_zombie(current->owner);
        schedule_noirq();
    }

    for (int i = 0; i < THREAD_QUEUE_SIZE; i++) {
        if (thread_queue[i]->owner->pid == p->pid) {
            thread_queue[i] = 0;
            success ++;
        }
    }

    if (!success)
        return -EINVAL;

    sched_current_processes --;
    sched_current_threads -= success;

    return sched_destroy_process(p);
}

int sched_create_stack(struct thread *t)
{
    if (!t)
        return -EINVAL;

    /* grab a new page in userspace */
    uint32_t page_addr = mm_alloc_pages(1);
    if (!page_addr)
        return -ENOMEM;

    memset((void *) page_addr, 0, 4096);
    uint32_t *s = (uint32_t *)(page_addr + 4096);

    THREAD_STACK_SET(t, (uintptr_t) s);

    return 0;
}

void sched_make_filetable(struct process *p)
{
    struct file **ft = p->file_table;

    /* fd 0 is stdin */
    /* FIXME */
    ft[0] = 0;

    /* fd 1 is stdout, this will be kmsg atm */
    ft[1] = &console_file;

}

struct process *sched_mk_process(char *comm, uint32_t entry)
{
    struct thread *t, **threads;
    struct process *p;

    if (!comm || !entry)
        return 0;

    p = kmalloc(sizeof(*p));
    if (!p)
        return 0;

    t = kmalloc(sizeof(*t));
    if (!t)
        return 0;

    threads = kmalloc(sizeof(struct thread *) * 1);
    if (!threads)
        return 0;

    p->comm = comm;
    p->pid = global_pid ++;
    p->no_threads = 1;
    p->threads = threads;
    p->threads[0] = t;
    p->threads[0]->owner = p;
    p->threads[0]->time_used = 0;
    p->threads[0]->state = THREAD_NULL;
    arch_sched_mk_initial_regs(&p->threads[0]->r);
    arch_sched_make_address_space(p);
    INS_PTR(&p->threads[0]->r) = entry;
    int r = sched_create_stack(p->threads[0]);
    if (r) {
        new_free(p);
        return 0;
    }
    sched_make_filetable(p);
    printk("sched: newproc: IP: 0x%x SP: 0x%x\n",
                INS_PTR(&p->threads[0]->r), STACK_PTR(&p->threads[0]->r));

    return p;
}

struct process *get_process_by_pid(int pid)
{
    for (int i = 0; i < THREAD_QUEUE_SIZE; i++) {
        if (!thread_queue[i])
            continue;
        if (thread_queue[i]->owner->pid == pid)
            return thread_queue[i]->owner;
    }
    return 0;
}

int sched_destroy_pid(int pid)
{
    struct process *p = get_process_by_pid(pid);
    if (!p)
        return -EINVAL;

    /* a running process cannot be destroyed */
    for (int i = 0; i < p->no_threads; i ++)
        if (p->threads[i]->state == THREAD_RUNNING)
            return -EAGAIN;

    return sched_destroy_process(p);
}

struct thread *sched_select()
{
    struct thread *next;

retry:
    if (sched_next >= THREAD_QUEUE_SIZE)
        sched_next = 0;

    next = thread_queue[sched_next];
    sched_next ++;
    if (!next || next->state == THREAD_ZOMBIE 
        /* only to schedule to idle if there are no processes */
          || (next->owner->pid == 0 && sched_current_threads > 1))
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

extern int main_init(void);
void kinit_task()
{
    int ret;
    printk("kinit: setting up userspace\n");

    ret = call_syscall(0, 0, 0, 0);
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
    struct process *idle = sched_mk_process("idle", (uintptr_t) idle_task);
    sched_add_process(idle);
    current = idle->threads[0];

    struct process *kinit = sched_mk_process("init", (uintptr_t) kinit_task);
    sched_add_process(kinit);

    arch_sched_setup_stack(current);
    ARCH_SWITCH_CONTEXT();
    panic("sched: couldn't start idle task\n");
}

/* FIXME: this shouldn't be here */
extern uint32_t pre_irq_esp;
void sched_schedule()
{
    struct thread *prev = 0;

    if (!can_schedule)
        return;

    /* alright we can schedule, so let's save the preIRQ registers to the
     * process
     */
    if (current) {
        struct pt_regs *irq_regs = GRAB_PRE_IRQ_REGS();

        if (current->state == THREAD_ZOMBIE) {
            prev = current;
            goto next;
        }

        current->time_used ++;
        if (current->time_used < MAX_PROC_TIME)
            return;

        current->state = THREAD_PREEMPTED;
    } else panic("Scheduler has no current process\n");

    /* select next process */
next:
    if (prev)
        sched_destroy_process(prev->owner);
    current = sched_select();
    current->time_used = 0;
    current->state = THREAD_RUNNING;
    arch_sched_setup_stack(current);
    arch_sched_setup_address_space(current->owner);
    arch_prep_exit_irq();
    ARCH_SWITCH_CONTEXT();
}
