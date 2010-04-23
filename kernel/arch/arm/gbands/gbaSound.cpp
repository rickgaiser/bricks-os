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


#include "gbaSound.h"
#include "kernel/interruptManager.h"
#include "kernel/debug.h"
#include "registers.h"
#include "macros.h"
#include "stdlib.h"


#define FREQUENCY_INDEX 6


struct SFrequencyTable
{
  uint16_t timer;
  uint16_t freq;
  uint16_t bufSize;
};
const SFrequencyTable freqTable[] =
{
  // timer, frequency, and buffer size for frequencies
  // that time out perfectly with VBlank.
  // These are in the order of the SND_FREQ enum in Sound.h.
  { 62610,  5734,  96 }, //  0
  { 63940, 10512, 176 }, //  1
  { 64282, 13379, 224 }, //  2
  { 64612, 18157, 304 }, //  3
  { 64738, 21024, 352 }, //  4
  { 64909, 26758, 448 }, //  5
  { 65004, 31536, 528 }, //  6
  { 65073, 36314, 608 }, //  7
  { 65118, 40137, 672 }, //  8
  { 65137, 42048, 704 }, //  9
  { 65154, 43959, 736 }  // 10
};


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
CGBASoundChannel::CGBASoundChannel()
{
  iFragmentSize_        = freqTable[FREQUENCY_INDEX].bufSize;

  iDMAFragmentCount_    = 2;
  iDMABufferSize_       = iFragmentSize_ * iDMAFragmentCount_;
  pDMABuffer_           = new int8_t[iDMABufferSize_];
  iDMARead_             = -1;
  iDMAWrite_            = 0;
  iDMAFragmentOffset_   = 0;

#ifdef CONFIG_MULTI_THREADING
  k_pthread_mutex_init(&mutex_, NULL);
  k_pthread_cond_init(&condFragmentPlayed_, NULL);
#endif

  // Clear buffers
  dmaFill32(0, pDMABuffer_,    iDMABufferSize_    >> 2);

  // Enable VBlank interrupt
  REG_DISPSTAT |= IRQ_VBLANK_ENABLE;

  // Initialize sound regs
  REG_SOUNDCNT_H = SNDA_LOUT | SNDA_ROUT | SNDA_FIFORESET | SNDA_VOL_100 | SNDA_TIMER0;
//                   SNDB_LOUT | SNDB_ROUT | SNDB_FIFORESET | SNDB_VOL_100 | SNDB_TIMER1;
  REG_SOUNDCNT_X = SND_ENABLE;

  // Initialize timer (have to use TM0 or TM1)
  REG_TM0D   = freqTable[FREQUENCY_INDEX].timer;
  REG_TM0CNT = TIMER_ENABLE;

  // Initialize DMA1 for sound (have to use DMA1 or DMA2)
  REG_DMACNT(1) = 0;
  REG_DMADAD(1) = (uint32_t)&REG_SOUNDA_FIFO;

  CInterruptManager::attach(0, this);
}

// -----------------------------------------------------------------------------
CGBASoundChannel::~CGBASoundChannel()
{
  CInterruptManager::detach(0, this);
}

// -----------------------------------------------------------------------------
void
CGBASoundChannel::write(const int16_t * sampleData, unsigned int sampleCount)
{
  if(sampleData != NULL)
  {
    // Write to dma buffer
    while(sampleCount > 0)
    {
      // Wait for free fragment
      while(iDMARead_ == iDMAWrite_)
      {
#ifdef CONFIG_MULTI_THREADING
        //printk("W");
        k_pthread_cond_wait(&condFragmentPlayed_, &mutex_);
#endif
      }

      // Copy sample data
      int8_t * pBuffer = pDMABuffer_ + (iDMAWrite_ * iFragmentSize_) + iDMAFragmentOffset_;
      unsigned int iWriteSize = sampleCount < (iFragmentSize_ - iDMAFragmentOffset_) ? sampleCount : (iFragmentSize_ - iDMAFragmentOffset_);
      for(unsigned int i = 0; i < iWriteSize; i++)
        *pBuffer++ = sampleData[i] >> 8;

      sampleCount -= iWriteSize;
      iDMAFragmentOffset_ += iWriteSize;
      if(iDMAFragmentOffset_ >= iFragmentSize_)
      {
        iDMAWrite_ = (iDMAWrite_ + 1) % iDMAFragmentCount_;
        iDMAFragmentOffset_ = 0;
      }
    }
  }
}

// -----------------------------------------------------------------------------
int
CGBASoundChannel::isr(int irq)
{
  if((iDMARead_ + 1) != iDMAWrite_)
  {
    //printk("P");
    iDMARead_ = (iDMARead_ + 1) % iDMAFragmentCount_;
#ifdef CONFIG_MULTI_THREADING
    k_pthread_cond_broadcast(&condFragmentPlayed_);
#endif

    if(iDMARead_ == 0)
    {
      // Reset DMA to the beginning of the sample buffer
      REG_DMACNT(1) = 0;
      REG_DMASAD(1) = (uint32_t)pDMABuffer_;
      REG_DMACNT(1) = DMA_DST_FIXED | DMA_REPEAT | DMA16 | DMA_SPECIAL | DMA_ENABLE;
    }
  }
  else
  {
    //printk("S");
    // Stop playing
    REG_DMACNT(1) = 0;
  }

  return 0;
}
