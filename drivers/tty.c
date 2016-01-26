#include <levos/device.h>
#include <errno.h>

static struct tty ttys[TTY_COUNT];
static int current_tty = -1;

static struct device *default_input = 0;

void tty_set_default_output(struct device *d)
{
    default_input = d;
}

void tty_set_current(int n)
{
    if (current_tty == -1)
        printk("boot: current tty will be tty%d\n", n);

    current_tty = n;
}

struct tty *get_current_tty()
{
    if (current_tty == -1)
        return 0;

    return &ttys[current_tty];
}

struct tty *get_tty(int i)
{
    if (i < 0 || i >= TTY_COUNT)
        return 0;

    return &ttys[i];
}

int tty_init()
{
    for (int i = 0; i < TTY_COUNT; i ++) {
        struct tty *t = kmalloc(sizeof(*t));

        t->input = default_input;
        t->output = 0;
        t->inputbuf = kmalloc(TTY_IN_BUFSIZE);
        t->outputbuf = kmalloc(TTY_OUT_BUFSIZE);
    }
    tty_set_current(0);

    printk("tty: there are %d TTYs\n", TTY_COUNT);
    return 0;
}

