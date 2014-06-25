.global _idt_load
.type _idt_load, @function
_idt_load:
	push %ebp
	movl %esp, %ebp
	
	lidt _idtr
	
	movl %ebp, %esp
	pop %ebp
	ret
