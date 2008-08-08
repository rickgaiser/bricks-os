#include "gif.h"


#define GIF_AD   0x0e
#define GIF_NOP  0x0f


//-------------------------------------------------------------------------
#define DMA_GIF_TAG(QWC, EOP, PRE, PRIM, FLG, NREG) \
  ((uint64_t)(QWC)  <<  0) | \
  ((uint64_t)(EOP)  << 15) | \
  ((uint64_t)(PRE)  << 46) | \
  ((uint64_t)(PRIM) << 47) | \
  ((uint64_t)(FLG)  << 58) | \
  ((uint64_t)(NREG) << 60)

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
    //this->add(DMA_GIF_TAG(0, 1, 0, 0, 0, 1));
    //this->add((uint64_t)GIF_AD);
    this->tagAd(1, 0, 0, 0);
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
CGIFPacket::tag(uint64_t EOP, uint64_t PRE, uint64_t PRIM, uint64_t FLG, uint64_t NREG, uint64_t REGS)
{
  addSetGSReg(REGS, (EOP  << 15) |
                    (PRE  << 46) |
                    (PRIM << 47) |
                    (FLG  << 58) |
                    (NREG << 60));
}

//-------------------------------------------------------------------------
void
CGIFPacket::tagAd(uint64_t EOP, uint64_t PRE, uint64_t PRIM, uint64_t FLG)
{
  tag(EOP, PRE, PRIM, FLG, 1, GIF_AD);
}

//-------------------------------------------------------------------------
void
CGIFPacket::addSetGSReg(uint64_t reg, uint64_t data)
{
  this->add(data);
  this->add(reg);
}
