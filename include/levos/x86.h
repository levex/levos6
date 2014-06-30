#ifndef __LEVOS_X86_H
#define __LEVOS_X86_H

int textmode_init();

int x86_serial_init();

struct pt_regs {
	uint32_t eax;
	uint32_t ebx;
	uint32_t ecx;
	uint32_t edx;
	uint32_t esp;
	uint32_t ebp;
};

#define DEF_IRQ_HANDLER(n, name) asm (			\
		#name ":\n"              	  	\
		"	pushal \n" 			\
		"	call __irq_" #name "\n"   	\
		"	movl $" #n ", current_irq \n"	\
		"	call send_eoi\n"		\
		"	popal \n"			\
		"	iretl");            		\
		extern void name();        		\
		void __irq_##name()



#endif /* __LEVOS_X86_H */
