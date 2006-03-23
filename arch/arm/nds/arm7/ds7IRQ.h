#ifndef DS7IRQ_H
#define DS7IRQ_H


#include "kernel/archIRQ.h"


#define DS7_IRQ_COUNT 25 // DS7 supports 25 interrupts max???


//---------------------------------------------------------------------------
struct pt_regs
{
  unsigned long uregs[18];
};

// -----------------------------------------------------------------------------
class CDS7IRQ
 : public IArchIRQ
{
public:
  CDS7IRQ();
  virtual ~CDS7IRQ();

  virtual int init();

  char * getDeviceName();

  void enable (unsigned int irq);
  void disable(unsigned int irq);
  void ack    (unsigned int irq);
  void end    (unsigned int irq);
};


#endif
