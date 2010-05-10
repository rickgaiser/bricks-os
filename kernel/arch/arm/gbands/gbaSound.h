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
 */


#ifndef ARCH_GBASOUND_H
#define ARCH_GBASOUND_H


#include "kernel/interrupt.h"
#include "kernel/pthread_k.h"
#include "asm/arch/memory.h"
#include "inttypes.h"


//#define STEREO_SOUND


// -----------------------------------------------------------------------------
class CGBASound
 : public IInterruptServiceRoutine
{
public:
   CGBASound();
  virtual ~CGBASound();

  // Inherited from IInterruptServiceRoutine
  virtual int isr(int irq) INTERRUPT_CODE;

  void write(const int16_t * sampleData, unsigned int sampleCount) FAST_CODE;

private:
  unsigned int   iFragmentSize_;

  int8_t       * pDMABufferA_;
#ifdef STEREO_SOUND
  int8_t       * pDMABufferB_;
#endif
  unsigned int   iDMABufferSize_;
  unsigned int   iDMAFragmentCount_;
  volatile int   iDMARead_;
  volatile int   iDMAWrite_;
  unsigned int   iDMAFragmentOffset_;

#ifdef CONFIG_MULTI_THREADING
  pthread_mutex_t mutex_;       // The locking mutex
  pthread_cond_t condFragmentPlayed_;    // State change condition
#endif
};


#endif
