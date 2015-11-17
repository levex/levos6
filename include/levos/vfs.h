#ifndef __LEVOS_VFS_H
#define __LEVOS_VFS_H

#include <levos/levos.h>

struct file;
struct inode;
struct file_operations;
struct filesystem;

struct file_operations {
	int (*read)(struct file *f, void *buf, size_t count);
	int (*write)(struct file *f, const void *buf, size_t count);
};

struct inode {
	int id;
	struct inode *parent;
};

struct file {
	struct file_operations *fops;
	struct filesystem *fs;
	char *respath; /* restricted path to mount loc */
	int isdir;
	int pos;
};

struct mount {
	char *point;
	struct filesystem *fs;
	struct device *dev;
};

struct dirent {
	int d_ino;
	int d_off;
	uint16_t d_reclen;
	uint8_t d_type;
	char d_name[256];
};

struct stat {
	uint16_t st_dev;
	uint16_t st_ino;
	uint32_t st_mode;
	uint16_t st_nlink;
	uint16_t st_uid;
	uint16_t st_gid;
	uint16_t st_rdev;
	uint32_t st_size;
	/* 32*/
};

struct filesystem {
	char *fsname;
	void *priv_data;
	struct device *dev;
	struct dirent *(*readdir)(struct file *f);
	int (*stat)(struct filesystem *fs, char *p, struct stat *buf);
	struct file *(*open)(struct filesystem *f, char *rp);
	struct filesystem *(*mount)(struct device *dev);
};

int vfs_init();
int vfs_root_mounted();
int register_fs(struct filesystem *fs);

inline void file_set_position(struct file *f, int p)
{
	f->pos = p;
}
int devfs_init();

#endif
