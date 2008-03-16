#include "kernel/debug.h"
#include "kernel/interruptManager.h"
#include "kernel/task.h"
#include "task.h"
#include "asm/irq.h"



// Assembler isr function calling our "C" isr function
extern "C" void __isr();


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
extern "C" void
isr(pt_regs * regs)
{
  // Find out who triggered the interrupt
  unsigned long iFlags(REG_IF & REG_IE);

  //printk("isr(0x%x)\n", iFlags);
  //printk(".");

  // Handle timer interrupt for scheduler
  if(iFlags & (1 << 5))
  {
    // Remove flag
    iFlags &= ~(1 << 5);
    // Acknowledge interrupt
    REG_IF |= (1 << 5);
    // Task/Thread stuff
    CTaskManager::updateSleepers();
    CTaskManager::removeDestroyed();
    if(CTaskManager::schedule() == true)
      CTaskManager::pCurrentThread_->runReturn();
  }

  // Handle other interrupts
  if(iFlags != 0)
  {
    unsigned long iIntFlag(0x0001);
    for(int i(0); i < MAX_INTERRUPTS; i++)
    {
      if((iFlags & iIntFlag) != 0)
      {
        // Interrupt found! Handle it!
        CInterruptManager::isr(i, 0);
        break;
      }
      iIntFlag <<= 1;
    }
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
    CInterruptManager::detach(i, this);
}

// -----------------------------------------------------------------------------
int
CIRQ::init()
{
  REG_INTMAIN = &::__isr;

  for(int i(0); i < MAX_INTERRUPTS; i++)
    CInterruptManager::attach(i, this);

  return 0;
}
