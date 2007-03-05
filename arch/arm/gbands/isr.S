    .extern isr
    .global __isr
@    .section .iwram,"ax", %progbits
    .code 32

__isr:
    ldr     r0, =isr
    bx      r0
    .align
    .pool
