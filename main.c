#include <stdint.h>
#include <levos/arch.h>

void main(void) {

	/* init the kmsg buffer and printk */
	kmsg_init();
	
	/* arch might want to setup stuff */
	arch_early_init();

	/* setup consoles to send output to user */
	console_sys_init();
	/* setup ttys */
	tty_init();

	/* arch should finish init now */
	arch_late_init();

	/* wait forever */
	while(1);

}
