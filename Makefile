KERN_NAME=kernel.img
ARCH=x86

OBJS=main.o

QEMU_OPTS=-serial stdio -hda hd.img -no-reboot

all: $(KERN_NAME)

start: $(KERN_NAME)
	@echo "  QEMU     $(KERN_NAME)"
	@$(QEMU) -kernel $(KERN_NAME) $(QEMU_OPTS)

debug: $(KERN_NAME)
	@echo "  QEMU [D] $(KERN_NAME)"
	@$(QEMU) -s -S -kernel $(KERN_NAME) $(QEMU_OPTS)

include arch/$(ARCH)/Makefile
include kernel/Makefile
include mm/Makefile
include drivers/Makefile
include fs/Makefile
include lib/Makefile


CFLAGS += -Iinclude -D__LEVOS_ARCH_$(ARCH)__
CFLAGS += -g


%.o: %.c
	@echo "  CC       $@"
	@$(CC) $(CFLAGS) -c $< -o $@

%.ao: %.S
	@echo "  AS       $@"
	@$(AS) -c $< -o $@

$(KERN_NAME): $(OBJS)
	@echo "  LD       $@"
	@$(LD) $(LDFLAGS) -T arch/$(ARCH)/linker.ld -o $(KERN_NAME) $(OBJS)

preprocess: $(KERN_NAME)
	@echo "  PP       $(KERN_NAME)"
	@$(CC) $(CFLAGS) -E $(OBJS:.o=.c) 
	# -T arch/$(ARCH)/linker.ld $(OBJS)

clean:
	-rm $(OBJS)
	-rm $(KERN_NAME)
