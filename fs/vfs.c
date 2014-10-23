#include <levos/levos.h>

#define MAX_MOUNTS 256

struct mount *mounts[MAX_MOUNTS];
int nmounts = 0;

struct mount *root_mount = 0;

struct filesystem *filesystems[8];
int nfilesystems = 0;


struct mount *__check_mounts(char *str)
{
	for (int i = 0; i < nmounts; i++)
		if (strcmp(mounts[i]->point, str) == 0)
			return mounts[i];
	
	return 0;
}

int __vfs_set_mount(char *p, struct device *dev, struct filesystem *fs)
{
	struct mount *m = kmalloc(sizeof(*m));
	m->point = p;
	m->fs = fs;
	m->dev = dev;
	mounts[nmounts] = m;
	nmounts ++;
	printk("vfs: mounted %s on %s to %s\n", fs->fsname, dev->name, p);
	if (strcmp(p, "/") == 0)
		root_mount = m;
	return 0;
}

struct mount *find_mount(char *path)
{
	struct mount *ret = root_mount;
	char *str;

	/* create a copy of the string */
	str = kmalloc(strlen(path) + 1);
	memcpy(str, path, strlen(path) + 1);
	if (strcmp(str, "/") == 0)
		goto out;
	
	while (strlen(str) > 1) {
		ret = __check_mounts(str);
		if (ret)
			goto out;
		str[strlen(str) - 1] = 0;
	}

	ret = root_mount;
out:
	new_free(str);
	return ret;
}

struct filesystem *vfs_mount_fs(struct device *dev)
{
	struct filesystem *fs = 0;
	for (int i = 0; i < nfilesystems; i++)
	{
		if (filesystems[i]->mount) {
			fs = filesystems[i]->mount(dev);
			if ((int)fs > 0)
				return fs;	
		}
	}
	return 0;
}

int vfs_mount(char *path, struct device *dev)
{
	if (!root_mount && strcmp("/", path) != 0)
		return -EAGAIN;

	struct filesystem *fs = vfs_mount_fs(dev);
	if (!fs)
		return -ENXIO;
	
	return __vfs_set_mount(path, dev, fs);
}

int vfs_root_mounted()
{
	return (root_mount)?1:0;
}

int register_fs(struct filesystem *fs)
{
	if (!fs)
		return -EINVAL;
	
	printk("vfs: registered filesystem %s\n", fs->fsname);

	filesystems[nfilesystems ++] = fs;
	return 0;
}


struct file *vfs_open(char *path)
{
	struct mount *m = find_mount(path);
	return m->fs->open(m->fs, path);
}

int vfs_stat(char *p, struct stat *buf)
{
	struct mount *m = find_mount(p);
	if (!m)
		panic("Location %s was not mounted!\n", p);
	
	if (!m->fs)
		panic("Mount deciphered for %s had not fs\n", p);
	
	if (!m->fs->stat)
		return -ENOSYS;
	return m->fs->stat(m->fs, p, buf);
}

int vfs_init()
{
	printk("vfs: loading filesystems\n");

	devfs_init();
	ext2_init();

	return 0;
}
