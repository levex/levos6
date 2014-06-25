#include <levos/levos.h>

extern int kernel_end;
uintptr_t plp = (uintptr_t)&kernel_end;

int heap_online = 0;

void prealloc_start(uintptr_t addr)
{
	plp = addr;
}

/* size, align, phys addr */
void *__kmalloc(size_t s, int a, uintptr_t *p)
{
	if (heap_online) {
		/* if we have a heap, pass request to liballoc */
		uint32_t addr = 0;
		if (!a) {
			addr = new_malloc(s);
		} else {
			panic("liballoc does not support align yet\n");
		}
		if (p)
			*p = virtual_to_physical(addr);
		return addr;
	}
	if (a && (plp & 0xFFFFF000)) {
		plp &= 0xFFFFF000;
		plp += 0x1000;
	}
	if (p)
		*p = plp;

	uintptr_t addr = plp;
	plp += s;
	return (void *)addr;
}

void *kmalloc(size_t s)
{
	return __kmalloc(s, 0, NULL);
}

void *kmalloc_p(size_t s, uintptr_t *p)
{
	return __kmalloc(s, 0, p);
}

void *kvmalloc(size_t s)
{
	return __kmalloc(s, 1, NULL);
}

void *kvmalloc_p(size_t s, uintptr_t *p)
{
	return __kmalloc(s, 1, p);
}

