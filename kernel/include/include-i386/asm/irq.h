#ifndef I386IRQ_H
#define I386IRQ_H


#include "inttypes.h"


#define MAX_INTERRUPTS 0x31


// -----------------------------------------------------------------------------
struct pt_regs
{
  // pushed by pusha
  unsigned long edi, esi, ebp, esp, ebx, edx, ecx, eax;
  // pushed separately
  unsigned long ss, ds, es, fs, gs;
  unsigned long iIntNumber, iErrorCode;
  // pushed by exception. Exception may also push err_code.
  // user_esp and user_ss are pushed only if a privilege change occurs.
  unsigned long eip, cs, eflags, user_esp, user_ss;
};

// -----------------------------------------------------------------------------
inline void
taskInit(pt_regs * regs, void * pc, void * sp, void * svcsp, uint32_t arg1, uint32_t arg2)
{
}

// -----------------------------------------------------------------------------
class CIRQ
{
public:
  CIRQ();
  virtual ~CIRQ();

  int init();

  void enable (unsigned int irq);
  void disable(unsigned int irq);
  void ack    (unsigned int irq);
  void end    (unsigned int irq);

private:
  unsigned char iMaskMaster_;
  unsigned char iMaskSlave_;
};


#endif
