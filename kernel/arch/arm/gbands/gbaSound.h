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
 *
 * Based on Deku's gba-mod tutorial: http://deku.gbadev.org/
 */


#ifndef ARCH_GBASOUND_H
#define ARCH_GBASOUND_H


#include "kernel/interrupt.h"
#include "asm/arch/memory.h"


#define SOUND_CHANNEL_COUNT  4


// -----------------------------------------------------------------------------
struct SSoundChannel
{
  const int8_t * data;
  uint32_t pos;
  uint32_t inc;
  uint32_t vol;
  uint32_t length;
  uint32_t loopLength;
};

// -----------------------------------------------------------------------------
struct SSoundBuffer
{
  int8_t   * mixBufferBase;  // Pointer to entire mix buffer (2 buffers)
  int8_t   * curMixBuffer;   // Pointer to current mix buffer
  uint32_t   mixBufferSize;  // Size of 1 mix buffer
  uint16_t   mixFreq;        // Output frequency
  uint8_t    activeBuffer;   // The current active buffer (0 or 1)
};

// -----------------------------------------------------------------------------
class CGBASound
 : public IInterruptServiceRoutine
{
public:
  CGBASound();
  virtual ~CGBASound();

  int init();

  void setChannel(uint8_t nr, SSoundChannel * data);
  void mix(uint32_t samplesToMix) FAST_CODE;

  // Inherited from IInterruptServiceRoutine
  virtual int isr(int irq) INTERRUPT_CODE;

private:
  // Input channels
  SSoundChannel * channel_[SOUND_CHANNEL_COUNT];

  // Hardware output channels
  SSoundBuffer buffera_;
  //SSoundBuffer bufferb_;
  int8_t bufferaData_[736*2];
  //int8_t bufferbData_[736*2];
};


#endif
