#include <levos/levos.h>

int device_register(struct device *dev)
{
	if (!dev)
		return -EINVAL;
	
	return 0;
}
