#include <levos/levos.h>


struct dirent *devfs_readdir(struct file *d)
{
    return 0;
}

struct filesystem *devfs_mount(struct device *dev)
{
    return (void *) -ENODEV;
}

struct file *devfs_open(struct filesystem *fs, char *p)
{
    if (!fs || !p)
        return (void *) -EINVAL;

    struct file *f = kmalloc(sizeof(*f));
    if (!f)
        return (void *) -ENOMEM;

    struct file_operations *fops = kmalloc(sizeof(*fops));
    if (!fops) {
        new_free(f);
        return (void *) -ENOMEM;
    }

    f->fs = fs;
    f->respath = p;
    f->fops = fops;
    return (void *) -ENOSYS;
}

struct filesystem devfs = {
    .fsname = "devfs",
    .readdir = devfs_readdir,
    .mount = devfs_mount,
    .open = devfs_open,
};

int devfs_init()
{
    printk("devfs: there are %d devices\n", get_num_of_devices());

    register_fs(&devfs);

    return 0;
}
