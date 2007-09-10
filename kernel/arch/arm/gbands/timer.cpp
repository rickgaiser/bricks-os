#include "timer.h"
#include "asm/arch/registers.h"
#include "asm/arch/macros.h"


// Max frequencies (Hz)
const float maxTimerFreq0(CPU_FREQUENCY);                   //   ~16780000 Hz
const float maxTimerFreq1(maxTimerFreq0 /   64.0f);         //     ~262188 Hz
const float maxTimerFreq2(maxTimerFreq0 /  256.0f);         //      ~65547 Hz
const float maxTimerFreq3(maxTimerFreq0 / 1024.0f);         //      ~16386 Hz

// Min frequencies (Hz)
const float minTimerFreq0(maxTimerFreq0 / 0x10000);         //        ~256 Hz
const float minTimerFreq1(minTimerFreq0 /   64.0f);         //          ~4 Hz
const float minTimerFreq2(minTimerFreq0 /  256.0f);         //          ~1 Hz
const float minTimerFreq3(minTimerFreq0 / 1024.0f);         //       ~0.25 Hz


// -----------------------------------------------------------------------------
void
setTimerFrequency(int timerId, float freq, bool reload)
{
  unsigned int iCount;
  unsigned int iCTRL;

  if(freq >= minTimerFreq0) // 256Hz < 17MHz
  {
    iCount = static_cast<unsigned int>(maxTimerFreq0 / freq);
    iCTRL  = TIMER_ENABLE | TIMER_IRQ | TIMER_SPEED_0;
  }
  else if(freq >= minTimerFreq1) // 4Hz < 262KHz
  {
    iCount = static_cast<unsigned int>(maxTimerFreq1 / freq);
    iCTRL  = TIMER_ENABLE | TIMER_IRQ | TIMER_SPEED_1;
  }
  else if(freq >= minTimerFreq2) // 1Hz < 66KHz
  {
    iCount = static_cast<unsigned int>(maxTimerFreq2 / freq);
    iCTRL  = TIMER_ENABLE | TIMER_IRQ | TIMER_SPEED_2;
  }
  else // 0.25Hz < 16KHz
  {
    iCount = static_cast<unsigned int>(maxTimerFreq3 / freq);
    iCTRL  = TIMER_ENABLE | TIMER_IRQ | TIMER_SPEED_3;
  }

  iCount = 0x10000 - (iCount <= 0xffff ? iCount : 0xffff);
  if(reload == true)
  {
    // Writing the 32bit value at once will reload the current counter value
    REG_TM(timerId) = iCount | ((iCTRL) << 16);
  }
  else
  {
    // Writing the registers separately will keep the current counter value
    REG_TMD(timerId)   = iCount;
    REG_TMCNT(timerId) = iCTRL;
  }
}

// -----------------------------------------------------------------------------
void
setTimerInterval(int timerId, unsigned int us, bool reload)
{
  if(us != 0)
    setFrequency(timerId, 1000000.0f / static_cast<float>(us), reload);
}

// -----------------------------------------------------------------------------
void
resetTimer(int timerId)
{
  REG_TM(timerId) = REG_TM(timerId);
}


};
