#include <levos/levos.h>

extern page_dir_t *kernel_dir;
extern uintptr_t plp;

page_dir_t *current_directory;

#define KHEAP_START 0x00800000
#define KHEAP_END   0x20000000

int init_paging(uint32_t mem)
{
	printk("mm: available memory: %d KB\n", mem);
	set_nframes(mem / 4);
	return 0;
}

uint32_t mem_total()
{
	return get_nframes() * 4;
}

page_dir_t *copy_page_dir(page_dir_t *src)
{
	page_dir_t *dst = kmalloc(sizeof(*dst));
	if (!dst)
		return 0;

	memcpy(dst, src, sizeof(*dst));
	return dst;
}

void alloc_frame(page_t *p, int k, int w)
{
	if (p->frame != 0) {
		p->present = 1;
		p->rw = (w)?1:0;
		p->user  = (k)?0:1;
		return;
	} else {
		int index = first_frame();
		set_frame(index * 0x1000);
		p->frame = index;
		alloc_frame(p, k, w);
	}
	return;
}

void alloc_dma_frame(page_t *p, int k, int w, uintptr_t a)
{
	/* DMA is a nasty pig, leave it as it is */
	p->present = 1;
	p->rw = (w)?1:0;
	p->user = (k)?0:1;
	p->frame = a / 0x1000;
	if (a < get_nframes() * 4 * 0x400)
		set_frame(a);
}

void free_frame(page_t *p)
{
	if (!p->frame)
		panic("Page at 0x%x attempted to free frame 0x%x\n", p, p->frame);
	
	clear_frame(p->frame * 0x1000);
	p->frame = 0;
}

page_t *get_page(uintptr_t addr, int make, page_dir_t *dir)
{
	addr /= 0x1000;
	uint32_t tindex = addr / 1024;
	if (dir->tables[tindex])
		return &dir->tables[tindex]->pages[addr % 1024];
	else if (make) {
		/* okay we should create it */
		uint32_t tmp;
		dir->tables[tindex] = (page_table_t *)kvmalloc_p(
						sizeof(page_table_t),
						(uintptr_t *)(&tmp));
		memset(dir->tables[tindex], 0, sizeof(page_table_t));
		dir->phy_tables[tindex] = tmp | 0x7;
		return &dir->tables[tindex]->pages[addr % 1024];
	} else
		return 0;
}

void page_fault() {
	panic("Page fault!\n");
}

uintptr_t virtual_to_physical(uintptr_t a)
{
	uintptr_t rem = a % 0x1000;
	uintptr_t f = a / 0x1000;
	uintptr_t t = f / 1024;
	uintptr_t i = f % 1024;

	if (current_directory->tables[t]) {
		page_t *p = &current_directory->tables[t]->pages[i];
		return p->frame * 0x1000 + rem;
	} else return 0;
}

void paging_fini()
{
	printk("mm: mmap reserved memory: %d KB\n", mem_used());
	get_page(0, 1, kernel_dir)->present = 0;
	set_frame(0);
	for (uintptr_t i = 0x1000; i < 0x80000; i += 0x1000) {
		alloc_dma_frame(get_page(i, 1, kernel_dir), 1, 0, i);
	}
	for (uintptr_t i = 0x80000; i < 0x100000; i += 0x1000) {
		alloc_dma_frame(get_page(i, 1, kernel_dir), 1, 0, i);
	}
	for (uintptr_t i = 0x100000; i < plp + 0x3000; i += 0x1000) {
		alloc_dma_frame(get_page(i, 1, kernel_dir), 1, 0, i);
	}
	/* vga */
	for (uintptr_t i = 0xB8000; i < 0xC0000; i += 0x1000) {
		alloc_dma_frame(get_page(i, 0, kernel_dir), 0, 1, i);
	}
	kernel_dir->phy_addr = (uint32_t)kernel_dir->phy_tables;

	uintptr_t hs = KHEAP_START;

	if (hs < plp + 0x3000)
		hs = plp + 0x100000;

	/* map map map! */
	for (uintptr_t i = plp + 0x3000; i < hs; i += 0x1000)
		alloc_frame(get_page(i, 1, kernel_dir), 1, 0);
	
	for (uintptr_t i = hs; i < KHEAP_END; i += 0x1000)
		get_page(i, 1, kernel_dir);

	printk("mm: page directory setup.\n");
	switch_page_dir(kernel_dir);
	
	printk("mm: total reserved memory: %d KB\n", mem_used());
	return;
}

void mark_region(uint64_t addr)
{
	set_frame(addr);
}
