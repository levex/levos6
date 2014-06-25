#include <levos/levos.h>

struct idt_e {
	uint16_t base_low;
	uint16_t sel;
	uint8_t nil;
	uint8_t flags;
	uint16_t base_high;
};

struct idtr {
	uint16_t limit;
	uintptr_t base;
};

static struct idt_e idt[256];
struct idtr _idtr;

extern void _idt_load(void);

void idt_set_int(uint8_t n, void(*base)(void), uint16_t sel, uint8_t flags)
{
	idt[n].base_low = ((uintptr_t)base & 0xFFFF);
	idt[n].base_high = ((uintptr_t)base >> 16) & 0xFFFF;
	idt[n].sel = sel;
	idt[n].nil = 0;
	idt[n].flags = flags | 0x60;

	printk("x86: idt: set desc %d\n", n);
}

int idt_init()
{
	_idtr.limit = (sizeof(struct idt_e) * 256) - 1;
	_idtr.base = (uintptr_t)&idt;

	memset(&idt, 0, sizeof(struct idt_e) * 256);
	_idt_load();

	printk("x86: idt: %s done\n", __func__);
	
	return 0;
}
