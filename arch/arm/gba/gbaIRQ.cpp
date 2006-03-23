#include "kernel/interruptManager.h"
#include "gbaIRQ.h"
#include "hal.h"

#include "iostream"


// Assembler isr function calling our "C" isr function
extern "C" void __isr();


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
extern "C" void
isr()
{
  // Find out who triggered the interrupt
  unsigned short iFlags(REG_IF & REG_IE);
  int iIntFlag(0x0001);
  for(int i(0); i != 14; i++)
  {
    if((iFlags & iIntFlag) != 0)
    {
      // Interrupt found! Handle it!
      CInterruptManager::instance()->isr(i, 0);
      break;
    }
    iIntFlag <<= 1;
  }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
CGBAIRQ::CGBAIRQ()
{
  // Don't use constructor, use init function instead
}

// -----------------------------------------------------------------------------
CGBAIRQ::~CGBAIRQ()
{
  for(int i(0); i != GBA_IRQ_COUNT; i++)
    CInterruptManager::instance()->detach(i, this);
}

// -----------------------------------------------------------------------------
int
CGBAIRQ::init()
{
  REG_INTMAIN = &::__isr;

  for(int i(0); i != GBA_IRQ_COUNT; i++)
    CInterruptManager::instance()->attach(i, this);

  return(0);
}

// -----------------------------------------------------------------------------
char *
CGBAIRQ::getDeviceName()
{
  return("GBA-IRQ");
}

// -----------------------------------------------------------------------------
void
CGBAIRQ::enable(unsigned int irq)
{
//  HAL::cli();
  // Set bit in IE(Interrupt Enable) register
  REG_IE |= (1 << irq);
//  HAL::sti();
}

// -----------------------------------------------------------------------------
void
CGBAIRQ::disable(unsigned int irq)
{
//  HAL::cli();
  // Clear bit in IE(Interrupt Enable) register
  REG_IE &= (1 << irq);
//  HAL::sti();
}

// -----------------------------------------------------------------------------
void
CGBAIRQ::ack(unsigned int irq)
{
  // ACK irq by writing irq bit to reg IF(Interrupt request Flags)
  REG_IF |= (1 << irq);
}

// -----------------------------------------------------------------------------
void
CGBAIRQ::end(unsigned int irq)
{
}
