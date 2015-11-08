#ifndef __CONSOLE_H
#define __CONSOLE_H

#include <levos/vfs.h>

struct console {
	struct device *dev;
	int id; /* To be filled by the subsystem */
};

extern struct file console_file;

extern int console_sys_init();

extern int console_send(char c);

extern int console_register(struct console *c);

#endif /* __CONSOLE_H */
