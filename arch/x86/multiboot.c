#include <levos/levos.h>

void arch_do_mboot(struct multiboot *m)
{
	/* parse mmap but first let me take a selfie with paging */
	init_paging(m->mem_upper + m->mem_lower);

	if (m->flags & (1 << 6)) {
		printk("x86: multiboot: parsing memory map...\n");
		mboot_memmap_t *mmap = (void *)m->mmap_addr;
		while((uint32_t)mmap < m->mmap_addr + m->mmap_length) {
			if (mmap->type == 2) {
				for (uint64_t i = 0; i < mmap->length; i += 0x1000) {
					if (mmap->base_addr + i > 0xFFFFFFFF);
					mark_region((mmap->base_addr + i) & 0xFFFFF000);
				}
			}
			mmap = (mboot_memmap_t *) ((uintptr_t)mmap + mmap->size + sizeof(uintptr_t));
		}
	}	
	printk("x86: multiboot: parse complete\n");
}
