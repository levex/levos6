#include <levos/levos.h>

#define MAX_DEVICES 256

static struct device *devs[MAX_DEVICES];
static int devices = 0;


int device_register(struct device *dev)
{
	if (!dev)
		return -EINVAL;
	
	if (devices >= MAX_DEVICES)
		return -ENOSPC;

	devs[devices ++] = dev;

	return 0;
}

int get_num_of_devices()
{
	return devices;
}

struct device *device_get(int i)
{
	if (i >= MAX_DEVICES)
		return 0;

	return devs[i];
}
