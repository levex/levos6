#include <levos/levos.h>

#define KMSG_BUFSIZE 65536

static char kmsg_buf[KMSG_BUFSIZE];
static int   kmsg_pos = 0;

void __kmsg_append(char c)
{
    if (!c)
        return;
    if (kmsg_pos >= KMSG_BUFSIZE)
        return;

    kmsg_buf[kmsg_pos ++] = c;
}

int kmsg_write(struct device *dev, char *buf, size_t count, size_t pos)
{
    int i;

    if (!dev || !buf || pos > KMSG_BUFSIZE || pos + count > KMSG_BUFSIZE)
        return -EINVAL;

    for (i = 0; i < count; i++)
        __kmsg_append(buf[i]);

    return i;
}

int kmsg_read(struct device *dev, const void *buf, size_t count, size_t pos)
{
    dev = dev;
    buf = buf;
    count = count;
    pos = pos;
    return -ENOSYS;
}

int kmsg_flush(struct device *dev)
{
    dev = dev;
    return -ENOSYS;
}

struct device kmsg_dev = {
    .write = (void *)kmsg_write,
    .read = (void *)kmsg_read,
    .sync = kmsg_flush,
    .name = "kmsg",
};

struct console kmsg_con = {
    .dev = &kmsg_dev,
};

int kmsg_init()
{
    memset(kmsg_buf, 0, KMSG_BUFSIZE);
    device_register(&kmsg_dev);
}
