#ifndef __LEVOS_LIB_H
#define __LEVOS_LIB_H

#include <levos/levos.h>

/* string */
extern size_t strlen(const uint8_t *s);
extern size_t strcmp(char *s1, char *s2);

/* integer magic */
extern void itoa(unsigned i,unsigned base,char* buf);


#endif /* __LEVOS_LIB_H */
