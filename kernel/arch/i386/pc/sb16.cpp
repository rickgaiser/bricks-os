/*
 * Bricks-OS, Operating System for Game Consoles
 * Copyright (C) 2010 Maximus32 <Maximus32@bricks-os.org>
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


#include "sb16.h"
#include "asm/hal.h"
#include "kernel/debug.h"
#include "kernel/interruptManager.h"


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
CSoundBlaster::CSoundBlaster()
{
  iFragmentSize_        = DSP_MAX_FRAGMENT_SIZE;

  iDMAFragmentCount_    = 2;
  iDMABufferSize_       = iFragmentSize_ * iDMAFragmentCount_;
  pDMABufferPhys_       = (void *)0x00020000; // FIXME: Dirty hack!
  pDMABuffer_           = pDMABufferPhys_;    // FIXME: Dirty hack!
  iDMARead_             = -1;
  iDMAWrite_            = 0;
  iDMAFragmentOffset_   = 0;

#ifdef CONFIG_MULTI_THREADING
  k_pthread_mutex_init(&mutex_, NULL);
  k_pthread_cond_init(&condFragmentPlayed_, NULL);
#endif

  iSampleRate_ = DSP_DEFAULT_SPEED;
  bStereo_ = false;
  bSigned_ = true;
  iBits_ = 16;
}

// -----------------------------------------------------------------------------
CSoundBlaster::~CSoundBlaster()
{
}

// -----------------------------------------------------------------------------
int
CSoundBlaster::init()
{
  if(dspReset() == false)
  {
    printk("sb16:ERROR: SoundBlaster not detected\n");
    return false;
  }

  uint8_t dspVersion[2];
  dspVersion[0] = dspVersion[1] = 0;
  dspCommand(DSP_GET_VERSION);
  for(int i = 1000; i; i--)
  {
    if(dspIn8(DSP_DATA_AVL) & 0x80)
    {
      if(dspVersion[0] == 0)
      {
        dspVersion[0] = dspIn8(DSP_READ);
      }
      else
      {
        dspVersion[1] = dspIn8(DSP_READ);
        break;
      }
    }
  }

  const char * sName;
  switch(dspVersion[0])
  {
  case 1:
    sName = "1.0";
    break;
  case 2:
    if(dspVersion[1] == 0)
      sName = "2.0";
    else
      sName = "2.01+";
    break;
  case 3:
    sName = "Pro";
    break;
  case 4:
    sName = "16";
    break;
  default:
    printk("sb16:ERROR: Unknown version (%d.%d)\n", dspVersion[0], dspVersion[1]);
    return -1;
  }

  printk("sb16: SoundBlaster %s detected\n", sName);

  if(dspVersion[0] < 4)
  {
    printk("sb16:ERROR: Only SoundBlaster 16 is supported\n");
    return -1;
  }

  // Set SB to use our IRQ and DMA channels
  mixerSet(MIXER_SET_IRQ, (1 << (SB_IRQ / 2 - 1)));
  mixerSet(MIXER_SET_DMA, (1 << SB_DMA_8 | 1 << SB_DMA_16));
  // Set master volume
  mixerSet(MIXER_MASTER_LEFT,  20 << 3); // Volume: 0..31
  mixerSet(MIXER_MASTER_RIGHT, 20 << 3); // Volume: 0..31
  // Set line-out volume
  //mixerSet(MIXER_LINE_LEFT,  20 << 3); // Volume: 0..31
  //mixerSet(MIXER_LINE_RIGHT, 20 << 3); // Volume: 0..31
#if 0
  // Manual output gain
  mixerSet(MIXER_GAIN_OUT_LEFT,  3 << 6); // Gain: 0..3
  mixerSet(MIXER_GAIN_OUT_RIGHT, 3 << 6); // Gain: 0..3
  // Automatic gain control
  mixerSet(MIXER_AGC, 0);
#else
  // Automatic gain control
  mixerSet(MIXER_AGC, 1);
#endif

  // Try to get the IRQ
  CInterruptManager::attach(0x20 + SB_IRQ, this);

  return 0;
}

// -----------------------------------------------------------------------------
void
CSoundBlaster::start()
{
  bool bWrite = true;

  unsigned int iChannel = (iBits_ == 8) ? SB_DMA_8 : SB_DMA_16;
  dmaDisable   (iChannel);
  dmaSetMode   (iChannel, (bWrite == true ? DMA_MODE_OUT : DMA_MODE_IN) | DMA_MODE_AUTO | DMA_MODE_SINGLE);
  dmaSetAddress(iChannel, (uint32_t)pDMABufferPhys_);
  dmaSetCount  (iChannel, (iBits_ == 8) ? (iFragmentSize_ * iDMAFragmentCount_) : ((iFragmentSize_ * iDMAFragmentCount_) >> 1));
  dmaEnable    (iChannel);

  dspSetSampleRate(iSampleRate_);

  /* Put the speaker on */
  if(bWrite == true)
  {
    dspCommand(DSP_CMD_SPKON);
    dspCommand((iBits_ == 8 ? DSP_CMD_8BITAUTO_OUT : DSP_CMD_16BITAUTO_OUT));
  }
  else
  {
    dspCommand(DSP_CMD_SPKOFF);
    dspCommand((iBits_ == 8 ? DSP_CMD_8BITAUTO_IN : DSP_CMD_16BITAUTO_IN));
  }

  /* Program DSP with transfer mode */
  if(bSigned_ == true)
    dspCommand((bStereo_ == true ? DSP_MODE_STEREO_S : DSP_MODE_MONO_S));
  else
    dspCommand((bStereo_ == true ? DSP_MODE_STEREO_US : DSP_MODE_MONO_US));

  /* Give length of fragment to DSP */
  if(iBits_ == 8)
  {
    // Bytes - 1
    dspCommand((iFragmentSize_ - 1) >> 0);
    dspCommand((iFragmentSize_ - 1) >> 8);
  }
  else
  {
    // Words - 1
    dspCommand((iFragmentSize_ - 2) >> 1);
    dspCommand((iFragmentSize_ - 2) >> 9);
  }
}

// -----------------------------------------------------------------------------
bool
CSoundBlaster::dspReset()
{
  volatile int i;

  dspOut8(DSP_RESET, 1);
  for(i = 0; i < 1000; i++);
  dspOut8(DSP_RESET, 0);
  for(i = 0; i < 1000 && !(dspIn8(DSP_DATA_AVL) & 0x80); i++);

  return (dspIn8(DSP_READ) == 0xAA);
}

// -----------------------------------------------------------------------------
bool
CSoundBlaster::dspCommand(uint8_t value)
{
  for(int i = 0; i < SB_TIMEOUT; i++)
  {
    if((dspIn8(DSP_STATUS) & 0x80) == 0)
    {
      dspOut8(DSP_COMMAND, value);
      return true;
    }
  }

  printk("sb16:ERROR: DSP Command(0x%x) timeout\n", value);
  return false;
}

// -----------------------------------------------------------------------------
bool
CSoundBlaster::dspSetSampleRate(uint16_t speed)
{
  if(speed < DSP_MIN_SPEED || speed > DSP_MAX_SPEED)
    return false;

  dspCommand(DSP_INPUT_RATE);
  dspCommand(speed >> 8);
  dspCommand(speed);
  dspCommand(DSP_OUTPUT_RATE);
  dspCommand(speed >> 8);
  dspCommand(speed);

  return true;
}

// -----------------------------------------------------------------------------
uint8_t
CSoundBlaster::mixerGet(int reg)
{
  outb(reg, MIXER_REG);
  for(volatile int i = 0; i < 100; i++);

  return inb(MIXER_DATA) & 0xff;
}

// -----------------------------------------------------------------------------
bool
CSoundBlaster::mixerSet(int reg, uint8_t data)
{
  outb(reg, MIXER_REG);
  for(volatile int i = 0; i < 100; i++);
  outb(data, MIXER_DATA);

  return true;
}

// -----------------------------------------------------------------------------
void
CSoundBlaster::write(const int16_t * sampleData, unsigned int sampleCount)
{
  //printk("write(%d)(%d == %d)\n", sampleCount, iDMARead_, iDMAWrite_);

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
        //k_pthread_cond_wait(&condFragmentPlayed_, &mutex_);
#endif
      }

      // Copy sample data
      unsigned int iWriteCount;
      unsigned int iBytesLeft = iFragmentSize_ - iDMAFragmentOffset_;
      if(iBits_ == 8)
      {
        iWriteCount = (sampleCount < iBytesLeft) ? sampleCount : iBytesLeft;
        int8_t * pBuffer = ((int8_t *)pDMABuffer_) + (iDMAWrite_ * iFragmentSize_) + iDMAFragmentOffset_;
        for(unsigned int i = 0; i < iWriteCount; i++)
          *pBuffer++ = sampleData[i] >> 8;

        sampleCount -= iWriteCount;
        iDMAFragmentOffset_ += iWriteCount;
      }
      else
      {
        iWriteCount = (sampleCount < (iBytesLeft >> 1)) ? sampleCount : (iBytesLeft >> 1);
        int16_t * pBuffer = ((int16_t *)pDMABuffer_) + (((iDMAWrite_ * iFragmentSize_) + iDMAFragmentOffset_) >> 1);
        for(unsigned int i = 0; i < iWriteCount; i++)
          *pBuffer++ = sampleData[i];

        sampleCount -= iWriteCount;
        iDMAFragmentOffset_ += iWriteCount * 2;
      }

      if(iDMAFragmentOffset_ >= iFragmentSize_)
      {
        //printk("DMA Buffer[%d] ready\n", iDMAWrite_);
        iDMAWrite_ = (iDMAWrite_ + 1) % iDMAFragmentCount_;
        iDMAFragmentOffset_ = 0;

        if(iDMARead_ < 0)
        {
          iDMARead_ = 0;
          start();
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
int
CSoundBlaster::isr(int irq)
{
  //static volatile int iISRCounter = 0;
  //printk("CSoundBlaster::isr: %d(R=%d W=%d)\n", iISRCounter++, iDMARead_, iDMAWrite_);

  //printk("DMA Buffer[%d] done playing\n", iDMARead_);

  if(iDMARead_ >= 0)
  {
    int iDMAReadNext = (iDMARead_ + 1) % iDMAFragmentCount_;
    if(iDMAReadNext == iDMAWrite_)
    {
      // Buffer empty
      printk("CSoundBlaster::isr: no more data to play!\n");
      dspCommand((iBits_ == 8 ? DSP_CMD_DMA8HALT : DSP_CMD_DMA16HALT));
      iDMARead_ = -1;
      iDMAWrite_ = 0;
      iDMAFragmentOffset_ = 0;
    }
    else
    {
      iDMARead_ = iDMAReadNext;
    }
  }

  // Acknowledge the interrupt on the DSP
  dspIn8((iBits_ == 8 ? DSP_DATA_AVL : DSP_DATA16_AVL));

#ifdef CONFIG_MULTI_THREADING
    //k_pthread_cond_broadcast(&condFragmentPlayed_);
#endif

  return 0;
}

// -----------------------------------------------------------------------------
uint8_t
CSoundBlaster::dspIn8(uint16_t addr)
{
  return inb(addr);
}

// -----------------------------------------------------------------------------
void
CSoundBlaster::dspOut8(uint16_t addr, uint8_t data)
{
  outb(data, addr);
}
