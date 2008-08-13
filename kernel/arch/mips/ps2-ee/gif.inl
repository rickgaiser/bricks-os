#include "gif.h"


//-------------------------------------------------------------------------
void
CGIFPacket::reset()
{
  CSCDMAPacket::reset();

  pGIFTag_ = NULL;
}

//-------------------------------------------------------------------------
void
CGIFPacket::gifAddPackedAD(uint64_t reg, uint64_t data)
{
  this->add64(data);
  this->add64(reg);
}

//-------------------------------------------------------------------------
void
CGIFPacket::pad128()
{
  CSCDMAPacket::pad128(0);
}
