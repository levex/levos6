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
	struct filesystem *fs = f->fs;
	struct ext2_inode *ibuf = kmalloc(sizeof(*ibuf));
	int ino = ext2_find_file_inode(fs, f->respath);
	int bs = EXT2_PRIV(fs)->blocksize;
	if (ino < 0)
		return ino;

	ext2_read_inode(fs, ibuf, ino);

	if (b < 12) {
		ext2_read_block(fs, buf, ibuf->dbp[b]);
	} else {
		if (b < bs / sizeof(uint32_t)) {
			uint32_t *bb = kmalloc(bs);
			ext2_read_block(f->fs, bb, ibuf->singly_block);
			uint32_t b = bb[b - 12];
			ext2_read_block(f->fs, buf, b);
			new_free(bb);
			goto exit;
		}
		printk("File blocks >+%d arent supported\n", bs / sizeof(uint32_t));
		return -ENOSYS;
	}
	
exit:	new_free(ibuf);
	return 0;
}

int ext2_read_file(struct file *f, void *buf, size_t count)
{
	if (!count)
		return 0;

	if (!f || !buf)
		return -EINVAL;
	
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

	void *block = kmalloc(bs * (blocks + 1));
	if (!block)
		return -ENOMEM;

	/* contignous reading .... @TODO */
	ext2_file_read_block(f, block, cblock);
	for (int i = 0; i < blocks; i++)
		ext2_file_read_block(f, block + (i + 1) * bs, cblock + i + 1);
	
	/* now read the bytes */
	memcpy(buf, block + (f->pos - cblock * bs), count);

	f->pos += count;
	
	return count;
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
