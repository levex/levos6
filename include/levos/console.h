#ifndef __CONSOLE_H
#define __CONSOLE_H

struct console {
	struct device *dev;
	int id; /* To be filled by the subsystem */
};

extern int console_sys_init();

extern int console_send(char c);

#endif /* __CONSOLE_H */
