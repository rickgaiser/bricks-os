#include "gif.h"
#include "dma.h"
#include "bios.h"
#include "asm/arch/registers.h"


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
    this->add64(DMA_GIF_TAG(0, 1, 0, 0, GIF::DataFormat::packed, regCount));
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
    this->add64(DMA_GIF_TAG(0, 1, 0, 0, GIF::DataFormat::reglist, regCount));
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
    this->scTagOpenRef(source, sendSize);
    this->scTagClose();

    (uint8_t *)source += sendSize * 16;
    qtotal -= sendSize;
  }
}
