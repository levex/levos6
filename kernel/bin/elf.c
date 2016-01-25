#include <levos/levos.h>

int elf_probe(struct file *filp)
{
	char magic[4];

	filp->fops->read(filp, magic, 4);

	if (magic[0] == 0x7F &&
		magic[1] == 'E'  &&
		magic[2] == 'L'  &&
		magic[3] == 'F')
		return 1;
	printk("%x %x %x %x", magic[0], magic[1], magic[2], magic[3]);
	return 0;
}

int elf_load(struct file *filp)
{
	elf_header_t header;
	elf_program_header_t *ph;
	int a = 0;

	/* reset file back to the beginning of the file */
	file_set_position(filp, 0);

	if (!elf_probe(filp))
		return -EBADF;

	/* read in ELF header */
	filp->fops->read(filp, &header, sizeof(elf_header_t));

	if (header.e_type >= 2) {
		printk("%d\n", header.e_type);
		return -ENOEXEC;
	}

	a = sizeof(elf_program_header_t) * header.e_phnum;

	ph = kmalloc(a);
	if (!ph)
		return -ENOMEM;

	file_set_position(filp, header.e_phoff);
	filp->fops->read(filp, ph, a);

	/* loop through all the program headers */
	for (int i = 0; i < header.e_phnum; i++, ph++)
	{
		switch (ph->p_type) {
			case 0: /* NULL */
				break;
			case 1: /* LOAD */
				printk("LOAD: offset 0x%x vaddr 0x%x"
					"paddr 0x%x filesz 0x%x memsz 0x%x\n",
					ph->p_offset, ph->p_vaddr, ph->p_paddr,
					ph->p_filesz, ph->p_memsz);
				break;
			default:
				break;
		}
	}

	/* the implementation is not perfect, let the user know it */
	return -ENOSYS;
}
