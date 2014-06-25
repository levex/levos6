KERN_NAME=kernel.img
ARCH=x86

OBJS=main.o

all: $(KERN_NAME)

start: $(KERN_NAME)
	@echo "  QEMU     $(KERN_NAME)"
	@$(QEMU) -kernel $(KERN_NAME) -serial stdio -hda hd.img

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


clean:
	-rm $(OBJS)
	-rm $(KERN_NAME)
