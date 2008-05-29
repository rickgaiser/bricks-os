	.text
// -----------------------------------------------------------------------------
// Wrapper for gpf task
	.global _gpf;
_gpf:
	call gpf
	iret
	jmp _gpf
