.extern isr

.global __isr
.global current_thread

.align 4
@.code 32
.data
.arm

__isr:
  @ Save task state
  ldr r0, current_thread          @ load pointer

  stmia r0!, {r4-r11}             @ save registers still in the CPU
  ldmfd sp!, {r4-r9}              @ pop registers pushed by BIOS
  stmia r0!, {r4-r9}              @ save registers pushed by BIOS

  mrs r5, spsr                    @ get the stored CPU state
  str r5, [r0], #4                @ store value for cpsr

  @ r2, cpsr flags with CPU supervisor mode
  @ r3, cpsr flags with CPU system mode
  @ r4, cpsr flags with CPU IRQ mode
  mrs r4, cpsr                    @ get cpu flags
  bic r4, r4, #0x1f               @ clear state code
  orr r2, r4, #0x13               @ supervisor mode
  orr r3, r4, #0x1f               @ system mode
  orr r4, r4, #0x12               @ irq mode

  msr cpsr, r2                    @ put CPU in supervisor mode
  mrs r5, spsr                    @ store value of spsr_svc
  str r5, [r0], #4
  str lr, [r0], #4                @ store lr_svc
  str sp, [r0], #4                @ store sp_svc
  msr cpsr, r3                    @ put CPU in system mode
  str lr, [r0], #4                @ store lr
  str sp, [r0], #4                @ store sp

  msr cpsr, r4                    @ go back to IRQ mode for scheduling

  @ Call generic interrupt handler
  push {r0-r1, lr}
    ldr lr, =isr_ret
    ldr r0, current_thread
    ldr r1, =isr
    bx r1
isr_ret:
  pop {r0-r1, lr}

  @ Restore task state
  ldr r0, current_thread          @ load pointer
  add r0, r0, #80                 @ set pointer to end of registers block

  @ r2, cpsr flags with CPU supervisor mode
  @ r3, cpsr flags with CPU system mode
  @ r4, cpsr flags with CPU IRQ mode
  mrs r4, cpsr                    @ get cpu flags
  bic r4, r4, #0x1f               @ clear state code
  orr r2, r4, #0x13               @ supervisor mode
  orr r3, r4, #0x1f               @ system mode
  orr r4, r4, #0x12               @ irq mode

  msr cpsr, r3                    @ put CPU in system mode to get lr and sp
  ldr sp, [r0, #-4]!              @ restore sp
  ldr lr, [r0, #-4]!              @ restore lr
  msr cpsr, r2                    @ put CPU in supervisor mode
  ldr sp, [r0, #-4]!              @ restore sp_svc
  ldr lr, [r0, #-4]!              @ restore lr_svc
  ldr r5, [r0, #-4]!              @ restore spsr_svc
  msr spsr, r5
  msr cpsr, r4                    @ banked registers done, restore IRQ mode

  ldr r5, [r0, #-4]!              @ get bits for CPSR
  msr spsr, r5                    @ cpsr = spsr_irq when interrupt returns

  ldmdb   r0!, {r4-r9}            @ load values for r0-r3, r12, r14
  stmfd   sp!, {r4-r9}            @ push values; BIOS will restore for us
  ldmdb   r0!, {r4-r11}           @ restore general registers

  @ Return to bios
  bx      lr

.align 4
.data

current_thread:     .word 0x0               @ pointer to thread that is running

.pool
