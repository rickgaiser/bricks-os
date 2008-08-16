#include "vif.h"


//-------------------------------------------------------------------------
void
CVIFPacket::reset()
{
  CSCDMAPacket::reset();
}

//-------------------------------------------------------------------------
void
CVIFPacket::vifAddCommand(uint8_t command, uint8_t num, uint8_t immediate)
{
  this->add32(VIF_CODE(immediate, num, command));
}

//-------------------------------------------------------------------------
void
CVIFPacket::pad128()
{
  CSCDMAPacket::pad128(VIF_CODE(VIF_CMD_NOP, 0, 0));
}
