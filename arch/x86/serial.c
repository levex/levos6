#include <levos/levos.h>

#define SERIAL_PORT 0x3F8

void serial_out(int o, uint8_t d)
{
    outportb(SERIAL_PORT + o, d);
}

int serial_is_ready()
{
    return inportb(SERIAL_PORT + 5) & 0x20;
}

int serial_write(struct device *dev, char *buf, size_t count, size_t pos)
{
    int i;

    for (i = 0; i < count; i++)
        serial_out(0, buf[i]);

    return i;
}

struct device serial_dev = {
    .name = "stty",
    .write = (void *)serial_write,
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

    device_register(&serial_dev);
    console_register(&serial_con);
    return 0;
}


