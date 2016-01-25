#ifndef __ARCH_H
#define __ARCH_H

#ifdef __LEVOS_ARCH_x86__
#include <levos/x86.h>
#elif defined(__LEVOS_ARCH_arm__)
#include <levos/arm.h>
#endif

#include <levos/page.h>
#include <levos/sched.h>

/* Called: right after kmsg_init() succeeds
 * Goal: setup memory management
 */

extern int arch_early_init();

/* Called: right before setting up scheduler
 * Goal: finish initilization
 */
extern int arch_late_init();

extern uint32_t arch_get_ticks();

extern void arch_do_mboot();

void switch_page_dir(page_dir_t *p);

extern int arch_sched_make_address_space(struct process *p);
extern void arch_sched_setup_address_space(struct process *p);
extern int arch_sched_mk_initial_regs(struct pt_regs *);
extern void arch_sched_setup_stack(struct process *);

extern void irq_set(size_t n, void (*h)(void));
extern void send_eoi();

uint8_t inportb(uint16_t p);
uint16_t inportw(uint16_t p);

void outportb(uint16_t p, uint8_t v);

#endif
