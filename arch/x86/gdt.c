#include <levos/levos.h>

struct gdt_e {
	uint16_t limit_low;
	uint16_t base_low;
	uint8_t  base_mid;
	uint8_t  access;
	uint8_t  gran;
	uint8_t  base_high;
} __attribute__((packed));

struct gdtr {
	uint16_t limit;
	uint32_t base;
} __attribute__((packed));

struct gdt_e gdt[6];
struct gdtr gdtr_t;

extern void _set_gdtr();
extern void _reload_segments();

void gdt_set_desc(size_t num, uint32_t base, uint32_t limit,
		  uint8_t ac, uint8_t gran)
{
	gdt[num].base_low = (base & 0xFFFF);
	gdt[num].base_mid = (base >> 16) & 0xFF;
	gdt[num].base_high = (base >> 24) & 0xFF;

	gdt[num].limit_low = (limit & 0xFFFF);
	gdt[num].gran = (limit >> 16) & 0x0F;
	gdt[num].gran |= (gran & 0xF0);

	gdt[num].access = ac;

	printk("x86: gdt: write desc %d\n", num);
}

int gdt_init()
{
	memset(&gdtr_t, 0, sizeof(struct gdtr));
	memset(&gdt, 0, sizeof(struct gdt_e) * 6);
	
	gdtr_t.limit = (sizeof(struct gdt_e) * 6) - 1;
	gdtr_t.base = (uint32_t)&gdt;

	gdt_set_desc(0, 0, 0, 0, 0);
	gdt_set_desc(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
	gdt_set_desc(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

	_set_gdtr();
	printk("x86: gdt: gdtr was set, reloading segments\n");
	_reload_segments();
	printk("x86: %s done\n", __func__);

	return 0;
}
