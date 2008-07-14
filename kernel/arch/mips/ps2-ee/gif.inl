#include "gif.h"


#define GIF_AD   0x0e
#define GIF_NOP  0x0f


//-------------------------------------------------------------------------
void
CGIFPacket::reset()
{
  iPos_        = 0;
  iDMASize_    = 0;
  iHeaderSize_ = 0;
}

//-------------------------------------------------------------------------
void
CGIFPacket::tag(uint64_t EOP, uint64_t PRE, uint64_t PRIM, uint64_t FLG, uint64_t NREG, uint64_t REGS)
{
  data(REGS, (EOP  << 15) |
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
CGIFPacket::data(uint64_t REG, uint64_t DAT)
{
  pData_[iPos_++] = DAT;
  pData_[iPos_++] = REG;
  iDMASize_++;
  if((iDMASize_ - 1) == iMAXSize_)
    send();
}

//-------------------------------------------------------------------------
void
CGIFPacket::setSize()
{
  // Reset total send size of data
  pData_[0] &= (~0x7fff);

  // Set total send size of data
  pData_[0] |= iDMASize_ - 1; // Data size (DMA - tag)
}

//-------------------------------------------------------------------------
void
CGIFPacket::headerSize(uint64_t size)
{
  iHeaderSize_ = size;
}
