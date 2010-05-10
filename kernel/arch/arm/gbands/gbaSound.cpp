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

/*
 * Based on Deku's gba-mod tutorial: http://deku.gbadev.org/
 *
 * One the GBA all sound is resampled to (16777216Hz / 512 =) 32.768KHz,
 * so the best used sample rates are:
 *    - 32.768kHz
 *    - 16.384kHz
 *    -  8.192kHz
 * We are already using 2 out of 4 available timers, and using one of
 * these sample rates would require the use of a 3rd timer.
 * Instead, we'll use the VBlank interrupt, with different sample rates.
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
  uint16_t freq;
  uint16_t bufSize;
};
const SFrequencyTable freqTable[] =
{
  // timer, frequency, and buffer size for frequencies
  // that time out perfectly with VBlank.
  {  5734,  96 }, //  0
  { 10512, 176 }, //  1
  { 13379, 224 }, //  2
  { 18157, 304 }, //  3
  { 21024, 352 }, //  4
  { 26758, 448 }, //  5
  { 31536, 528 }, //  6
  { 36314, 608 }, //  7
  { 40137, 672 }, //  8
  { 42048, 704 }, //  9
  { 43959, 736 }  // 10
};

int8_t dmaBufferA[736 * 2];
#ifdef STEREO_SOUND
int8_t dmaBufferB[736 * 2];
#endif


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
CGBASound::CGBASound()
{
  iFragmentSize_      = freqTable[FREQUENCY_INDEX].bufSize;
  iDMAFragmentCount_  = 2;
  iDMABufferSize_     = iFragmentSize_ * iDMAFragmentCount_;
  //pDMABufferA_        = new int8_t[iDMABufferSize_];
  pDMABufferA_        = dmaBufferA;
#ifdef STEREO_SOUND
  pDMABufferB_        = dmaBufferB;
#endif
  iDMARead_           = -1;
  iDMAWrite_          = 0;
  iDMAFragmentOffset_ = 0;

#ifdef CONFIG_MULTI_THREADING
  k_pthread_mutex_init(&mutex_, NULL);
  k_pthread_cond_init(&condFragmentPlayed_, NULL);
#endif

  // Clear buffers
  dmaFill32(0, pDMABufferA_, iDMABufferSize_ >> 2);
#ifdef STEREO_SOUND
  dmaFill32(0, pDMABufferB_, iDMABufferSize_ >> 2);
#endif

  // Enable VBlank interrupt
  REG_DISPSTAT |= IRQ_VBLANK_ENABLE;

  // Initialize sound regs
#ifdef STEREO_SOUND
  // stereo a=left, b=right
  REG_SOUNDCNT_H = SNDA_LOUT | SNDA_FIFORESET | SNDA_VOL_100 | SNDA_TIMER0 |
                   SNDB_ROUT | SNDB_FIFORESET | SNDB_VOL_100 | SNDB_TIMER1;
#else
                   // mono
                   REG_SOUNDCNT_H = SNDA_LOUT | SNDA_ROUT | SNDA_FIFORESET | SNDA_VOL_100 | SNDA_TIMER0;
#endif
  REG_SOUNDCNT_X = SND_ENABLE;

  // Initialize timer (have to use TM0 or TM1)
  REG_TM0D   = 65536 - (TIMER_FREQUENCY / freqTable[FREQUENCY_INDEX].freq);
  REG_TM0CNT = TIMER_ENABLE;
  // Initialize DMA1 for sound (have to use DMA1 or DMA2)
  REG_DMACNT(1) = 0;
  REG_DMADAD(1) = (uint32_t)&REG_SOUNDA_FIFO;
#ifdef STEREO_SOUND
  // Initialize timer (have to use TM0 or TM1)
  REG_TM1D   = 65536 - (TIMER_FREQUENCY / freqTable[FREQUENCY_INDEX].freq);
  REG_TM1CNT = TIMER_ENABLE;
  // Initialize DMA1 for sound (have to use DMA1 or DMA2)
  REG_DMACNT(2) = 0;
  REG_DMADAD(2) = (uint32_t)&REG_SOUNDB_FIFO;
#endif

  CInterruptManager::attach(0, this);
}

// -----------------------------------------------------------------------------
CGBASound::~CGBASound()
{
  CInterruptManager::detach(0, this);
}

// -----------------------------------------------------------------------------
void
CGBASound::write(const int16_t * sampleData, unsigned int sampleCount)
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
        k_pthread_cond_wait(&condFragmentPlayed_, &mutex_);
#endif
      }

      // Copy sample data
#ifdef STEREO_SOUND
      int8_t * pBufferA = pDMABufferA_ + (iDMAWrite_ * iFragmentSize_) + iDMAFragmentOffset_;
      int8_t * pBufferB = pDMABufferB_ + (iDMAWrite_ * iFragmentSize_) + iDMAFragmentOffset_;
      unsigned int iWriteSize = sampleCount <= ((iFragmentSize_ - iDMAFragmentOffset_) << 1) ? sampleCount : ((iFragmentSize_ - iDMAFragmentOffset_) << 1);
      for(unsigned int i = 0; i < iWriteSize; i += 2)
      {
        *pBufferA++ = (*sampleData++) >> 8;
        *pBufferB++ = (*sampleData++) >> 8;
      }

      iDMAFragmentOffset_ += iWriteSize >> 1;
#else
      int8_t * pBufferA = pDMABufferA_ + (iDMAWrite_ * iFragmentSize_) + iDMAFragmentOffset_;
      unsigned int iWriteSize = sampleCount <= (iFragmentSize_ - iDMAFragmentOffset_) ? sampleCount : (iFragmentSize_ - iDMAFragmentOffset_);
      for(unsigned int i = 0; i < iWriteSize; i++)
      {
        *pBufferA++ = (*sampleData++) >> 8;
      }

      iDMAFragmentOffset_ += iWriteSize;
#endif
      sampleCount -= iWriteSize;
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
CGBASound::isr(int irq)
{
  if((iDMARead_ + 1) != iDMAWrite_)
  {
    iDMARead_ = (iDMARead_ + 1) % iDMAFragmentCount_;
#ifdef CONFIG_MULTI_THREADING
    k_pthread_cond_broadcast(&condFragmentPlayed_);
#endif

    if(iDMARead_ == 0)
    {
      // Reset DMA to the beginning of the sample buffer
      REG_DMACNT(1) = 0;
      REG_DMASAD(1) = (uint32_t)pDMABufferA_;
      REG_DMACNT(1) = DMA_DST_FIXED | DMA_REPEAT | DMA16 | DMA_SPECIAL | DMA_ENABLE;
#ifdef STEREO_SOUND
      REG_DMACNT(2) = 0;
      REG_DMASAD(2) = (uint32_t)pDMABufferB_;
      REG_DMACNT(2) = DMA_DST_FIXED | DMA_REPEAT | DMA16 | DMA_SPECIAL | DMA_ENABLE;
#endif
    }
  }
  else
  {
    // Stop playing
    REG_DMACNT(1) = 0;
#ifdef STEREO_SOUND
    REG_DMACNT(2) = 0;
#endif
  }

  return 0;
}
