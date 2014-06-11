#ifndef __DEVICE_H
#define __DEVICE_H

#include <stdint.h>
#include <stddef.h>

#define DEV_TYPE_PSEUDO 0
#define DEV_TYPE_ISA    1
#define DEV_TYPE_PCI    2
#define DEV_TYPE_TTY    3


struct device {

	char *name; /* name of the device as in devfs */
	uint32_t type; /* device type for automatic container_of */

	int (*read)(struct device *dev, void *buf, size_t count, size_t pos);
	int (*write)(struct device *dev, const void *buf, size_t count, size_t pos);
	int (*sync)(struct device *dev);

	void *priv; /* private data as the device pleases */
};

#endif
