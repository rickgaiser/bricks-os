#include "kernel/interruptManager.h"
#include "ds9IRQ.h"
#include "nds/interrupts.h"


// Assembler isr function calling our "C" isr function
extern "C" void __isr();


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
extern "C" void
isr()
{
  // Find out who triggered the interrupt
  unsigned long iFlags(REG_IF & REG_IE);
  unsigned long iIntFlag(0x0001);
  for(int i(0); i < DS_IRQ_COUNT; i++)
  {
    if((iFlags & iIntFlag) != 0)
    {
      // Handle interrupt
      CInterruptManager::instance()->isr(i, 0);

      break;
    }
    iIntFlag <<= 1;
  }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
CDS9IRQ::CDS9IRQ()
{
  // Don't use constructor, use init function instead
}

// -----------------------------------------------------------------------------
CDS9IRQ::~CDS9IRQ()
{
  for(int i(0); i < DS_IRQ_COUNT; i++)
    CInterruptManager::instance()->detach(i, this);
}

// -----------------------------------------------------------------------------
int
CDS9IRQ::init()
{
  IRQ_HANDLER = &::__isr;

  for(int i(0); i < DS_IRQ_COUNT; i++)
    CInterruptManager::instance()->attach(i, this);

  return(0);
}

// -----------------------------------------------------------------------------
char *
CDS9IRQ::getDeviceName()
{
  return("DS-IRQ");
}

// -----------------------------------------------------------------------------
void
CDS9IRQ::enable(unsigned int irq)
{
//  HAL::cli();
  // Set bit in IE(Interrupt Enable) register
  REG_IE |= (1 << irq);
//  HAL::sti();
}

// -----------------------------------------------------------------------------
void
CDS9IRQ::disable(unsigned int irq)
{
//  HAL::cli();
  // Clear bit in IE(Interrupt Enable) register
  REG_IE &= ~(1 << irq);
//  HAL::sti();
}

// -----------------------------------------------------------------------------
void
CDS9IRQ::ack(unsigned int irq)
{
  // ACK irq by writing irq bit to reg IF(Interrupt request Flags)
  REG_IF |= (1 << irq);
}

// -----------------------------------------------------------------------------
void
CDS9IRQ::end(unsigned int irq)
{
}
