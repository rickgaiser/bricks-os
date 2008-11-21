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
  { 62610, 5734, 96 },
  { 63940, 10512, 176 },
  { 64282, 13379, 224 },
  { 64612, 18157, 304 },
  { 64738, 21024, 352 },
  { 64909, 26758, 448 },
  { 65004, 31536, 528 },
  { 65073, 36314, 608 },
  { 65118, 40137, 672 },
  { 65137, 42048, 704 },
  { 65154, 43959, 736 }
};


// -----------------------------------------------------------------------------
CGBASound::CGBASound()
{
}

// -----------------------------------------------------------------------------
CGBASound::~CGBASound()
{
}

// -----------------------------------------------------------------------------
int
CGBASound::init()
{
  // Clear output buffer
  dmaFill32(0, bufferaData_, (736*2) >> 2);

  // Clear channels
  for(int iChannel(0); iChannel < SOUND_CHANNEL_COUNT; iChannel++)
    channel_[iChannel] = NULL;

  // Initialize channel buffer
  buffera_.mixBufferSize = freqTable[FREQUENCY_INDEX].bufSize;
  buffera_.mixBufferBase = bufferaData_;
  buffera_.curMixBuffer  = buffera_.mixBufferBase;
  buffera_.activeBuffer  = 1;
  buffera_.mixFreq       = freqTable[FREQUENCY_INDEX].freq;

//  bufferb_.mixBufferSize = freqTable[3].bufSize;
//  bufferb_.mixBufferBase = bufferbData_;
//  bufferb_.curMixBuffer  = bufferb_.mixBufferBase;
//  bufferb_.activeBuffer  = 1;
//  bufferb_.mixFreq       = freqTable[FREQUENCY_INDEX].freq;

  // Enable VBlank interrupt
  REG_DISPSTAT |= IRQ_VBLANK_ENABLE;
  // Register to receive the VBlank interrupt
  CInterruptManager::attach(0, this); // VBlank IRQ

  // Initialize sound regs
  REG_SOUNDCNT_H = SNDA_LOUT | SNDA_ROUT | SNDA_FIFORESET | SNDA_VOL_100 | SNDA_TIMER0;
//                   SNDB_LOUT | SNDB_ROUT | SNDB_FIFORESET | SNDB_VOL_100 | SNDB_TIMER1;
  REG_SOUNDCNT_X = SND_ENABLE;

  // Initialize timer (have to use TM0 or TM1)
  REG_TM0D   = freqTable[FREQUENCY_INDEX].timer;
  REG_TM0CNT = TIMER_ENABLE;
//  REG_TM1D   = freqTable[3].timer;
//  REG_TM1CNT = TIMER_ENABLE;

  // Initialize DMA1 for sound (have to use DMA1 or DMA2)
  REG_DMACNT(1) = 0;
  REG_DMADAD(1) = (uint32_t)&REG_SOUNDA_FIFO;
//  REG_DMACNT(2) = 0;
//  REG_DMADAD(2) = (uint32_t)&REG_SOUNDB_FIFO;

  return 0;
}

// -----------------------------------------------------------------------------
void
CGBASound::setChannel(uint8_t nr, SSoundChannel * data)
{
  if(nr < SOUND_CHANNEL_COUNT)
  {
    channel_[nr] = data;
  }
}

// -----------------------------------------------------------------------------
void
CGBASound::mix(uint32_t samplesToMix)
{
  int iActiveChannels(0);  // Count active channels
  int16_t temp[736];

  // Clear temp buffer
  dmaFill32(0, temp, (samplesToMix+1) >> 1);

  for(int iChannel(0); iChannel < SOUND_CHANNEL_COUNT; iChannel++)
  {
    SSoundChannel * pChannel = channel_[iChannel];

    if((pChannel != NULL) && (pChannel->data != NULL))
    {
      iActiveChannels++;

      if((pChannel->pos + (pChannel->inc * buffera_.mixBufferSize)) >= pChannel->length)
      {
        // Channel will end, so check it
        for(uint32_t i(0); i < samplesToMix; i++)
        {
          temp[i] += pChannel->data[pChannel->pos >> 12] * pChannel->vol;
          pChannel->pos += pChannel->inc;

          if(pChannel->pos >= pChannel->length)
          {
            if(pChannel->loopLength == 0)
            {
              // Stop
              pChannel->data = NULL;
              i = samplesToMix;
            }
            else
            {
              // Loop
              while(pChannel->pos >= pChannel->length)
                pChannel->pos -= pChannel->loopLength;
            }
          }
        }
      }
      else
      {
        // Channel will not end, fast processing
        for(uint32_t i(0); i < samplesToMix; i++)
        {
          temp[i] += pChannel->data[pChannel->pos >> 12] * pChannel->vol;
          pChannel->pos += pChannel->inc;
        }
      }
    }
  }

  if(iActiveChannels > 0)
  {
    // Copy temp buffer to sound buffer
    for(uint32_t i(0); i < samplesToMix; i++)
      buffera_.curMixBuffer[i] = temp[i] >> 8;
  }
  else
  {
    // Clear sound buffer
    dmaFill32(0, buffera_.curMixBuffer, buffera_.mixBufferSize >> 2);
  }

  buffera_.curMixBuffer += samplesToMix;
}

// -----------------------------------------------------------------------------
int
CGBASound::isr(int irq)
{
  if(buffera_.activeBuffer == 1)
  {
    REG_DMACNT(1) = 0;
    REG_DMASAD(1) = (uint32_t)buffera_.mixBufferBase;
    REG_DMACNT(1) = DMA_DST_FIXED | DMA_REPEAT | DMA16 | DMA_SPECIAL | DMA_ENABLE;

    buffera_.curMixBuffer = buffera_.mixBufferBase + buffera_.mixBufferSize;
    buffera_.activeBuffer = 0;
  }
  else
  {
    buffera_.curMixBuffer = buffera_.mixBufferBase;
    buffera_.activeBuffer = 1;
  }

  return 0;
}
