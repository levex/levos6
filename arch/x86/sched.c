#include <levos/levos.h>

/* This is the struct grabbed by GRAB_PRE_IRQ_REGS() */
struct pt_regs __x86_pre_irq_regs;
extern uint32_t pre_irq_esp;

uint32_t pre_pushal_esp = 0;

void save_pre_irq_regs()
{
	memcpy(&__x86_pre_irq_regs,
		pre_irq_esp,
		sizeof(struct pt_regs));
	__x86_pre_irq_regs.esp = pre_pushal_esp + 12;
	
	if (__x86_pre_irq_regs.magic != 0x13371337)
		panic("Wrong IRQ register magic! Stack corrupted!\n");
	if (current)
		memcpy(&current->r, &__x86_pre_irq_regs, sizeof(struct pt_regs));
}

void arch_sched_setup_stack(struct process *p)
{
	uint32_t *s = p->r.esp;
	*(--s) = p->r.eflags;
	*(--s) = p->r.cs;
	*(--s) = p->r.eip;
	uint32_t tmp = s;
	*(--s) = p->r.eax;
	*(--s) = p->r.ecx;
	*(--s) = p->r.edx;
	*(--s) = p->r.ebx;	
	*(--s) = tmp;
	*(--s) = p->r.ebp;
	*(--s) = p->r.esi;
	*(--s) = p->r.edi;
	p->r.esp = s;
}

int arch_sched_mk_initial_regs(struct pt_regs *r)
{
	memset(r, 0, sizeof(*r));
	r->cs = 0x8;
	r->eflags = 0x202;

	asm volatile("mov %%cr3, %%eax" :"=a"(r->cr3));
	return 0;
}
