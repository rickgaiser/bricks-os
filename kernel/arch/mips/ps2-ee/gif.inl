#include "gif.h"


#define GIF_AD   0x0e
#define GIF_NOP  0x0f


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
