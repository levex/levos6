#include <levos/levos.h>

int ext2_find_file_inode(struct filesystem *fs, char *path)
{
	char *fn = kmalloc(strlen(path) + 1);
	memcpy(fn, path, strlen(path) + 1);
	char *pch;
	pch = strtok(fn, "/");
	if (!pch) {
		/* user wants the R! (root directory) */
		return 2;
	}
	/* this means that the tokenizing has at least one token, so
	 * let us find the inode number of that token */
	int ino = 2;
	while (pch != 0) {
		ino = ext2_read_directory(fs, ino, pch);
		if (ino < 0)
			return -ENOENT;
		pch = strtok(0, "/");
	}
	return ino;
}

int ext2_stat(struct filesystem *fs, char *p, struct stat *buf)
{
	if (!p || !buf || !fs)
		return -EINVAL;

	int inode = ext2_find_file_inode(fs, p);
	if (inode < 0)
		return inode;

	struct ext2_inode *ibuf = kmalloc(sizeof(*ibuf));
	if (!ibuf)
		return -ENOMEM;
	
	ext2_read_inode(fs, ibuf, inode);
	buf->st_dev = 0;
	buf->st_ino = inode;
	buf->st_mode = ibuf->type;
	buf->st_nlink = ibuf->hardlinks;
	buf->st_uid = ibuf->uid;
	buf->st_gid = ibuf->gid;
	buf->st_rdev = 0;
	buf->st_size = ibuf->size;

	return 0;
}

int ext2_file_read_block(struct file *f, void *buf, size_t b)
{
	if (b < 0 || !f || !buf)
		return -EINVAL;
	
	struct ext2_inode *inode = kmalloc(sizeof(*inode));
	if (!inode)
		return -ENOMEM;

	int in = ext2_find_file_inode(f->fs, f->respath);
	if (in < 0)
		return in;
	
	ext2_read_inode(f->fs, inode, in);

	if (b < 12) {
		/* dbp */
		int _b = inode->dbp[b];
		ext2_read_block(f->fs, buf, _b);
		new_free(inode);
		return 0;
	} else {
		printk("ext2: read of fblock %d is not yet supported!\n");
		new_free(inode);
		return -ENOSYS;
	}
}

int ext2_read_file(struct file *f, void *buf, size_t count)
{
	int inode = ext2_find_file_inode(f->fs, f->respath);
	if (inode < 0)
		return inode;
	
	struct ext2_inode *ibuf = kmalloc(sizeof(*ibuf));
	if (!ibuf)
		return -ENOMEM;
	
	ext2_read_inode(f->fs, ibuf, inode);
	int total;
	int bs = EXT2_PRIV(f->fs)->blocksize;

	/* determine how many blocks do we need */
	int blocks = count / bs;
	int off = count % bs;
	int rblocks = 0; /* blocks read so far */

	/* determine position */
	int cblock = f->pos / bs;
	int coff = f->pos % bs;
	
	int opos = f->pos;

	void *block = kmalloc(bs);
	if (!block)
		return -ENOMEM;

	return f->pos - opos;
}

int ext2_write_file(struct file *f, const void *buf, size_t count)
{
	return -EROFS;
}

struct file_operations ext2_fops = {
	.read = ext2_read_file,
	.write = ext2_write_file,
};

struct file *ext2_open(struct filesystem *fs, char *p)
{
	if (!fs || !p)
		return -EINVAL;
	
	struct file *f = kmalloc(sizeof(*fs));
	if (!f)
		return -ENOMEM;

	f->fops = &ext2_fops;
	f->fs = fs;
	f->respath = p;
	f->isdir = -1;
	f->pos = 0;

	return f;
}
