#include <levos/device.h>
#include <errno.h>

struct tty ttys[TTY_COUNT];
int current_tty = -1;

void tty_set_current(int n)
{
	if (current_tty == -1)
		printk("boot: current tty will be tty%d\n", n);
	current_tty = n;
	return 0;
}

int tty_init() {
	printk("tty: there are %d TTYs\n", TTY_COUNT);

	tty_set_current(0);

	return 0;
}

