#include <levos/device.h>
#include <errno.h>

static struct tty *ttys[TTY_COUNT];
static int active_tty = -1;

static struct device *default_input = 0;
static struct device *default_output = 0;

#define for_each_tty(t) t = ttys[0]; \
                            for (int i = 0; \
                                i < TTY_COUNT - 1; \
                                i ++, t = ttys[i])

void tty_set_default_output(struct device *d)
{
    default_input = d;
}

void tty_set_current(int n)
{
    if (active_tty == -1)
        printk("boot: current tty will be tty%d\n", n);

    active_tty = n;
}

struct tty *get_active_tty()
{
    if (active_tty == -1)
        return 0;

    return ttys[active_tty];
}

struct tty *get_tty(int i)
{
    if (i < 0 || i >= TTY_COUNT)
        return 0;

    return ttys[i];
}

void tty_set_input(struct tty *tty, struct device *in)
{
    tty->input = in;
}

void tty_set_output(struct tty *tty, struct device *out)
{
    tty->output = out;
}

void tty_override_all_defaults()
{
    struct tty *t;

    for_each_tty(t) {
        tty_set_input(t, default_input);
        tty_set_output(t, default_output);
    }
}

int tty_write(struct device *dev, const void *buf, size_t count, size_t pos)
{
    struct tty_private *priv = dev->priv;
    struct tty *tty = priv->tty;

    return -ENOSYS;
}

int tty_read(struct device *dev, void *buf, size_t count, size_t pos)
{
    struct tty_private *priv = dev->priv;
    struct tty *tty = priv->tty;

    return -ENOSYS;
}

int tty_flush(struct device *dev)
{
    struct tty_private *priv = dev->priv;
    struct tty *tty = priv->tty;
    tty = tty;

    return -ENOSYS;
}

struct device *tty_create_device(struct tty *tty)
{
    struct device *dev;
    struct tty_private *priv;
    static char *string = "tty0";
    char *ptr = 0;

    if (tty->selfdevice)
        return tty->selfdevice;

    dev = kmalloc(sizeof(*tty));
    if (!dev)
        return (void *) -ENOMEM;

    priv = kmalloc(sizeof(*priv));
    if (!priv) {
        new_free(dev);
        return (void *) -ENOMEM;
    }

    ptr = kmalloc(strlen(string) + 1);
    if (!ptr) {
        new_free(dev);
        new_free(priv);
        return (void *) -ENOMEM;
    }
    memcpy(ptr, string, strlen(string) + 1);

    /* FIXME: name needs asprintf */
    dev->name  = ptr;
    dev->name[3] += tty->tty_id;
    dev->type  = DEV_TYPE_TTY;
    dev->priv  = priv;
    dev->write = tty_write;
    dev->read  = tty_read;
    dev->sync  = tty_flush;
    dev->fs    = 0;

    return dev;
}

int tty_init()
{
    for (int i = 0; i < TTY_COUNT; i ++) {
        struct tty *t = kmalloc(sizeof(*t));

        t->tty_id = i;
        t->input = default_input;
        t->output = 0;
        t->inputbuf = kmalloc(TTY_IN_BUFSIZE);
        t->outputbuf = kmalloc(TTY_OUT_BUFSIZE);
        t->selfdevice = 0;
        ttys[i] = t;

        device_register(tty_create_device(t));
    }
    tty_set_current(0);

    tty_override_all_defaults();

    printk("tty: there are %d TTYs\n", TTY_COUNT);
    return 0;
}
