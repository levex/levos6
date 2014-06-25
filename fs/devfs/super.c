#include <levos/levos.h>


struct dirent *devfs_readdir(struct file *d)
{
	return 0;
}

int devfs_mount(struct device *dev)
{
	return -ENODEV;
}

struct filesystem devfs = {
	.fsname = "devfs",
	.readdir = devfs_readdir,
	.mount = devfs_mount,
};

int devfs_init()
{
	printk("devfs: there are %d devices\n", get_num_of_devices());
	
	register_fs(&devfs);

	return 0;
}
