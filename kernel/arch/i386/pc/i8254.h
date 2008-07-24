// Class for handling Intel 8254 Programmable Interval Timer (PIT)
// Typically located @ 0x40
#ifndef I8254_H
#define I8254_H


#include "kernel/interrupt.h"


#define I8254_FREQUENCY 1193182 // ~1.2 MHz
#define I8254_COUNTER0_OFF 0
#define I8254_COUNTER1_OFF 1
#define I8254_COUNTER2_OFF 2
#define I8254_CONTROL_OFF 0


class CI8254
 : public IInterruptServiceRoutine
{
public:
  CI8254(uint16_t baseAddr);
  ~CI8254();

  int init();
  void setTimerFrequency(float hz);

  // Inherited from IInterruptServiceRoutine
  virtual int isr(int irq);

private:
  uint16_t iBaseAddr_;
};


#endif
