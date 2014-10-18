CC=i586-elf-gcc
LD=i586-elf-gcc
AS=i586-elf-as
QEMU=qemu-system-i386
CFLAGS=-Iinclude -std=gnu99 -ffreestanding -O2
LDFLAGS=-ffreestanding -nostdlib -O2

OBJS += arch/x86/start.ao
OBJS += arch/x86/textmode.o
OBJS += arch/x86/init.o
OBJS += arch/x86/serial.o
OBJS += arch/x86/hal.o
OBJS += arch/x86/gdt.o
OBJS += arch/x86/gdt.ao
OBJS += arch/x86/idt.o arch/x86/idt.ao
OBJS += arch/x86/exception.o
OBJS += arch/x86/irq.o
OBJS += arch/x86/multiboot.o
OBJS += arch/x86/pit.o
OBJS += arch/x86/sched.o
