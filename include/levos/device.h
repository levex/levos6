#ifndef __DEVICE_H
#define __DEVICE_H

#include <levos/levos.h>

#define DEV_TYPE_PSEUDO     0
#define DEV_TYPE_ISA        1
#define DEV_TYPE_PCI        2
#define DEV_TYPE_TTY        3
#define DEV_TYPE_BLK        4
#define DEV_TYPE_TTY_INPUT  5

struct device {
    char *name; /* name of the device as in devfs */
    uint32_t type; /* device type for automatic container_of */

    int (*read)(struct device *dev, void *buf, size_t count, size_t pos);
    int (*write)(struct device *dev, const void *buf, size_t count, size_t pos);
    int (*sync)(struct device *dev);

    void *priv; /* private data as the device pleases */
    struct filesystem *fs; /* filesystem if any */
};

extern int device_register(struct device *dev);

extern int get_num_of_devices();
extern struct device *device_get(int i);

#endif
