#include <levos/levos.h>

void exc_divide_by_zero()
{
	panic("Divide by zero!\n");
	for(;;);
}

void exc_debug()
{
	panic("Debug!\n");
	for(;;);
}

void exc_nmi()
{
	panic("NMI\n");
	for(;;);
	return;
}

void exc_brp()
{
	panic("Breakpoint!\n");
	return;
}

void exc_overflow()
{
	panic("Overflow!\n");
	for(;;);
}

void exc_bound()
{
	panic("Bound range exceeded.\n");
	for(;;);
}

void exc_invopcode()
{
	panic("Invalid opcode.\n");
	for(;;);
}

void exc_device_not_avail()
{
	panic("Device not available.\n");
	for(;;);
}

void exc_double_fault()
{
	panic("Double fault, halting.\n");
	for(;;);
}

void exc_coproc()
{
	panic("Coprocessor fault, halting.\n");
	for(;;);
	return;
}

void exc_invtss()
{
	panic("TSS invalid.\n");
	for(;;);
	return;
}

void exc_segment_not_present()
{
	panic("Segment not present.\n");
	for(;;);
	return;
}

void exc_ssf()
{
	panic("Stacksegment faulted.\n");
	for(;;);
	return;
}

void exc_gpf()
{
	panic("General protection fault");
	return;
}

void exc_pf(int errc)
{
	int resolved = 0;
	char *comm = "kernel";
	
	printk("------[ Page fault ]------\n");
	uint32_t cr2 = 0;
	asm volatile("mov %%cr0, %%eax":"=a"(cr2));
	printk("Process: %s\n", comm);
	printk("Linear address: 0x%x\n", cr2);
	printk("Error code: 0x%x\n", errc);
	printk("------[ End  Trace ]------\n");
	if (strcmp(comm, "kernel") == 0)
		resolved = 0;
	
	if (!resolved)
		panic("Unable to handle kernel paging request\n");
}

void exc_reserved()
{
	panic("This shouldn't happen. Halted.\n");
	for(;;);
}

int exceptions_init()
{
	printk("x86: exceptions being loaded\n");
	irq_set(0, exc_divide_by_zero);
	irq_set(1, exc_debug);
	irq_set(2, exc_nmi);
	irq_set(3, exc_brp);
	irq_set(4, exc_overflow);
	irq_set(5, exc_bound);
	irq_set(6, exc_invopcode);
	irq_set(7, exc_device_not_avail);
	irq_set(8, exc_double_fault);
	irq_set(9, exc_coproc);
	irq_set(10, exc_invtss);
	irq_set(11, exc_segment_not_present);
	irq_set(12, exc_ssf);
	irq_set(13, exc_gpf);
	irq_set(14, exc_pf);
	irq_set(15, exc_reserved);
	return 0;
}
