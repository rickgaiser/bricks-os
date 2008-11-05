#ifndef ASM_IRQ_H
#define ASM_IRQ_H


#include "inttypes.h"
#include "asm/arch/irq.h"


//---------------------------------------------------------------------------
//#define sp r13
//#define lr r14
//#define pc r15
struct pt_regs
{
  // System/User mode state
  uint32_t r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r0;
  uint32_t pc, cpsr, lr, sp;
};

#define CPU_MODE_USER        0x00000010
#define CPU_MODE_FIQ         0x00000011
#define CPU_MODE_IRQ         0x00000012
#define CPU_MODE_SUPERVISOR  0x00000013
#define CPU_MODE_ABORT       0x00000017
#define CPU_MODE_UNDEFINED   0x0000001b
#define CPU_MODE_SYSTEM      0x0000001f

#define CPU_MODE_THUMB       0x00000020
#define CPU_MODE_ARM         0x00000000

#define CPU_FIQ_DISABLE      0x00000040
#define CPU_FIQ_ENABLE       0x00000000

#define CPU_IRQ_DISABLE      0x00000080
#define CPU_IRQ_ENABLE       0x00000000


#endif
