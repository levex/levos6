KERN_NAME=kernel.img
ARCH=x86

OBJS=main.o
CFLAGS += -Iinclude
all: $(KERN_NAME)

include arch/$(ARCH)/Makefile
include kernel/Makefile
include drivers/Makefile
include lib/Makefile

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
