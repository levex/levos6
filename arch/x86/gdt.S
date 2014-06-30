.global _set_gdtr
.type _set_gdtr, @function
_set_gdtr:
	#push %ebp
	#movl %esp, %ebp

	lgdtl (gdtr_t)

	#movl %ebp, %esp
	#pop %ebp
	retl

.global _reload_segments
.type _reload_segments, @function
_reload_segments:
#	push %ebp
#	movl %esp, %ebp

	pushl %eax
	xor %eax, %eax
	movw $0x10, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	movw %ax, %ss
	popl %eax

	ljmp $0x8, $me
me:
#	movl %ebp, %esp
#	pop %ebp
	ret
