#include "timer.h"
#include "asm/arch/registers.h"
#include "asm/arch/macros.h"


// -----------------------------------------------------------------------------
void
setTimerFrequency(int timerId, float freq)
{
                                                               //       -GBA-
  const static float maxTimerFreq0(CPU_FREQUENCY);             // ~16780000Hz
  const static float maxTimerFreq1(CPU_FREQUENCY /   64.0f);   //   ~262188Hz
  const static float maxTimerFreq2(CPU_FREQUENCY /  256.0f);   //    ~65547Hz
  const static float maxTimerFreq3(CPU_FREQUENCY / 1024.0f);   //    ~16386Hz
  const static float minTimerFreq0(maxTimerFreq0 / 0x10000);   //      ~256Hz
  const static float minTimerFreq1(maxTimerFreq1 / 0x10000);   //        ~4Hz
  const static float minTimerFreq2(maxTimerFreq2 / 0x10000);   //        ~1Hz
//  const static float minTimerFreq3(maxTimerFreq3 / 0x10000);   //   ~0.25Hz

  if(freq >= minTimerFreq0)
  {
    REG_TMD(timerId)   = 0x10000 - static_cast<unsigned short>(maxTimerFreq0 / freq);
    REG_TMCNT(timerId) = TIMER_ENABLE | TIMER_IRQ | TIMER_SPEED_0;
  }
  else if(freq >= minTimerFreq1)
  {
    REG_TMD(timerId)   = 0x10000 - static_cast<unsigned short>(maxTimerFreq1 / freq);
    REG_TMCNT(timerId) = TIMER_ENABLE | TIMER_IRQ | TIMER_SPEED_1;
  }
  else if(freq >= minTimerFreq2)
  {
    REG_TMD(timerId)   = 0x10000 - static_cast<unsigned short>(maxTimerFreq2 / freq);
    REG_TMCNT(timerId) = TIMER_ENABLE | TIMER_IRQ | TIMER_SPEED_2;
  }
  else
  {
    REG_TMD(timerId)   = 0x10000 - static_cast<unsigned short>(maxTimerFreq3 / freq);
    REG_TMCNT(timerId) = TIMER_ENABLE | TIMER_IRQ | TIMER_SPEED_3;
  }
}

// -----------------------------------------------------------------------------
void
setInterval(int timerId, unsigned int ms)
{
  if(ms != 0)
    setTimerFrequency(timerId, 1000.0f / static_cast<float>(ms));
}
