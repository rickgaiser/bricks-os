#ifndef GBAIRQ_H
#define GBAIRQ_H


#include "kernel/archIRQ.h"
#include "hal.h"
#include "gba.h"


#define GBA_IRQ_COUNT 14 // GBA supports 14 interrupts


//---------------------------------------------------------------------------
struct pt_regs
{
  unsigned long uregs[18];
};

// -----------------------------------------------------------------------------
class CGBAIRQ
 : public IArchIRQ
{
public:
  CGBAIRQ();
  virtual ~CGBAIRQ();

  virtual int init();

  char * getDeviceName();

  void enable (unsigned int irq);
  void disable(unsigned int irq);
  void ack    (unsigned int irq);
  void end    (unsigned int irq);
};


#endif
