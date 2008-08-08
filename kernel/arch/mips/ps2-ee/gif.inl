#include "gif.h"


//-------------------------------------------------------------------------
void
CGIFPacket::reset()
{
  CSCDMAPacket::reset();

  pGIFTag_ = NULL;
}

//-------------------------------------------------------------------------
CGIFPacket &
CGIFPacket::gifTagOpen()
{
  if(pGIFTag_ == NULL)
  {
    pGIFTag_ = pCurrent_;
    this->add(DMA_GIF_TAG(0, 1, 0, 0, GIF::DataFormat::packed, 1));
    this->add((uint64_t)GIF::REG::AD);
  }

  return *this;
}

//-------------------------------------------------------------------------
CGIFPacket &
CGIFPacket::gifTagClose()
{
  if((pGIFTag_ != NULL) && (((uint32_t)pCurrent_ & 0xf) == 0))
  {
    uint32_t qwc = ((uint32_t)pCurrent_ - (uint32_t)pGIFTag_) / 16 - 1;
    *((uint64_t *)pGIFTag_) |= qwc;

    pGIFTag_ = NULL;
  }

  return *this;
}

//-------------------------------------------------------------------------
void
CGIFPacket::addSetGSReg(uint64_t reg, uint64_t data)
{
  this->add(data);
  this->add(reg);
}
