#ifndef ASM_IRQ_H
#define ASM_IRQ_H


#include "inttypes.h"
#include "asm/arch/registers.h"


#define INT_GS            0
#define INT_SBUS          1
#define INT_VBLANK_START  2
#define INT_VBLANK_END    3
#define INT_VIF0          4
#define INT_VIF1          5
#define INT_VU0           6
#define INT_VU1           7
#define INT_IPU           8
#define INT_TIMER0        9
#define INT_TIMER1       10

#define MAX_INTERRUPTS 0x0b


//---------------------------------------------------------------------------
struct pt_regs
{
  // 32 128-bit General Purpose Registers
  uint128_t cpu_r00;
  uint128_t cpu_r01;
  uint128_t cpu_r02;
  uint128_t cpu_r03;
  uint128_t cpu_r04;
  uint128_t cpu_r05;
  uint128_t cpu_r06;
  uint128_t cpu_r07;
  uint128_t cpu_r08;
  uint128_t cpu_r09;
  uint128_t cpu_r10;
  uint128_t cpu_r11;
  uint128_t cpu_r12;
  uint128_t cpu_r13;
  uint128_t cpu_r14;
  uint128_t cpu_r15;
  uint128_t cpu_r16;
  uint128_t cpu_r17;
  uint128_t cpu_r18;
  uint128_t cpu_r19;
  uint128_t cpu_r20;
  uint128_t cpu_r21;
  uint128_t cpu_r22;
  uint128_t cpu_r23;
  uint128_t cpu_r24;
  uint128_t cpu_r25;
  uint128_t cpu_r26;
  uint128_t cpu_r27;
  uint128_t cpu_r28;
  uint128_t cpu_r29;
  uint128_t cpu_r30;
  uint128_t cpu_r31;

  uint64_t fpu_r00;
  uint64_t fpu_r01;
  uint64_t fpu_r02;
  uint64_t fpu_r03;
  uint64_t fpu_r04;
  uint64_t fpu_r05;
  uint64_t fpu_r06;
  uint64_t fpu_r07;
  uint64_t fpu_r08;
  uint64_t fpu_r09;
  uint64_t fpu_r10;
  uint64_t fpu_r11;
  uint64_t fpu_r12;
  uint64_t fpu_r13;
  uint64_t fpu_r14;
  uint64_t fpu_r15;
  uint64_t fpu_r16;
  uint64_t fpu_r17;
  uint64_t fpu_r18;
  uint64_t fpu_r19;
  uint64_t fpu_r20;
  uint64_t fpu_r21;
  uint64_t fpu_r22;
  uint64_t fpu_r23;
  uint64_t fpu_r24;
  uint64_t fpu_r25;
  uint64_t fpu_r26;
  uint64_t fpu_r27;
  uint64_t fpu_r28;
  uint64_t fpu_r29;
  uint64_t fpu_r30;
  uint64_t fpu_r31;

  uint32_t fp_acc;
  uint32_t reserved;
  uint32_t t0;
  uint32_t t1;
  uint32_t t2;
  uint32_t t3;
  uint32_t t4;
  uint32_t fpu_status;
  uint32_t epc;
};

//---------------------------------------------------------------------------
#define MIPS_INT_0 0
#define MIPS_INT_1 1
#define MIPS_INT_2 2
class IInterruptHandler
{
public:
  virtual ~IInterruptHandler(){}

  virtual void isr(unsigned int irq, pt_regs * regs) = 0;
};
void initExceptions();
void setInterruptHandler(uint32_t nr, IInterruptHandler & handler);

// -----------------------------------------------------------------------------
class CIRQ
 : public IInterruptHandler
{
public:
  CIRQ();
  virtual ~CIRQ();

  int init();

  void isr(unsigned int irq, pt_regs * regs);

  void enable (unsigned int irq);
  void disable(unsigned int irq);
  void ack    (unsigned int irq);
  inline void end    (unsigned int irq){}

private:
  uint32_t iINTMask_;
};


#endif
