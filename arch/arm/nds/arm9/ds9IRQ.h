#ifndef DS9IRQ_H
#define DS9IRQ_H


#include "kernel/archIRQ.h"


#define DS_IRQ_COUNT 25 // DS supports 25 interrupts max???


//---------------------------------------------------------------------------
struct pt_regs
{
  unsigned long uregs[18];
};

// -----------------------------------------------------------------------------
class CDS9IRQ
 : public IArchIRQ
{
public:
  CDS9IRQ();
  virtual ~CDS9IRQ();

  virtual int init();

  char * getDeviceName();

  void enable (unsigned int irq);
  void disable(unsigned int irq);
  void ack    (unsigned int irq);
  void end    (unsigned int irq);
};


#endif
