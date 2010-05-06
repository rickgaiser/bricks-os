@
@ r0 - r13: General Purpose Registers
@ r13: Also used as stack pointer (SP)
@ r14: Link Register (LR)
@ r15: Program Counter (PC)
@ SPSR: Saved Program Status Register, only available in privileged modes
@
.extern isr

.global MultiBoot
.global local_save_flags
.global local_irq_restore
.global local_irq_disable
.global local_irq_enable
.global __jump
.global __gba_isr
.global __isr
.global current_thread

  .align 4
  .data

current_thread:     .word 0x0         @ pointer to thread that is running


@ -----------------------------------------------------------------------------
@ uint32_t MultiBoot(MultiBootParam * mp, uint32_t mode);
@  - code: THUMB
@ -----------------------------------------------------------------------------
  .text
  .code 16
  .thumb_func
MultiBoot:
  swi     37
  bx      lr

@ -----------------------------------------------------------------------------
@ unsigned long local_save_flags(void);
@  - code: THUMB
@ -----------------------------------------------------------------------------
  .text
  .code 16
  .thumb_func
local_save_flags:
  ldr     r2, =local_save_flagsARM
  bx      r2
  .data
  .code 32
local_save_flagsARM:
  mrs     r0, cpsr
  bx      lr

@ -----------------------------------------------------------------------------
@ void local_irq_restore(unsigned long flags);
@  - code: THUMB
@  - r0 = flags
@ -----------------------------------------------------------------------------
  .text
  .code 16
  .thumb_func
local_irq_restore:
  ldr     r2, =local_irq_restoreARM
  bx      r2
  .data
  .code 32
local_irq_restoreARM:
  msr     cpsr, r0
  bx      lr

@ -----------------------------------------------------------------------------
@ void local_irq_disable(void);
@  - code: THUMB
@ -----------------------------------------------------------------------------
  .text
  .code 16
  .thumb_func
local_irq_disable:
  ldr     r2, =local_irq_disableARM
  bx      r2
  .data
  .code 32
local_irq_disableARM:
  mrs     r0, cpsr
  orr     r0, r0, #0xc0
  msr     cpsr, r0
  bx      lr

@ -----------------------------------------------------------------------------
@ void local_irq_enable(void);
@  - code: THUMB
@ -----------------------------------------------------------------------------
  .text
  .code 16
  .thumb_func
local_irq_enable:
  ldr     r2, =local_irq_enableARM
  bx      r2
  .data
  .code 32
local_irq_enableARM:
  mrs     r0, cpsr
  bic     r0, r0, #0xc0
  msr     cpsr, r0
  bx      lr

@ -----------------------------------------------------------------------------
@ void __jump(pt_regs * current_thread, pt_regs * new_thread);
@   Special function for task switching WITHOUT interrupt. Saves the current
@   state and loads the state of the new task.
@  - code: THUMB
@  - mode: System/User
@  - r0 = old_thread / current_thread
@  - r1 = new_thread
@ -----------------------------------------------------------------------------
  .text
  .code 16
  .thumb_func
__jump:
  ldr     r2, =__jump_ARM
  bx      r2
  .data
  .code 32
__jump_ARM:
  @ Save r0-r12,r14
  stmia   r0!, {r1-r12}               @ Save r1-r12
  stmia   r0!, {r0}                   @ Save r0
  ldr     r2, =__jump_Return
  stmia   r0!, {r2}                   @ Save pc

  mrs     r4, cpsr
  str     r4, [r0], #4                @ Save cpsr
  str     lr, [r0], #4                @ Save lr
  str     sp, [r0], #4                @ Save sp

  str     r1, current_thread
  b       __isr_Return

  nop
__jump_Return:
  bx      lr


@ ---------------------------------------------------------------------------
@ GBA/NDS ARM ISR
@ ---------------------------------------------------------------------------
@ Upon interrupt the GBA bios is called first:
@   push r0-r3,r12,r14
@   call __gba_isr
@   pop  r0-r3,r12,r14
@   ret
@ -----------------------------------------------------------------------------
  .code 32
__gba_isr:
  ldmfd   sp!, {r0-r3,r12,r14}        @ pop registers pushed by GBA BIOS
@  b       _isr

@ ---------------------------------------------------------------------------
@ ARM ISR
@   Saves the current context to "current_thread" and calls isr.
@  - code: ARM
@  - mode: IRQ
@ ---------------------------------------------------------------------------
@  .code 32
__isr:
  stmfd   sp!, {r0}
  ldr     r0, current_thread
  stmia   r0!, {r1-r12}               @ Save r1-r12
  ldmfd   sp!, {r1}
  stmia   r0!, {r1}                   @ Save r0
  stmia   r0!, {r14}                  @ Save r14/pc

  mrs     r5, spsr
  str     r5, [r0], #4                @ Save cpsr

  @ Switch to system mode
  mrs     r4, cpsr                    @ get cpu flags
  bic     r4, r4, #0x1f               @ clear state code
  orr     r4, r4, #0x1f               @ system mode
  msr     cpsr, r4

  str     lr, [r0], #4                @ Save lr
  str     sp, [r0], #4                @ Save sp

  @ Call generic interrupt handler, THUMB mode
  ldr     lr, =__isr_Return
  ldr     r0, current_thread
  ldr     r1, =isr
  bx      r1

@ -----------------------------------------------------------------------------
@   Return from interrupt. Loads "current_state" and returns to it.
@   NOTE: Current context is destoyed!
@  - code: ARM
@  - mode: System/User
@ -----------------------------------------------------------------------------
  .code 32
__isr_Return:
  ldr     r0, current_thread
  add     r0, r0, #68                 @ set pointer to end of current task state

  ldr     sp, [r0, #-4]!              @ Restore sp
  ldr     lr, [r0, #-4]!              @ Restore lr

  @ Switch to irq mode
  mrs     r4, cpsr                    @ get cpu flags
  bic     r4, r4, #0x1f               @ clear state code
  orr     r4, r4, #0x12               @ irq mode
  msr     cpsr, r4

  ldr     r5, [r0, #-4]!
  msr     spsr, r5                    @ Restore cprs

  ldmdb   r0!, {r14}                  @ Restore r14/pc
  ldmdb   r0!, {r1}                   @ Load r0
  stmfd   sp!, {r1}                   @ Push r0
  ldmdb   r0!, {r1-r12}               @ Restore r1-r12
  ldmfd   sp!, {r0}                   @ Restore r0

  subs    pc, lr, #4                  @ interrupt return to task


  .pool
  .end
