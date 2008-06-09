.data
hw:
	.string "hello linux\n"
.text
.globl _start
_start:
	movl	$4,%eax # $SYS_write == $4
	movl	$1,%ebx
	movl	$hw,%ecx
	movl	$12,%edx
	int	$0x80
	movl	$1,%eax # $SYS_exit == &1
	xorl	%ebx,%ebx
	int	$0x80

