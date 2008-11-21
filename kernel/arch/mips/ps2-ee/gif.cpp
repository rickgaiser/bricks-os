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


#include "gif.h"
#include "dma.h"
#include "bios.h"
#include "asm/arch/registers.h"


uint64_t gifData_Data[50 * 2] __attribute__((aligned(64)));
CGIFPacket gifData(gifData_Data, 50, DMAC::Channel::GIF);


//-------------------------------------------------------------------------
//---------------------------------------------------------------------------
void
ee_to_gsBitBlt(uint32_t daddr, int dw, int dpsm, int dx, int dy, int w, int h, uint32_t source)
{
  // BitBlt from EE to GS
  gifData.scTagOpenCnt();
  {
    gifData.gifTagOpenPacked();
    {
      gifData.gifAddPackedAD(GIF::REG::bitbltbuf, GIF::REG::BITBLTBUF(0, 0, 0, daddr >> 8, dw >> 6, dpsm));
      gifData.gifAddPackedAD(GIF::REG::trxpos,    GIF::REG::TRXPOS(0, 0, dx, dy, 0));
      gifData.gifAddPackedAD(GIF::REG::trxreg,    GIF::REG::TRXREG(w, h));
      gifData.gifAddPackedAD(GIF::REG::trxdir,    GIF::REG::TRXDIR(GS_TRXDIR_EE_TO_GS));
    }
    gifData.gifTagClose();
  }
  gifData.scTagClose();

  // Send image data
  switch(dpsm)
  {
    case GS_PSM_32:  // Framebuffer, Z-Buffer & Textures
      gifData.gifAddSendImage((void *)source, w * h * 4);
      break;
    case GS_PSM_24:  // Framebuffer, Z-Buffer & Textures
      gifData.gifAddSendImage((void *)source, w * h * 3);
      break;
    case GS_PSM_16:  // Framebuffer, Z-Buffer & Textures
    case GS_PSM_16S: // Framebuffer, Z-Buffer & Textures
      gifData.gifAddSendImage((void *)source, w * h * 2);
      break;
    case GS_PSM_8:   // Textures only (palettized)
    case GS_PSM_8H:  // Textures only (palettized)
      gifData.gifAddSendImage((void *)source, w * h);
      break;
    case GS_PSM_4:   // Textures only (palettized)
    case GS_PSM_4HH: // Textures only (palettized)
    case GS_PSM_4HL: // Textures only (palettized)
      gifData.gifAddSendImage((void *)source, (w * h) >> 1);
      break;
  };

  // Access the TEXFLUSH register with anything to flush the texture
  gifData.scTagOpenEnd();
  {
    gifData.gifTagOpenPacked();
    {
      gifData.gifAddPackedAD(GIF::REG::texflush, 0x42);
    }
    gifData.gifTagClose();
  }
  gifData.scTagClose();

  // Start data transfer and reset packet
  gifData.send();
  gifData.reset();
}

//---------------------------------------------------------------------------
void
gs_to_gsBitBlt(uint32_t daddr, int dw, int dpsm, int dx, int dy, int w, int h, uint32_t source, int sw, int spsm, int sx, int sy)
{
  gifData.scTagOpenEnd();
    gifData.gifTagOpenPacked();
      gifData.gifAddPackedAD(GIF::REG::bitbltbuf, GIF::REG::BITBLTBUF(source>>8, sw>>6, spsm, daddr>>8, dw>>6, dpsm));
      gifData.gifAddPackedAD(GIF::REG::trxpos,    GIF::REG::TRXPOS(sx, sy, dx, dy, 0));
      gifData.gifAddPackedAD(GIF::REG::trxreg,    GIF::REG::TRXREG(w, h));
      gifData.gifAddPackedAD(GIF::REG::trxdir,    GIF::REG::TRXDIR(GS_TRXDIR_GS_TO_GS));
    gifData.gifTagClose();
  gifData.scTagClose();
  gifData.send();
  gifData.reset();
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
CGIFPacket::CGIFPacket(uint32_t qwSize, EDMAChannel channelId)
 : CSCDMAPacket(qwSize, channelId)
 , pGIFTag_(NULL)
{
}

//-------------------------------------------------------------------------
CGIFPacket::CGIFPacket(void * buffer, uint32_t qwSize, EDMAChannel channelId)
 : CSCDMAPacket(buffer, qwSize, channelId)
 , pGIFTag_(NULL)
{
}

//-------------------------------------------------------------------------
CGIFPacket::~CGIFPacket()
{
}

//-------------------------------------------------------------------------
void
CGIFPacket::gifTagOpenPacked(
  bool embedPrim,
  uint16_t prim,
  uint8_t regCount,
  uint8_t reg1,
  uint8_t reg2,
  uint8_t reg3,
  uint8_t reg4,
  uint8_t reg5,
  uint8_t reg6,
  uint8_t reg7,
  uint8_t reg8,
  uint8_t reg9,
  uint8_t reg10,
  uint8_t reg11,
  uint8_t reg12,
  uint8_t reg13,
  uint8_t reg14,
  uint8_t reg15,
  uint8_t reg16)
{
  if(pGIFTag_ == NULL)
  {
    pGIFTag_ = pCurrent_;
    this->add64(DMA_GIF_TAG(0, 1, embedPrim, prim, GIF::DataFormat::packed, regCount));
    this->add64(
      (uint64_t)reg1  << ( 0 * 4) |
      (uint64_t)reg2  << ( 1 * 4) |
      (uint64_t)reg3  << ( 2 * 4) |
      (uint64_t)reg4  << ( 3 * 4) |
      (uint64_t)reg5  << ( 4 * 4) |
      (uint64_t)reg6  << ( 5 * 4) |
      (uint64_t)reg7  << ( 6 * 4) |
      (uint64_t)reg8  << ( 7 * 4) |
      (uint64_t)reg9  << ( 8 * 4) |
      (uint64_t)reg10 << ( 9 * 4) |
      (uint64_t)reg11 << (10 * 4) |
      (uint64_t)reg12 << (11 * 4) |
      (uint64_t)reg13 << (12 * 4) |
      (uint64_t)reg14 << (13 * 4) |
      (uint64_t)reg15 << (14 * 4) |
      (uint64_t)reg16 << (15 * 4)
    );
    // Divide by 16 to get the number of 128bit values
    nloopDiv_ = 16 * regCount;
  }
}

//-------------------------------------------------------------------------
void
CGIFPacket::gifTagOpenReglist(
  bool embedPrim,
  uint16_t prim,
  uint8_t regCount,
  uint8_t reg1,
  uint8_t reg2,
  uint8_t reg3,
  uint8_t reg4,
  uint8_t reg5,
  uint8_t reg6,
  uint8_t reg7,
  uint8_t reg8,
  uint8_t reg9,
  uint8_t reg10,
  uint8_t reg11,
  uint8_t reg12,
  uint8_t reg13,
  uint8_t reg14,
  uint8_t reg15,
  uint8_t reg16)
{
  if(pGIFTag_ == NULL)
  {
    pGIFTag_ = pCurrent_;
    this->add64(DMA_GIF_TAG(0, 1, embedPrim, prim, GIF::DataFormat::reglist, regCount));
    this->add64(
      (uint64_t)reg1  << ( 0 * 4) |
      (uint64_t)reg2  << ( 1 * 4) |
      (uint64_t)reg3  << ( 2 * 4) |
      (uint64_t)reg4  << ( 3 * 4) |
      (uint64_t)reg5  << ( 4 * 4) |
      (uint64_t)reg6  << ( 5 * 4) |
      (uint64_t)reg7  << ( 6 * 4) |
      (uint64_t)reg8  << ( 7 * 4) |
      (uint64_t)reg9  << ( 8 * 4) |
      (uint64_t)reg10 << ( 9 * 4) |
      (uint64_t)reg11 << (10 * 4) |
      (uint64_t)reg12 << (11 * 4) |
      (uint64_t)reg13 << (12 * 4) |
      (uint64_t)reg14 << (13 * 4) |
      (uint64_t)reg15 << (14 * 4) |
      (uint64_t)reg16 << (15 * 4)
    );
    // Divide by 8 to get the number of 64bit values
    nloopDiv_ = 8 * regCount;
  }
}

//-------------------------------------------------------------------------
void
CGIFPacket::gifTagClose()
{
  if((pGIFTag_ != NULL) && (((uint32_t)pCurrent_ & 0xf) == 0))
  {
    uint32_t nloop = ((uint32_t)pCurrent_ - ((uint32_t)pGIFTag_ + 16)) / nloopDiv_;
    *((uint64_t *)pGIFTag_) |= nloop;

    pGIFTag_ = NULL;
  }
}

//-------------------------------------------------------------------------
// Uses source chain to send the image data
// NOTE: SC & GIF tags HAVE TO BE CLOSED before calling this function
void
CGIFPacket::gifAddSendImage(const void * source, uint32_t size)
{
  uint32_t qtotal = size / 16; // total number of quadwords to transfer.

  while(qtotal > 0)
  {
    // Total send size of data
    uint32_t sendSize = (qtotal > DMA_MAX_QWTRANSFER) ? DMA_MAX_QWTRANSFER : qtotal;

    // Send GIF tag
    this->scTagOpenCnt();
      this->add64(DMA_GIF_TAG(sendSize, 1, 0, 0, GIF::DataFormat::image, 0));
      this->add64(0);
    this->scTagClose();

    // Send data
    this->scTagRef(source, sendSize);

    (uint8_t *)source += sendSize * 16;
    qtotal -= sendSize;
  }
}
