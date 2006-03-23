#ifdef ARM7
  .text
#endif
#ifdef ARM9
  .section .itcm, "ax", %progbits
#endif

  .extern isr
  .code 32

  .global __isr
@ ------------------------------------------------------------------------------
__isr:
    ldr     r0, =isr
    bx      r0
    .align
    .pool
