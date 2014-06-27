#include <levos/levos.h>

void parse_multiboot(struct multiboot *mboot)
{
	arch_do_mboot(mboot);
}

#define DRIVER_INIT(x) rc = x(); if (rc) { printk(#x \
	" failed to init with rc=%d\n", rc); }

int drivers_init()
{
	int rc;
	DRIVER_INIT(vfs_init);
	DRIVER_INIT(tty_init);
	DRIVER_INIT(ata_init);
}

void start_init()
{
	/* try /bin/sh */
	char *p = "/levex.txt";
	printk("test: trying to find inode of %s\n", p);
	int i = ext2_find_file_inode(__get__fs());
	if (i < 0) {
		printk("test: %s does not exist!\n", p);
		return;
	}
	printk("test: inode of %s is %d\n", p, i);
	struct stat st;
	int rc = vfs_stat(p, &st);
	if (rc < 0) {
		printk("test: an error occured while stating\n");
		return;
	}
	printk("test: size of %s is %d bytes\n", p, st.st_size);

	struct file *f = vfs_open(p);
	if ((int)f <= 0) {
		printk("test: failed to open %s\n", p);
		return;
	}
	void *buffer = kmalloc(st.st_size);
	rc = f->fops->read(f, buffer, st.st_size);
	if (rc < 0) {
		printk("test: failed to read from file %s\n", p);
		return;
	}
	printk("test: contents: %s", buffer);
}

void do_mount() {
	for(int i = 0; i < get_num_of_devices(); i++)
		vfs_mount("/", device_get(i));
	
	if (!vfs_root_mounted())
		panic("unable to mount root directory\n");
}

void main(struct multiboot *mboot) {

	int rc;

	/* init the kmsg buffer and printk */
	console_sys_init();
	kmsg_init();

	/* arch might want to setup stuff */
	arch_early_init();

	printk("kmsg: buffer setup!\n");

	parse_multiboot(mboot);
	
	paging_fini();

	heap_install();

	/* arch should finish init now */
	arch_late_init();

	printk("sys: init done!\n");

	drivers_init();

	do_mount();
	start_init();

	/* wait forever */
	panic("finished with main, but no init was started!\n");
}
