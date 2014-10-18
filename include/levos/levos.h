#ifndef __LEVOS_H
#define __LEVOS_H

#include <errno.h>
#include <stddef.h>
#include <stdint.h>


#define __weak __attribute__((weak))

#include <levos/arch.h>
#include <levos/console.h>
#include <levos/device.h>
#include <levos/memset.h>
#include <levos/kmsg.h>
#include <levos/tty.h>
#include <levos/lib.h>
#include <levos/multiboot.h>
#include <levos/page.h>
#include <levos/mm.h>
#include <levos/vfs.h>
#include <levos/ext2.h>
#include <levos/sched.h>

#endif /* __LEVOS_H */
