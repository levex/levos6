#ifndef __LEVOS_X86_H
#define __LEVOS_X86_H

int textmode_init();

int x86_serial_init();

struct pt_regs {
    uint32_t magic;
    uint32_t cr3;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
} __attribute__((packed));

struct pt_regs2 {
    uint32_t eflags;
    uint32_t cs;
    uint32_t eip;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t cr3;
    uint32_t magic;
} __attribute__((packed));

#define ARCH_PROCESS_FIELDS page_dir_t *page_dir;

#define DUMP_AREA_AT(x, n)     do {                           \
                    int ___n = (n);                           \
                    int ___x = (x);                           \
                    for (int ___i = 0; ___i < ___n; ___i++)   \
                    printk("<0x%x>: 0x%x\n", ___x + 4 * ___i, \
                    (*(uint32_t *)(___x + 4 * ___i)));        \
                } while (0);


#define THREAD_STACK_SET(p, a) do { (&((p)->r))->esp = a; \
                    (&((p)->r))->ebp = a; } while(0);
#define THREAD_STACK_GET(p) ((p)->r.esp)

extern struct pt_regs __x86_pre_irq_regs;
#define GRAB_PRE_IRQ_REGS()  (&__x86_pre_irq_regs)

#define INS_PTR(r) ((r)->eip)
#define STACK_PTR(r) ((r)->esp)
#define SYSCALL_NUMBER(r) ((r)->eax)
#define SYSCALL_ARG1(r) ((r)->ebx)
#define SYSCALL_ARG2(r) ((r)->ecx)
#define SYSCALL_ARG3(r) ((r)->edx)
#define SYSCALL_ARG4(r) ((r)->esi)
#define SYSCALL_ARG5(r) ((r)->edi)
#define SYSCALL_RETVAL(r) ((r)->eax)

#define DUMP_REGISTERS(r) do {                              \
                printk("\nEAX 0x%x EBX 0x%x ECX 0x%x\n"     \
                       "EDX 0x%x ESP 0x%x EBP 0x%x\n"       \
                       "ESI 0x%x EDI 0x%x MAG 0x%x\n"       \
                       "EFLAGS 0x%x CS 0x%x EIP 0x%x\n",    \
                       r->eax, r->ebx, r->ecx, r->edx,      \
                       r->esp, r->ebp, r->esi, r->edi,      \
                       r->magic, r->eflags, r->cs, r->eip); \
                } while (0);

#define ARCH_SWITCH_CONTEXT() do {                        \
                asm volatile(""                           \
                "mov %%eax, %%esp \n"                     \
                "mov %%ebx, %%cr3 \n"                     \
                "popal" ::                                \
                "a"(current->r.esp),"b"(current->r.cr3)); \
                asm volatile("iretl");                    \
                } while(0);

#define ARCH_VOLUNTEER_SCHEDULE() asm volatile("int $0x2F");

#define ENABLE_IRQ()  asm volatile("sti");
#define DISABLE_IRQ() asm volatile("cli");

#define DEF_IRQ_HANDLER(n, name) asm (            \
        #name ":\n"                               \
        "    mov %esp, pre_pushal_esp \n"         \
        "    pushal \n"                           \
        "    movl %cr3, %eax \n"                  \
        "    pushl %eax \n"                       \
        "    pushl $0x13371337 \n"                \
        "    movl %esp, pre_irq_esp \n"           \
        "    call save_pre_irq_regs \n"           \
        "    movl $" #n ", current_irq \n"        \
        "    call __irq_" #name "\n"              \
        "    call send_eoi\n"                     \
        "    pop %eax \n"                         \
        "    pop %eax \n"                         \
        "    popal \n"                            \
        "    iretl");                             \
        extern void name();                       \
        void __irq_##name()



#endif /* __LEVOS_X86_H */
