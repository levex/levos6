#ifndef __LEVOS_INCLUDE_ARM_H
#define __LEVOS_INCLUDE_ARM_H

struct pt_regs {
	char regs[32];
} __attribute__((packed));

#define ARCH_PROCESS_FIELDS ;

#define DUMP_ARE_AT(x, n) ;

#define PROCESS_STACK_SET(p, a) panic("Cant do PROCESS_STACK_SET yet");

#define PROCESS_STACK_GET(p) panic("Unimplemented PROCESS_STACK_GET");

#define GRAB_PRE_IRQ_REGS() panic("FATAL ERROR");

#define INS_PTR(r) 0
#define STACK_PTR(r) 0
#define SYSCALL_NUMBER(r) 0
#define SYSCALL_ARG1(r) 0
#define SYSCALL_ARG2(r) 0
#define SYSCALL_ARG3(r) 0
#define SYSCALL_ARG4(r) 0
#define SYSCALL_ARG5(r) 0
#define SYSCALL_RETVAL(r) 0

#define DUMP_REGISTERS(r) ;

#define ARCH_SWITCH_CONTEXT() panic("cant context switch yet");

#define ARCH_VOLUNTEER_SCHEDULE() panic("cant volunteer schedule yet");

#define ENABLE_IRQ() panic("enabling IRQs would crash ARM");

#define DISABLE_IRQ() ;

#define DEF_IRQ_HANDLER(n, name) void name()

#endif
