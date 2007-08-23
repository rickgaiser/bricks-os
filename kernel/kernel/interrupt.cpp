#include "kernel/interrupt.h"


// -----------------------------------------------------------------------------
CInterrupt::CInterrupt()
{
  // Clear all observer fields
  for(int i(0); i != MAX_OBSERVERS; i++)
  {
    observers_[i] = 0;
  }
}

// -----------------------------------------------------------------------------
CInterrupt::~CInterrupt()
{
}

// -----------------------------------------------------------------------------
void
CInterrupt::attach(IInterruptServiceRoutine * isr)
{
  // Find an empty observer slot and put new isr in it
  for(int i(0); i != MAX_OBSERVERS; i++)
  {
    if(observers_[i] == 0)
    {
      // Found an empty slot
      observers_[i] = isr;
      break;
    }
  }
}

// -----------------------------------------------------------------------------
void
CInterrupt::detach(IInterruptServiceRoutine * isr)
{
  // Find isr slot and delete it
  for(int i(0); i != MAX_OBSERVERS; i++)
  {
    if(observers_[i] == isr)
    {
      // Found isr slot, delete it
      observers_[i] = 0;
    }
  }
}

// -----------------------------------------------------------------------------
int
CInterrupt::isrCount()
{
  int iRetVal(0);
  
  // Count observers
  for(int i(0); i != MAX_OBSERVERS; i++)
    if(observers_[i] != 0)
      iRetVal++;

  return(iRetVal);
}

// -----------------------------------------------------------------------------
int
CInterrupt::handle(int irq)
{
  // Give all observers a chance to handle the interrupt
  for(int i(0); i != MAX_OBSERVERS; i++)
  {
    if(observers_[i] != 0)
    {
      // Found observer, let him handle the interrupt
      observers_[i]->isr(irq);
    }
  }

  return(0);
}
