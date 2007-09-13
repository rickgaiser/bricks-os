#include "kernel/debug.h"
#include "kernel/interruptManager.h"
#include "asm/cpu.h"


// -----------------------------------------------------------------------------
// Static data members
CInterrupt   CInterruptManager::interrupt_[MAX_INTERRUPTS];


// -----------------------------------------------------------------------------
void
CInterruptManager::attach(unsigned int irq, IInterruptServiceRoutine * isr)
{
  if(irq < MAX_INTERRUPTS)
  {
    interrupt_[irq].attach(isr);
    // If this is the first ISR, enable the interrupt
    if(interrupt_[irq].isrCount() == 1)
    {
      interrupt_[irq].hardware_->enable(irq);
    }
  }
}

// -----------------------------------------------------------------------------
void
CInterruptManager::detach(unsigned int irq, IInterruptServiceRoutine * isr)
{
  if(irq < MAX_INTERRUPTS)
  {
    interrupt_[irq].detach(isr);
    // If this is the last ISR, disable the interrupt
    if(interrupt_[irq].isrCount() == 0)
    {
      interrupt_[irq].hardware_->disable(irq);
    }
  }
}

// -----------------------------------------------------------------------------
void
CInterruptManager::attach(unsigned int irq, CIRQ * irqhardware)
{
  if(irq < MAX_INTERRUPTS)
    interrupt_[irq].hardware_ = irqhardware;
}

// -----------------------------------------------------------------------------
void
CInterruptManager::detach(unsigned int irq, CIRQ * irqhardware)
{
  if(irq < MAX_INTERRUPTS)
    interrupt_[irq].hardware_ = 0;
}

// -----------------------------------------------------------------------------
unsigned int
CInterruptManager::isr(unsigned int irq, pt_regs * regs)
{
  unsigned long flags;

  // First disable interrupts
  flags = local_save_flags();
  local_irq_disable();

  if(irq < MAX_INTERRUPTS)
  {
    if(interrupt_[irq].isrCount() != 0)
    {
      interrupt_[irq].handle(irq);
    }
    else
    {
      printk("CInterruptManager::isr: ERROR: Unhandled interrupt(int=%d)\n", (int)irq);
    }
    interrupt_[irq].hardware_->ack(irq);
  }
  else
  {
    printk("CInterruptManager::isr: ERROR: Interrupt out of range(int=%d)\n", (int)irq);
  }

  // Restore interrupt and return
  local_irq_restore(flags);

  return 0;
}
