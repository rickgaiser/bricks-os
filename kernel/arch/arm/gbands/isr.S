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
.global runJump
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
@ void runJump(pt_regs * old_thread, pt_regs * new_thread);
@  - code: THUMB
@  - r0 = old_thread / current_thread
@  - r1 = new_thread
@ -----------------------------------------------------------------------------
  .text
  .code 16
  .thumb_func
runJump:
  ldr     r2, =runJumpARM
  bx      r2
  .data
  .code 32
runJumpARM:
  @ Save r0-r12,r14
  stmia   r0!, {r1-r12}               @ Save r1-r12
  stmia   r0!, {r0}                   @ Save r0 (garbage)
  ldr     r2, =runJumpReturn          @ Task will continue at runJumpReturn
  stmia   r0!, {r2}                   @ Save pc

  @ Save current mode cpsr as irq mode spsr
  mrs     r4, cpsr
  str     r4, [r0], #4

  @ Prepare registers r2-r4 for changing the cpu mode
  bic     r4, r4, #0x1f               @ clear state code
  orr     r2, r4, #0x13               @ supervisor mode
  orr     r3, r4, #0x1f               @ system mode
  orr     r4, r4, #0x12               @ irq mode

  @ Save supervisor mode spsr, lr & sp
  msr     cpsr, r2
  mrs     r5, spsr
  str     r5, [r0], #4
  str     lr, [r0], #4
  str     sp, [r0], #4

  @ Save system mode lr & sp
  msr     cpsr, r3
  str     lr, [r0], #4
  str     sp, [r0], #4

  msr     cpsr, r4                    @ Get in irq mode
  str     r1, current_thread          @ Make new_thread the new current_thread
  b       isr_ret

  nop
runJumpReturn:
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
@ ---------------------------------------------------------------------------
@  .code 32
__isr:
  @ Save r0-r12,r14
  stmfd   sp!, {r0}                   @ push r0
  ldr     r0, current_thread          @ load pointer to current task state in r0
  stmia   r0!, {r1-r12}               @ save r1-r12
  ldmfd   sp!, {r1}                   @ pop r0
  stmia   r0!, {r1}                   @ save r0
  stmia   r0!, {r14}                  @ save r14/pc

  @ Save irq mode spsr
  mrs     r5, spsr                    @ get the stored CPU state
  str     r5, [r0], #4                @ store value for cpsr

  @ Prepare registers r2-r4 for changing the cpu mode
  mrs     r4, cpsr                    @ get cpu flags
  bic     r4, r4, #0x1f               @ clear state code
  orr     r2, r4, #0x13               @ supervisor mode
  orr     r3, r4, #0x1f               @ system mode
  orr     r4, r4, #0x12               @ irq mode

  @ Save supervisor mode spsr, lr & sp
  msr     cpsr, r2
  mrs     r5, spsr
  str     r5, [r0], #4
  str     lr, [r0], #4
  str     sp, [r0], #4

  @ Save system mode lr & sp
  msr     cpsr, r3
  str     lr, [r0], #4
  str     sp, [r0], #4

  @ Call generic interrupt handler in irq mode
  msr     cpsr, r4

  @ Call generic interrupt handler, THUMB mode
  ldr     lr, =isr_ret
  ldr     r0, current_thread
  ldr     r1, =isr
  bx      r1

@ -----------------------------------------------------------------------------
@ Return from interrupt
@ -----------------------------------------------------------------------------
  .code 32
isr_ret:
  @ Load pointer to current task state in r0
  ldr     r0, current_thread          @ load pointer to current task state in r0
  add     r0, r0, #80                 @ set pointer to end of current task state

  @ Prepare registers r2-r4 for changing the cpu mode
  mrs     r4, cpsr                    @ get cpu flags
  bic     r4, r4, #0x1f               @ clear state code
  orr     r2, r4, #0x13               @ supervisor mode
  orr     r3, r4, #0x1f               @ system mode
  orr     r4, r4, #0x12               @ irq mode

  @ Restore system mode sp & lr
  msr     cpsr, r3
  ldr     sp, [r0, #-4]!
  ldr     lr, [r0, #-4]!

  @ Restore supervisor mode sp, lr & sprs
  msr     cpsr, r2
  ldr     sp, [r0, #-4]!
  ldr     lr, [r0, #-4]!
  ldr     r5, [r0, #-4]!
  msr     spsr, r5

  @ Restore irq mode sprs
  msr     cpsr, r4
  ldr     r5, [r0, #-4]!
  msr     spsr, r5

  @ Restore r0-r12,r14
  ldmdb   r0!, {r14}                  @ Load r14/pc
  ldmdb   r0!, {r1}                   @ Load r0
  stmfd   sp!, {r1}                   @ Push r0
  ldmdb   r0!, {r1-r12}               @ Restore r1-r12,r14
  ldmfd   sp!, {r0}                   @ Pop r0

  @ Return to task
  subs    pc, lr, #4


  .pool
  .end
