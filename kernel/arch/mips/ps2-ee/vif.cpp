#include "vif.h"


//-------------------------------------------------------------------------
CVIFPacket::CVIFPacket(uint32_t qwSize, EDMAChannel channelId)
 : CSCDMAPacket(qwSize, channelId)
{
}

//-------------------------------------------------------------------------
CVIFPacket::CVIFPacket(void * buffer, uint32_t qwSize, EDMAChannel channelId)
 : CSCDMAPacket(buffer, qwSize, channelId)
{
}

//-------------------------------------------------------------------------
CVIFPacket::~CVIFPacket()
{
}

//-------------------------------------------------------------------------
void
CVIFPacket::vifAddSendCode(const void * code, uint32_t size)
{
  int32_t size64 = size / 8;

  while(size64 > 0)
  {
    // Total send size
    uint32_t sendSize = (size64 > 256) ? 256 : size64;

    // Add send code command (VIF_CMD_MPG)
    this->scTagOpenCnt();
    {
      this->vifAddCommand(VIF_CMD_NOP,    0, 0);
      this->vifAddCommand(VIF_CMD_NOP,    0, 0);
      this->vifAddCommand(VIF_CMD_NOP,    0, 0);
      this->vifAddCommand(VIF_CMD_MPG,    (sendSize == 256) ? 0 : sendSize, 0);
    }
    this->scTagClose();

    // If not 128bit aligned, add 2 NOP commands after code
    if(sendSize & 1)
    {
      ((uint32_t *)code)[sendSize * 2 + 0] = VIF_CODE(0, 0, VIF_CMD_NOP);
      ((uint32_t *)code)[sendSize * 2 + 1] = VIF_CODE(0, 0, VIF_CMD_NOP);
    }

    // Add code to DMA packet
    this->scTagRef(code, (sendSize + 1) / 2);

    (uint64_t *)code += sendSize;
    size64 -= sendSize;
  }
}
