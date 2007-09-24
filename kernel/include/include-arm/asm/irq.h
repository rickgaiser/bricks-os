#ifndef ASM_IRQ_H
#define ASM_IRQ_H


#include "inttypes.h"
#include "asm/arch/irq.h"
#include "asm/arch/registers.h"


extern "C" void kill();


//---------------------------------------------------------------------------
// CPU modes      Banked regs       Used by
//
// System/User    -                 bios
// FIQ            r8 -r14, sprs     -
// Supervisor     r13-r14, sprs     apps
// Abort          r13-r14, sprs     -
// IRQ            r13-r14, sprs     interrupts

//---------------------------------------------------------------------------
// On interrupt:
//  - CPU:  set lr_irq & spsr_irq, goto irq mode, goto bios irq handler
//  - BIOS: push r0-r3, r12, lr onto (irq)stack
//  - BIOS: set lr to bios return addr
//  - jump to handler
//#define sp r13
//#define lr r14
//#define pc r15
struct pt_regs
{
  // Tasks Registers
  uint32_t r4, r5, r6, r7, r8, r9, r10, r11;
  // Tasks registers (saved by bios)
  uint32_t r0, r1, r2, r3, r12, pc;
  // IRQ state (banked regs), as we get called by bios
  uint32_t cpsr;
  // Supervisor state (banked regs), as the task was when it got interrupted
  uint32_t spsr_svc, lr_svc, sp_svc;
  // System/User state (banked regs), used by bios
  uint32_t lr, sp;
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


// -----------------------------------------------------------------------------
class CIRQ
{
public:
  CIRQ();
  virtual ~CIRQ();

  int init();

  inline void enable (unsigned int irq){REG_IE |= (1 << irq);}
  inline void disable(unsigned int irq){REG_IE &= ~(1 << irq);}
  inline void ack    (unsigned int irq){REG_IF |= (1 << irq);}
  inline void end    (unsigned int irq){}
};


#endif
