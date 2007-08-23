#include "kernel/interruptManager.h"
#include "asm/cpu.h"
#include "iostream"


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
  // First disable interrupts
  CCPU::cli();

  if(irq < MAX_INTERRUPTS)
  {
    if(interrupt_[irq].isrCount() != 0)
    {
      interrupt_[irq].handle(irq);
    }
    else
    {
      std::cout<<"CInterruptManager::isr: ERROR: Unhandled interrupt(int="<<(int)irq<<")"<<std::endl;
    }
    interrupt_[irq].hardware_->ack(irq);
  }
  else
  {
    std::cout<<"CInterruptManager::isr: ERROR: Interrupt out of range(int="<<(int)irq<<")"<<std::endl;
  }

  // Enable interrupt again and return
  CCPU::sti();

  return 0;
}
