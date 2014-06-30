.set ALIGN,    1<<1             
.set MEMINFO,  1<<1            
.set FLAGS,    ALIGN | MEMINFO  
.set MAGIC,    0x1BADB002       
.set CHECKSUM, -(MAGIC + FLAGS)


.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

.section .bootstrap_stack
stack_bottom:
.align 4
.skip 65536 # 16 KiB
stack_top:

.section .text

.global _start
.type _start, @function
_start:

	movl $stack_bottom, %esp
	mov $0x1337, %eax
	push %ebx
	call main
	
	mov $0xdeadc0de, %eax

	cli
	hlt

.Lhang:
	jmp .Lhang
