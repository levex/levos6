#ifndef __LEVOS_TTY_H
#define __LEVOS_TTY_H

extern int tty_init();

struct tty {
	struct device dev;
};

#endif /* __LEVOS_TTY_H */
