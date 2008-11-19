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

struct SDMAChannel
{
  vuint32_t * qwc;
  vuint32_t * madr;
  vuint32_t * tadr;
  vuint32_t * chcr;
};

static SDMAChannel dmaChannel_[10] =
{
  {(vuint32_t *)0x10008020, (vuint32_t *)0x10008010, (vuint32_t *)0x10008030, (vuint32_t *)0x10008000},
  {(vuint32_t *)0x10009020, (vuint32_t *)0x10009010, (vuint32_t *)0x10009030, (vuint32_t *)0x10009000},
  {(vuint32_t *)0x1000a020, (vuint32_t *)0x1000a010, (vuint32_t *)0x1000a030, (vuint32_t *)0x1000a000},
  {(vuint32_t *)0x1000b020, (vuint32_t *)0x1000b010, (vuint32_t *)0x1000b030, (vuint32_t *)0x1000b000},
  {(vuint32_t *)0x1000b420, (vuint32_t *)0x1000b410, (vuint32_t *)0x1000b430, (vuint32_t *)0x1000b400},
  {(vuint32_t *)0x1000c020, (vuint32_t *)0x1000c010, (vuint32_t *)0x1000c030, (vuint32_t *)0x1000c000},
  {(vuint32_t *)0x1000c420, (vuint32_t *)0x1000c410, (vuint32_t *)0x1000c430, (vuint32_t *)0x1000c400},
  {(vuint32_t *)0x1000c820, (vuint32_t *)0x1000c810, (vuint32_t *)0x1000c830, (vuint32_t *)0x1000c800},
  {(vuint32_t *)0x1000d020, (vuint32_t *)0x1000d010, (vuint32_t *)0x1000d030, (vuint32_t *)0x1000d000},
  {(vuint32_t *)0x1000d420, (vuint32_t *)0x1000d410, (vuint32_t *)0x1000d430, (vuint32_t *)0x1000d400}
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
  bios::_SyncDCache(pData_, (uint8_t *)pData_ + qwc);

  // Send
  *dmaChannel_[eChannelId_].qwc  = DMA_QWC(qwc);
  *dmaChannel_[eChannelId_].madr = DMA_MADR((uint32_t)ADDR_TO_PHYS(pData_), 0);
  *dmaChannel_[eChannelId_].tadr = DMA_TADR(0, 0);
  *dmaChannel_[eChannelId_].chcr = DMA_CHCR(DMAC::Channel::fromMemory, DMAC::Channel::normal, 0, 0, 0, 1, 0);

  // Wait for completion
  if(waitComplete == true)
  {
    while(*dmaChannel_[eChannelId_].chcr & (1<<8));
  }
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
  bios::_SyncDCache(pData_, (uint8_t *)pData_ + qwc);

  // Send
  *dmaChannel_[eChannelId_].qwc  = DMA_QWC(0);
  *dmaChannel_[eChannelId_].madr = DMA_MADR(0, 0);
  *dmaChannel_[eChannelId_].tadr = DMA_TADR((uint32_t)ADDR_TO_PHYS(pData_), 0);
  *dmaChannel_[eChannelId_].chcr = DMA_CHCR(DMAC::Channel::fromMemory, DMAC::Channel::chain,  0, 0, 0, 1, 0);

  // Wait for completion
  if(waitComplete == true)
  {
    while(*dmaChannel_[eChannelId_].chcr & (1<<8));
  }
}
