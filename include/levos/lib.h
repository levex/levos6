#ifndef __LEVOS_LIB_H
#define __LEVOS_LIB_H

#include <levos/levos.h>

/* string */
extern size_t strlen(const uint8_t *s);
extern size_t strcmp(char *s1, char *s2);
extern char *strtok(char *, const char *);
extern size_t strncmp(char *, char *, size_t);

/* integer magic */
extern void itoa(unsigned i,unsigned base,char* buf);

extern int call_syscall(int, int, int, int);

extern void printk_switch_tty(int ctty);


#endif /* __LEVOS_LIB_H */
