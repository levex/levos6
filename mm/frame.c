#include <levos/levos.h>

#define INDEX_BIT(x) (x / 32)
#define OFFSET_BIT(x) (x % 32)

static uint32_t nframes = 0;

static uint32_t *frames = 0;

page_dir_t *kernel_dir = 0;

void set_nframes(uint32_t n)
{
	nframes = n;
	if (frames)
		panic("reallocing frames\n");
	
	frames = (uint32_t *)kmalloc(INDEX_BIT(nframes * 8));
	memset(frames, 0, INDEX_BIT(nframes));

	uintptr_t p;
	kernel_dir = kvmalloc_p(sizeof(page_dir_t), &p);
	printk("mm: frame: kernel_dir at 0x%x\n", kernel_dir);
	memset(kernel_dir, 0, sizeof(page_dir_t));
}

uint32_t get_nframes()
{
	return nframes;
}

void set_frame(uintptr_t a)
{
	int f = a / 0x1000;
	int i = INDEX_BIT(f);
	int o = OFFSET_BIT(f);
	frames[i] |= (1 << o);
}

void clear_frame(uintptr_t a)
{
	int f = a / 0x1000;
	int i = INDEX_BIT(f);
	int o = OFFSET_BIT(f);

	frames[i] &= ~(1 << o);
}

int test_frame(uintptr_t a)
{
	int f = a / 0x1000;
	int i = INDEX_BIT(f);
	int o = OFFSET_BIT(f);

	return (frames[i] & (1 << o));
}

uint32_t first_n_frames(int n)
{
	for (int i = 0; i < nframes * 0x1000; i += 0x1000) {
		int b = 0;
		for (int j = 0; j < n; j++) {
			if (test_frame(i + 0x1000 * j))
				b = j + 1;
		}
		if (!b)
			return i / 0x1000;
	}
	return 0xFFFFFFFF;
}

extern page_t *get_page(uintptr_t addr, int make, page_dir_t *d);
uintptr_t mm_alloc_pages(int n)
{
	uint32_t addr = first_n_frames(n) * 0x1000;
	for (uint32_t i = addr; i < addr + n * 0x1000; i += 0x1000) {
		if (test_frame(i))
			panic("first_n_frames is broken\n");
		alloc_dma_frame(get_page(i, 1, kernel_dir), 1, 0, i);
	}

	return addr;
}

uintptr_t mm_free_pages(void *p, int n)
{
	while(n--)
		free_frame(p -= 0x1000);
}

uint32_t first_frame() {
	int i, j;

	for (i = 0; i < INDEX_BIT(nframes); i++) {
		if (frames[i] != 0xFFFFFFFF) {
			for (j = 0; j < 32; j++) {
				uint32_t test = 1 << j;
				if (!(frames[i] & test))
					return i * 0x20 + j;
			}
		}
	}

	panic("Out of memory");

	return 0xFFFFFFFF;
}

uint32_t mem_used()
{
	uint32_t ret = 0;
	int i, j;
	for (i = 0; i < INDEX_BIT(nframes); i++)
	{
		for (j = 0; j < 32; j++) {
			int t = 1 << j;
			if (frames[i] & t)
				ret ++;
		}
	}
	return ret * 4;
}
