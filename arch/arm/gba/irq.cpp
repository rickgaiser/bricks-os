#include "kernel/interruptManager.h"
#include "asm/irq.h"
#include "asm/arch.h"


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
  for(int i(0); i < MAX_INTERRUPTS; i++)
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
CIRQ::CIRQ()
{
  // Don't use constructor, use init function instead
}

// -----------------------------------------------------------------------------
CIRQ::~CIRQ()
{
  for(int i(0); i < MAX_INTERRUPTS; i++)
    CInterruptManager::instance()->detach(i, this);
}

// -----------------------------------------------------------------------------
int
CIRQ::init()
{
  REG_INTMAIN = &::__isr;

  for(int i(0); i < MAX_INTERRUPTS; i++)
    CInterruptManager::instance()->attach(i, this);

  return(0);
}
