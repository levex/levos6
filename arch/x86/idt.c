#include <levos/levos.h>

struct idt_e {
    uint16_t base_low;
    uint16_t sel;
    uint8_t nil;
    uint8_t flags;
    uint16_t base_high;
} __attribute__((packed));

struct idtr {
    uint16_t limit;
    uintptr_t base;
} __attribute__((packed));

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
}

void idt_noop_irq() {
    asm volatile("iretl");
}

extern void post_schedule_noirq();
int idt_init()
{
    _idtr.limit = (sizeof(struct idt_e) * 256) - 1;
    _idtr.base = (uintptr_t)&idt;

    for (int i = 0; i < 256; i++)
        idt_set_int(i, idt_noop_irq, 0x08, 0x8E);

    idt_set_int(0x2F, post_schedule_noirq, 0x08, 0x8E);

    _idt_load();

    printk("x86: idt: %s done\n", __func__);
    printk("sys: enabling IRQs\n");
    asm volatile("sti");
    printk("sys: IRQs were enabled we seem to be OK\n");
    return 0;
}
