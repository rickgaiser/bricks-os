#include "kernel/debug.h"
#include "kernel/interruptManager.h"
#include "kernel/task.h"
#include "kernel/srr_k.h"
#include "asm/irq.h"
#include "asm/arch/memory.h"
#include "asm/arch/config.h"


// Assembler isr function calling our "C" isr function
//extern "C" void __isr();
extern "C" void __gba_isr();
// Function __gba_isr calls
extern "C" void isr(pt_regs * regs) INTERRUPT_CODE;


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
      CTaskManager::pCurrentThread_->impl().runReturn();
  }

  // Handle other interrupts
  if(iFlags != 0)
  {
    for(int i(0); i < MAX_INTERRUPTS; i++)
    {
      if(iFlags & (1 << i))
      {
        // Interrupt found! Handle it!
        CInterruptManager::isr(i, 0);
      }
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
  // Initialize function pointers
  REG_INTMAIN    = &::__gba_isr;
#ifdef GBA
#ifndef CONFIG_DIRECT_ACCESS_KERNEL
  REG_MSGSEND    = &::k_msgSend;
  REG_MSGRECEIVE = &::k_msgReceive;
  REG_MSGREPLY   = &::k_msgReply;
#endif // CONFIG_DIRECT_ACCESS_KERNEL
#endif // GBA

  for(int i(0); i < MAX_INTERRUPTS; i++)
    CInterruptManager::attach(i, this);

  return 0;
}
