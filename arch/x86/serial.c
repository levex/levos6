#include <levos/levos.h>

#define SERIAL_PORT 0x3F8

static void serial_out(int o, uint8_t d)
{
    outportb(SERIAL_PORT + o, d);
}

static int serial_is_ready_to_send()
{
    return inportb(SERIAL_PORT + 5) & 0x20;
}

static int serial_has_recv()
{
    return inportb(SERIAL_PORT + 5) & (1 << 0);
}

void serial_out_data(uint8_t d)
{
    while (!serial_is_ready_to_send())
        ;

    serial_out(0, d);
}

uint8_t serial_read_data()
{
    while (!serial_has_recv())
        ;

    return inportb(SERIAL_PORT + 0);
}

uint8_t serial_try_read_data()
{
    if (!serial_has_recv())
        return 0;

    return inportb(SERIAL_PORT + 0);
}

int serial_write(struct device *dev, char *buf, size_t count, size_t pos)
{
    int i;

    for (i = 0; i < count; i++)
        serial_out_data(buf[i]);

    return i;
}

int serial_read(struct device *dev, char *buf, size_t count, size_t pos)
{
    int i;

    for (i = 0; i < count; i++) {
        uint8_t val = serial_try_read_data();

        if (!val)
            break;

        buf[i] = val;
    }

    return i;
}

struct device serial_dev = {
    .name = "stty0",
    .write = (void *) serial_write,
    .read = (void *) serial_read,
};

struct console serial_con = {
    .dev = &serial_dev,
};

int x86_serial_init() {
    serial_out(1, 0x00);
    serial_out(3, 0x80);
    serial_out(0, 0x03);
    serial_out(1, 0x00);
    serial_out(3, 0x03);
    serial_out(2, 0xC7);
    serial_out(4, 0x0B);

    tty_set_default_input(&serial_dev);

    device_register(&serial_dev);
    console_register(&serial_con);
    return 0;
}


