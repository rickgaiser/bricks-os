#include "i8254.h"
#include "kernel/interruptManager.h"
#include "asm/hal.h"


const float maxTimerFreq(I8254_FREQUENCY);


// -----------------------------------------------------------------------------
CI8254::CI8254(uint16_t baseAddr)
 : iBaseAddr_(baseAddr)
{
}

// -----------------------------------------------------------------------------
CI8254::~CI8254()
{
}

// -----------------------------------------------------------------------------
int
CI8254::init()
{
  // We don't actually "get" the interrupt, but this does enable the it.
  CInterruptManager::attach(0x20, this); // Timer

  return 0;
}

// -----------------------------------------------------------------------------
void
CI8254::setTimerFrequency(float hz)
{
  uint16_t iCounterTimeout = (maxTimerFreq / hz) + 0.5f;

  outb((iCounterTimeout     ) & 0x00ff, iBaseAddr_ + I8254_COUNTER0_OFF);
  outb((iCounterTimeout >> 8) & 0x00ff, iBaseAddr_ + I8254_COUNTER0_OFF);
}

// -----------------------------------------------------------------------------
int
CI8254::isr(int irq)
{
  return 0;
}
