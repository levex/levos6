#ifndef __LEVOS_MM_H
#define __LEVOS_MM_H

#include <levos/levos.h>

extern int init_paging(uint32_t mem);
extern void paging_fini();

uint32_t mem_used();

extern void mark_region(uint64_t addt);

extern uintptr_t virtual_to_physical(uintptr_t p);

/* frames */
void set_nframes(uint32_t n);
uint32_t get_nframes();
uint32_t first_frame();
void set_frame(uintptr_t frame);
void clear_frame(uintptr_t frame);
int test_frame(uintptr_t frame);


/* alloc */
void *kmalloc(size_t s);
void *kvmalloc(size_t s);
void *kmalloc_p(size_t s, uintptr_t *p);
void *kvmalloc_p(size_t, uintptr_t *p);

#endif
