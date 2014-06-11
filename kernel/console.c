#include <levos/console.h>
#include <errno.h>

int console_send(char c) {
	return -ENOSYS;
}

int console_sys_init() {
	return -ENOMEM;
}
