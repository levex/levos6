#include <levos/levos.h>

struct filesystem *ext2_mount(struct device *dev);
int ext2_stat(struct filesystem *fs, char *f, struct stat *buf);

struct filesystem ext2_fs = {
    .fsname = "ext2",
    .priv_data = 0,
    .readdir = 0,
    .stat = ext2_stat,
    .mount = ext2_mount,
    .open = ext2_open,
};

struct filesystem *__fs = 0;

struct filesystem *__get__fs()
{
    return __fs;
}

struct filesystem *ext2_mount(struct device *dev)
{
    if (dev->type != DEV_TYPE_BLK)
        return (void *) -ENODEV;

    void *buf = kmalloc(1024);
    dev->read(dev, buf, 2, 2);
    struct ext2_superblock *sb = (struct ext2_superblock *)buf;
    if (sb->ext2_sig != EXT2_SIGNATURE) {
        printk("ext2: signature mismatch on device %s\n", dev->name);
        return (void *) -EINVAL;
    }
    struct ext2_priv_data *p = kmalloc(sizeof(*p));
    struct filesystem *fs = kmalloc(sizeof(*fs));
    p->blocksize = 1024 << sb->blocksize_hint;
    p->inodes_per_block = p->blocksize / sizeof(struct ext2_inode);
    p->sectors_per_block = p->blocksize / 512;
    printk("ext2: %s: volume size: %d bytes\n", dev->name, p->blocksize * sb->blocks);
    uint32_t number_of_bgs0 = sb->blocks / sb->blocks_in_blockgroup;
    if (!number_of_bgs0) number_of_bgs0 = 1;
    p->number_of_bgs = number_of_bgs0;

    p->first_bgd = sb->superblock_id + sizeof(struct ext2_superblock) / p->blocksize;
    memcpy(&p->sb, (void *) sb, sizeof(struct ext2_superblock));


    dev->fs = fs;
    memcpy(fs, (void *) &ext2_fs, sizeof(*fs));
    fs->priv_data = p;
    fs->dev = dev;

    __fs = fs;

    return fs;
}

int ext2_init()
{
    printk("ext2: registered filesystem to vfs\n");
    struct filesystem *f = kmalloc(sizeof(*f));
    memcpy(f, (void *) &ext2_fs, sizeof(*f));
    register_fs(f);
    return 0;
}
