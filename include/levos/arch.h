#ifndef __ARCH_H
#define __ARCH_H

#include <levos/levos.h>

/* Called: right after kmsg_init() succeeds
 * Goal: setup memory management
 */
int __weak arch_early_init() {
	return -ENOSYS;
}

/* Called: right before setting up scheduler
 * Goal: finish initilization
 */
int __weak arch_late_init() {
	return -ENOSYS;
}

#endif
