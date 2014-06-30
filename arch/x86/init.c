#include <levos/levos.h>


extern int gdt_init();
extern int idt_init();
extern int irq_init();

int arch_early_init()
{
	int rc;

	rc = x86_serial_init();
	if (rc)
		return rc;

	rc = textmode_init();
	if (rc)
		return rc;

	rc = gdt_init();
	if (rc)
		return rc;

	rc = idt_init();
	if (rc)
		return rc;

	rc = exceptions_init();
	if (rc)
		return rc;

	rc = irq_init();
	if (rc)
		return rc;

	rc = pit_init();
	if (rc)
		return rc;

	return 0;
}

int arch_late_init()
{
	return 0;
}
