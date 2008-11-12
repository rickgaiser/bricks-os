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


#include "dma.h"
#include "kernel/debug.h"


const char * sDMASource[] =
{
  "VIF0",
  "VIF1",
  "GIF",
  "fromIPU",
  "toIPU",
  "SIF0 (IOP to EE)",
  "SIF1 (EE to IOP)",
  "SIF2",
  "fromSPR",
  "toSPR"
};


//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
CDMAC::CDMAC()
{
}

//-------------------------------------------------------------------------
CDMAC::~CDMAC()
{
}

//-------------------------------------------------------------------------
void
CDMAC::init()
{
  iDMAMask_ = 0;

  // Clear DMA mask/status
  //  0.. 9 == status (writing 1 clears status)
  // 16..25 == mask   (writing 1 xors mask)
  REG_DMA_STAT = REG_DMA_STAT;

  // DMA controller is connected to MIPS interrupt PIN1
  setInterruptHandler(MIPS_INT_1, *this);
}

//-------------------------------------------------------------------------
void
CDMAC::isr(unsigned int irq, pt_regs * regs)
{
  uint32_t status = REG_DMA_STAT;

  // DMA controller
  printk("interrupt from DMAC(0x%x)\n", status);

  for(int i(0); i < 10; i++)
  {
    if(status & (1 << i))
    {
      printk(" - %s Complete\n", sDMASource[i]);

      // Clear status
      REG_DMA_STAT = (1 << i);
    }
  }
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
CDMAPacket::CDMAPacket(uint32_t qwSize, EDMAChannel channelId)
 : bHasOwnership_(true)
 , eChannelId_(channelId)
{
  // Clip qwSize to DMA_MAX_QWTRANSFER
  if(qwSize > DMA_MAX_QWTRANSFER)
    qwSize = DMA_MAX_QWTRANSFER;
  iQWSize_ = qwSize;

  // Allocate data (+63 bytes for alignment)
  pRawData_ = new uint8_t[(qwSize * 16) + (64 - 1)];

  // Align to 64 byte boundry
  pData_    = (uint8_t *)((((uint32_t)pRawData_) + (64 - 1)) & (~(64 - 1)));
  pCurrent_ = pData_;
  pEnd_     = pData_ + (qwSize * 16);
}

//-------------------------------------------------------------------------
CDMAPacket::CDMAPacket(void * buffer, uint32_t qwSize, EDMAChannel channelId)
 : pRawData_((uint8_t *)buffer)
 , bHasOwnership_(true)
 , eChannelId_(channelId)
{
  // Clip qwSize to DMA_MAX_QWTRANSFER
  if(qwSize > DMA_MAX_QWTRANSFER)
    qwSize = DMA_MAX_QWTRANSFER;
  iQWSize_ = qwSize;

  // Align to 64 byte boundry
  pData_    = (uint8_t *)((((uint32_t)pRawData_) + (64 - 1)) & (~(64 - 1)));
  pCurrent_ = pData_;
  pEnd_     = pData_ + (qwSize * 16);
}

//-------------------------------------------------------------------------
CDMAPacket::~CDMAPacket()
{
  if((bHasOwnership_ == true) && (pRawData_ != NULL))
    delete pRawData_;
}

//-------------------------------------------------------------------------
void
CDMAPacket::send(bool waitComplete)
{
  uint32_t qwc = ((uint32_t)pCurrent_ - (uint32_t)pData_) / 16;

  // Flush caches before transfer
  bios::FlushCache(0);

  // Send
  dmaSend(eChannelId_, pData_, qwc, false);

  // Wait for completion
  if(waitComplete == true)
    dmaWait(eChannelId_);
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
CSCDMAPacket::CSCDMAPacket(uint32_t qwSize, EDMAChannel channelId)
 : CDMAPacket(qwSize, channelId)
 , pSCTag_(NULL)
{
}

//-------------------------------------------------------------------------
CSCDMAPacket::CSCDMAPacket(void * buffer, uint32_t qwSize, EDMAChannel channelId)
 : CDMAPacket(buffer, qwSize, channelId)
 , pSCTag_(NULL)
{
}

//-------------------------------------------------------------------------
CSCDMAPacket::~CSCDMAPacket()
{
}

//-------------------------------------------------------------------------
void
CSCDMAPacket::send(bool waitComplete)
{
  uint32_t qwc = ((uint32_t)pCurrent_ - (uint32_t)pData_) / 16;

  // Flush caches before transfer
  bios::FlushCache(0);

  // Send
  dmaSend(eChannelId_, pData_, qwc, true);

  // Wait for completion
  if(waitComplete == true)
    dmaWait(eChannelId_);
}
