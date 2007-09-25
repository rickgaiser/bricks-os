@
@ r0 - r13: General Purpose Registers
@ r13: Also used as stack pointer (SP)
@ r14: Link Register (LR)
@ r15: Program Counter (PC)
@ SPSR: Saved Program Status Register, only available in privileged modes
@
.extern isr

.global local_save_flags
.global local_irq_restore
.global local_irq_disable
.global local_irq_enable
.global runJump
.global __isr
.global current_thread

  .align 4
  .data

current_thread:     .word 0x0               @ pointer to thread that is running
bios_return:        .word 0x0


@ -----------------------------------------------------------------------------
@ unsigned long local_save_flags(void);
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
  mrs r0, cpsr
  bx lr

@ -----------------------------------------------------------------------------
@ void local_irq_restore(unsigned long flags);
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
  msr cpsr, r0
  bx lr

@ -----------------------------------------------------------------------------
@ void local_irq_disable(void);
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
  mrs r0, cpsr
  orr r0, r0, #0xc0
  msr cpsr, r0
  bx lr

@ -----------------------------------------------------------------------------
@ void local_irq_enable(void);
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
  mrs r0, cpsr
  bic r0, r0, #0xc0
  msr cpsr, r0
  bx lr

@ -----------------------------------------------------------------------------
@ void runJump(pt_regs * old_thread, pt_regs * new_thread);
@  - context: THUMB, SYSTEM mode (IRQ mode?)
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
@  b       runJumpReturn

  stmia   r0!, {r4-r11}           @ Save r4-r11
  stmia   r0!, {r0-r3,r12}        @ Save r0-r3,r12
  ldr     r2, =runJumpReturn      @ Task will continue at runJumpReturn
  stmia   r0!, {r2}               @ Save pc
  mrs     r4, cpsr                @ get cprs
  str     r4, [r0], #4            @ save as sprs_irq

  @ Prepare registers r2-r4 for changing the cpu mode
  bic     r4, r4, #0x1f           @ clear state code
  orr     r2, r4, #0x13           @ supervisor mode
  orr     r3, r4, #0x1f           @ system mode
  orr     r4, r4, #0x12           @ irq mode

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

  msr     cpsr, r4                @ Return back to IRQ mode
  str     r1, current_thread      @ Make new_thread the new current_thread
  ldr     r2, bios_return         @ Push BIOS return addr to the stack
  push    {r2}
  b       isr_ret

  nop
runJumpReturn:
  bx      lr


@ ---------------------------------------------------------------------------
@ Upon interrupt the GBA bios is called first:
@   push r0-r3,r12,r14  ; Push to SP_irq
@   call __irq
@   pop  r0-r3,r12,r14  ; Pop from SP_irq
@   ret
@ -----------------------------------------------------------------------------
  .code 32
__isr:
  @ Save bios return address so we can start jumping tasks :-)
  str lr, bios_return

  @ Load pointer to current task state in r0
  ldr r0, current_thread          @ load pointer

  @ Save r4-r11, not saved by the bios
  stmia r0!, {r4-r11}             @ save registers still in the CPU

  @ Get r0-r3,r12,r14 from stack, saved by the bios
  ldmfd sp!, {r4-r9}              @ pop registers pushed by BIOS
  stmia r0!, {r4-r9}              @ save registers pushed by BIOS

  @ Save IRQ mode sprs
  mrs r5, spsr                    @ get the stored CPU state
  str r5, [r0], #4                @ store value for cpsr

  @ Prepare registers r2-r4 for changing the cpu mode
  mrs r4, cpsr                    @ get cpu flags
  bic r4, r4, #0x1f               @ clear state code
  orr r2, r4, #0x13               @ supervisor mode
  orr r3, r4, #0x1f               @ system mode
  orr r4, r4, #0x12               @ irq mode

  @ Save supervisor mode spsr, lr & sp
  msr cpsr, r2
  mrs r5, spsr
  str r5, [r0], #4
  str lr, [r0], #4
  str sp, [r0], #4

  @ Save system mode lr & sp
  msr cpsr, r3
  str lr, [r0], #4
  str sp, [r0], #4

  @ Return back to IRQ mode for interrupt handling
  msr cpsr, r4

  @ Call generic interrupt handler, THUMB mode
  push {lr}
    ldr lr, =isr_ret
    ldr r0, current_thread
    ldr r1, =isr
    bx r1
isr_ret:
  pop {lr}                        @ lr still contains bios return addr

  @ Load pointer to current task state in r0
  ldr r0, current_thread          @ load pointer
  add r0, r0, #80                 @ set pointer to end of registers block

  @ Prepare registers r2-r4 for changing the cpu mode
  mrs r4, cpsr                    @ get cpu flags
  bic r4, r4, #0x1f               @ clear state code
  orr r2, r4, #0x13               @ supervisor mode
  orr r3, r4, #0x1f               @ system mode
  orr r4, r4, #0x12               @ irq mode

  @ Restore system mode sp & lr
  msr cpsr, r3
  ldr sp, [r0, #-4]!
  ldr lr, [r0, #-4]!

  @ Restore supervisor mode sp, lr & sprs
  msr cpsr, r2
  ldr sp, [r0, #-4]!
  ldr lr, [r0, #-4]!
  ldr r5, [r0, #-4]!
  msr spsr, r5

  @ Restore IRQ mode sprs, the tasks cprs
  msr cpsr, r4
  ldr r5, [r0, #-4]!
  msr spsr, r5

  @ Push r0-r3,r12,r14 on the stack, BIOS will restore them
  ldmdb   r0!, {r4-r9}
  stmfd   sp!, {r4-r9}

  @ Restore r4-r11
  ldmdb   r0!, {r4-r11}

  @ Return to bios
  bx      lr


  .pool
  .end
