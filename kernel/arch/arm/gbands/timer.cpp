/*
 * Bricks-OS, Operating System for Game Consoles
 * Copyright (C) 2008 Maximus32 <Maximus32@bricks-os.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA
 */


#include "timer.h"
#include "asm/arch/registers.h"
#include "asm/arch/macros.h"


// Max frequencies (Hz)                                     //            GBA    NDS ARM7&9
const float maxTimerFreq0(CPU_FREQUENCY);                   //   ~16780000 Hz  ~33513982 Hz
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
    setTimerFrequency(timerId, 1000000.0f / static_cast<float>(us), reload);
}

// -----------------------------------------------------------------------------
void
resetTimer(int timerId)
{
  REG_TM(timerId) = REG_TM(timerId);
}
