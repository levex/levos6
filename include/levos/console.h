#ifndef __CONSOLE_H
#define __CONSOLE_H

struct console {
	struct device *dev;
	int id; /* To be filled by the subsystem */
};

#endif /* __CONSOLE_H */
