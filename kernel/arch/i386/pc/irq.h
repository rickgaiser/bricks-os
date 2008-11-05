#ifndef PC_IRQ_H
#define PC_IRQ_H


#include "kernel/interruptManager.h"


// -----------------------------------------------------------------------------
class CIRQ
 : public IInterruptProvider
{
public:
  CIRQ();
  virtual ~CIRQ();

  int init();

  void enable (unsigned int irq);
  void disable(unsigned int irq);
  void ack    (unsigned int irq);

private:
  unsigned char iMaskMaster_;
  unsigned char iMaskSlave_;
};


#endif
