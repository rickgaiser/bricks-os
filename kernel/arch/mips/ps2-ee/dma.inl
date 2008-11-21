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
#include "bios.h"
#include "stddef.h"
#include "asm/cpu.h"


//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
void
CDMAPacket::reset()
{
  pCurrent_ = pData_;
}

//-------------------------------------------------------------------------
#define ADD_DATA(__TYPE, __DATA) *((__TYPE *)pCurrent_) = __DATA; pCurrent_ += sizeof(__TYPE);

//-------------------------------------------------------------------------
template<class T> void
CDMAPacket::add(const T data)
{
  ADD_DATA(T, data);
}

//-------------------------------------------------------------------------
void
CDMAPacket::pad128(uint32_t data)
{
  while(((uint32_t)pCurrent_ & 0xf) != 0)
    this->add(data);
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
#define DMA_SC_TAG(QWC, PCE, ID, IRQ, ADDR, SPR) \
  ((uint64_t)(QWC)    <<  0)  | \
  ((uint64_t)(PCE)    << 26)  | \
  ((uint64_t)(ID)     << 28)  | \
  ((uint64_t)(IRQ)    << 31)  | \
  ((uint64_t)((uint32_t)(ADDR))   << 32)  | \
  ((uint64_t)(SPR)    << 63)

//-------------------------------------------------------------------------
void
CSCDMAPacket::reset()
{
  CDMAPacket::reset();

  pSCTag_ = NULL;
}

//-------------------------------------------------------------------------
CSCDMAPacket &
CSCDMAPacket::scTagOpenCnt(bool irq)
{
  if(pSCTag_ == NULL)
  {
    pSCTag_ = (SDMATag *)pCurrent_;
    this->add64(DMA_SC_TAG(0, 0, DMAC::Tag::cnt, irq, 0, false));
    this->add64(0);
  }

  return *this;
}

//-------------------------------------------------------------------------
CSCDMAPacket &
CSCDMAPacket::scTagRef(const void * data, uint32_t qwSize, bool irq)
{
  if(pSCTag_ == NULL)
  {
    this->add64(DMA_SC_TAG(qwSize, 0, DMAC::Tag::ref, irq, ADDR_TO_PHYS(data), false));
    this->add64(0);
  }

  return *this;
}

//-------------------------------------------------------------------------
CSCDMAPacket &
CSCDMAPacket::scTagRefe(const void * data, uint32_t qwSize, bool irq)
{
  if(pSCTag_ == NULL)
  {
    this->add64(DMA_SC_TAG(qwSize, 0, DMAC::Tag::refe, irq, ADDR_TO_PHYS(data), false));
    this->add64(0);
  }

  return *this;
}

//-------------------------------------------------------------------------
CSCDMAPacket &
CSCDMAPacket::scTagOpenEnd(bool irq)
{
  if(pSCTag_ == NULL)
  {
    pSCTag_ = (SDMATag *)pCurrent_;
    this->add64(DMA_SC_TAG(0, 0, DMAC::Tag::end, irq, 0, false));
    this->add64(0);
  }

  return *this;
}

//-------------------------------------------------------------------------
CSCDMAPacket &
CSCDMAPacket::scTagClose()
{
  if((pSCTag_ != NULL) && (((uint32_t)pCurrent_ & 0xf) == 0))
  {
    uint32_t qwc = ((uint32_t)pCurrent_ - (uint32_t)pSCTag_) / 16 - 1;
    *((uint64_t *)pSCTag_) |= qwc;

    pSCTag_ = NULL;
  }

  return *this;
}
