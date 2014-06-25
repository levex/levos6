#include <levos/levos.h>

int ext2_read_block(struct filesystem *fs, void *buf, uint32_t block)
{
	uint32_t spb = EXT2_PRIV(fs)->sectors_per_block;
	if (!spb) spb ++;
	fs->dev->read(fs->dev, buf, spb, block * spb);
	return 0;
}
