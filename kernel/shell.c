#include <levos/levos.h>
#include <stdarg.h>

static void _printk(char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);

	vprintk(fmt, ap);

	va_end(ap);
}

void show_prompt()
{
	_printk("LevOS$ ");
}

char *grab_input()
{
	static char buffer[512];
	int i = 0;

	memset(buffer, 0, 512);

	while(1) {
		char c = keyboard_get_key();
		if (!c) continue;
		if (c == '\n')
			break;
		buffer[i++] = c;
		__printk_emit(c);
	}
	__printk_emit('\n');
	return buffer;
}

void cmd_ps()
{
	_printk("NAME [PID] extra\n");
	for (int i = 0; i < PROC_QUEUE_SIZE; i++) {
		struct process *p = process_queue[i];

		if (!p)
			continue;

		_printk("%s [%d] %s\n", p->comm, p->pid,
				current == p ? "[current]" : "");
	}
}

void cmd_lsdev()
{
	_printk("NAME TYPE\n");
	for (int i = 0; i < get_num_of_devices(); i++) {
		struct device *d = device_get(i);

		if (!d) {
			i --;
			continue;
		}

		_printk("%s %s\n", d->name,
				d->type == DEV_TYPE_TTY ? "[tty]"
				: d->type == DEV_TYPE_PSEUDO ? "[pseudo]"
				: d->type == DEV_TYPE_ISA ? "[isa]"
				: d->type == DEV_TYPE_PCI ? "[pci]"
				: d->type == DEV_TYPE_BLK ? "[blk]"
				: d->type == DEV_TYPE_TTY_INPUT ? "[intty]"
				: "[unktype]");
	}
}

void parse_input(char *cmd)
{
	if(strcmp("ps", cmd) == 0) {
		cmd_ps();
	} else if (strcmp("lsdev", cmd) == 0) {
		cmd_lsdev();
	}
}

void kernel_shell_start()
{
	printk("Starting LevOS debug shell...\n");
	while (1) {
		show_prompt();
		char *in = grab_input();
		parse_input(in);
	}
}
