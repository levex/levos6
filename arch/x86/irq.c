#include <levos/levos.h>

extern void idt_set_int(int i, void(*h)(void), int b, int c);
void irq_set(size_t i, void(*h)(void)) {
	idt_set_int(i, h, 0x08, 0x8E);
}

void irq_remap()
{
	outportb(0x20, 0x11);
	outportb(0xA0, 0x11);
	outportb(0x21, 0x20);
	outportb(0xA1, 0x28);
	outportb(0x21, 0x04);
	outportb(0xA1, 0x02);
	outportb(0x21, 0x01);
	outportb(0xA1, 0x01);
	outportb(0x21, 0x0);
	outportb(0xA1, 0x0);
}

int irq_init()
{
	irq_remap();	

	return 0;
}
