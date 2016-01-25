#include <levos/levos.h>
#include <levos/ata.h>

#define ATA_PRIMARY_IRQ 14
#define ATA_PRIMARY_IO 0x1F0
#define ATA_PRIMARY_DCR_AS 0x3F6

#define ATA_SECONDARY_IRQ 15
#define ATA_SECONDARY_IO 0x170
#define ATA_SECONDARY_DCR_AS 0x376

#define ATA_PRIMARY 0x00
#define ATA_SECONDARY 0x01

#define ATA_MASTER 0x00
#define ATA_SLAVE  0x01

void ide_select_drive(uint8_t bus, uint8_t i)
{
    if (bus == ATA_PRIMARY)
        if (i == ATA_MASTER)
            outportb(ATA_PRIMARY_IO + ATA_REG_HDDEVSEL, 0xA0);
        else outportb(ATA_PRIMARY_IO + ATA_REG_HDDEVSEL, 0xB0);
    else
        if (i == ATA_MASTER)
            outportb(ATA_SECONDARY_IO + ATA_REG_HDDEVSEL, 0xA0);
        else outportb(ATA_SECONDARY_IO + ATA_REG_HDDEVSEL, 0xB0);
}

void ide_400ns_delay(uint16_t io)
{
    for (int i = 0; i < 4; i++)
        inportb(io + ATA_REG_ALTSTATUS);
}

void ide_poll(uint16_t io)
{
    ide_400ns_delay(io);
    retry:;
    uint8_t status = inportb(io + ATA_REG_STATUS);
    if (status & ATA_SR_BSY) goto retry;
    retry2:;
    status = inportb(io + ATA_REG_STATUS);
    if (status & ATA_SR_ERR)
        panic ("ata: ERROR bit set!\n");
    if (!(status & ATA_SR_DRQ)) goto retry2;

    return;
}

void ata_read_one(struct device *dev, char *buf, size_t lba)
{
    uint16_t io = ATA_PRIMARY_IO;
    uint8_t  dr = ATA_MASTER;

    uint8_t cmd = 0xE0;
    uint8_t slavebit = 0x00;

    outportb(io + ATA_REG_HDDEVSEL, (cmd | (uint8_t)((lba >> 24 & 0x0F))));
    outportb(io + 1, 0x00);
    outportb(io + ATA_REG_SECCOUNT0, 1);
    outportb(io + ATA_REG_LBA0, (uint8_t)(lba));
    outportb(io + ATA_REG_LBA1, (uint8_t)(lba >> 8));
    outportb(io + ATA_REG_LBA2, (uint8_t)(lba >> 16));
    outportb(io + ATA_REG_COMMAND, ATA_CMD_READ_PIO);

    ide_poll(io);

    for (int i = 0; i < 256; i++) {
        uint16_t d = inportw(io + ATA_REG_DATA);
        *(uint16_t *)(buf + i * 2) = d;
    }

    ide_400ns_delay(io);
    return;
}

int ata_read(struct device *dev, void *buf, size_t count, size_t pos)
{
    for (int i = 0; i < count; i++)
    {
        ata_read_one(dev, buf, pos + i);
        buf += 512;
    }
    return count;
}

int ata_write(struct device *dev, const void *buf, size_t c, size_t p)
{
    return -ENOSYS;
}

int ata_sync(struct device *dev)
{
    return -ENOSYS;
}

int ide_identify()
{
    uint16_t io = 0;
    io = 0x1F0;
    outportb(ATA_PRIMARY_IO + ATA_REG_HDDEVSEL, 0xA0);
    outportb(io + ATA_REG_SECCOUNT0, 0);
    outportb(io + ATA_REG_LBA0, 0);
    outportb(io + ATA_REG_LBA1, 0);
    outportb(io + ATA_REG_LBA2, 0);
    outportb(io + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);

    uint8_t status = inportb(io + ATA_REG_STATUS);
    if (status)
    {
        /* read the IDENTIFY data */
        void *ide_buf = kmalloc(512);
        if (!ide_buf)
            return -ENOMEM;
        for (int i = 0; i < 256; i++)
            *(uint16_t *)(ide_buf + i*2) = inportw(io + ATA_REG_DATA);
        new_free(ide_buf);
        struct device *dev = kmalloc(sizeof(*dev));
        if (!dev)
            return -ENOMEM;
        dev->name = "sda";
        dev->type = DEV_TYPE_BLK;
        dev->read = ata_read;
        dev->write = ata_write;
        dev->sync = ata_sync;
        device_register(dev);
        return 1;
    } else {
        printk("ata: IDENTIFY error on b0d0 -> no status\n");
        return 0;
    }
}

int ata_probe()
{
    int devs = 0;
    if (ide_identify() > 0) {
        printk("ata: primary master is online\n");
        devs ++;
    }
    printk("ata: %d devices brought online\n", devs);
    return 0;
}

DEF_IRQ_HANDLER(0x20, ide_prim_irq)
{
    return;
}

int ata_init()
{
    int rc = 0;

    printk("ata: using IDE mode, disregarding secondary\n");
    irq_set(0x20 + ATA_PRIMARY_IRQ, ide_prim_irq);
    rc = ata_probe();
    return rc;
}
