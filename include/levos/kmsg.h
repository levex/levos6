#ifndef __LEVOS_KMSG_H
#define __LEVOS_KMSG_H

extern int kmsg_init();

extern void printk(char *fmt, ...);
extern void panic(char *fmt, ...);

struct console kmsg_con;

#endif /* __LEVOS_KMSG_H */
