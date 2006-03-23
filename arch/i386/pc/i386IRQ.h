#ifndef I386IRQ_H
#define I386IRQ_H


#include "kernel/archIRQ.h"


#define IRQ_COUNT 16


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
class CI386IRQ
 : public IArchIRQ
{
public:
  CI386IRQ();
  virtual ~CI386IRQ();

  virtual int init();

  char * getDeviceName();

  void enable (unsigned int irq);
  void disable(unsigned int irq);
  void ack    (unsigned int irq);
  void end    (unsigned int irq);

private:
  unsigned char iMaskMaster_;
  unsigned char iMaskSlave_;
};


#endif
