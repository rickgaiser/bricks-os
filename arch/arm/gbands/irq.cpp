#include "kernel/interruptManager.h"
#include "kernel/task.h"
#include "asm/irq.h"
#include "iostream"



// Assembler isr function calling our "C" isr function
extern "C" void __isr();
extern pt_regs * current_thread;


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
extern "C" void
schedule()
{
//  std::cout<<'.';
//  uint32_t * pRegs = (uint32_t *)current_thread;
//  for(int i(13); i < 20; i++)
//    std::cout<<pRegs[i]<<" - ";
//  std::cout<<std::endl;

  CTask::pCurrentTask_ = CTask::pCurrentTask_->next;
  while(CTask::pCurrentTask_->eState_ != TS_RUNNING)
    CTask::pCurrentTask_ = CTask::pCurrentTask_->next;

  current_thread = CTask::pCurrentTask_->pTaskState_;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
extern "C" void
isr(pt_regs * regs)
{
//  std::cout<<"isr("<<(uint32_t)regs<<')'<<std::endl;

  // Find out who triggered the interrupt
  unsigned long iFlags(REG_IF & REG_IE);

  // Handle timer interrupt for scheduler
  if(iFlags & (1 << 3))
  {
    // Remove flag
    iFlags &= ~(1 << 3);
    // Acknowledge interrupt
    REG_IF |= (1 << 3);
    // Run scheduler
    schedule();
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
