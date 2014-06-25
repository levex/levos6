#include <levos/levos.h>

#define MAX_CONSOLES 4

static struct console *cons[MAX_CONSOLES];
static int consoles;

int console_send(char c) {
	int i, rc;
	
	for (i = 0; i < consoles; i++) {
		rc = cons[i]->dev->write(cons[i]->dev, &c, 1, 0); 
		if (rc <= 0) {
			panic("%s rc=%d\n", __func__, rc);
		}
	}
	return 0;
}

void dump_consoles() {
	int i;
	for (i = 0; i < consoles; i++)
		printk("CONSOLE %d: %s\n", i, cons[i]->dev->name);
}

int console_register(struct console *c)
{
	if (consoles >= MAX_CONSOLES)
		return -ENOSPC;
	
	cons[consoles] = c;

	consoles ++;

	return 0;
}

int console_sys_init() {
	int i;

	for(i = 0; i < MAX_CONSOLES; i++)
		cons[i] = 0;
	
	consoles = 0;
	
	return 0;
}
