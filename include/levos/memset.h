#ifndef __LEVOS__MEMSET_H
#define __LEVOS__MEMSET_H

#include <levos/levos.h>

void *memset(void *dst, char d, size_t t);
void *memset16(void *dst, uint16_t d, size_t t);
void memcpy(void *, void *, size_t);

#endif /* __LEVOS__MEMSET_H */
