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
CGIFPacket::sendImage(uint32_t source, uint32_t size)
{
  uint32_t qtotal = size / 16; // total number of quadwords to transfer.

  this->reset(); // Just to be sure
  this->tag(1, 0, 0, 2, 0, 0);

  while(qtotal > 0)
  {
    // Total send size of data
    uint32_t sendSize = (qtotal > DMA_MAX_QWTRANSFER) ? DMA_MAX_QWTRANSFER : qtotal;

    ((uint64_t *)pData_)[0] &= (~0x7fff);
    ((uint64_t *)pData_)[0] |= sendSize;

    // Flush caches before transfer
    bios::FlushCache(0);

    // Send tag
    dmaSend(eChannelId_, pData_, 1);
    dmaWait(eChannelId_);

    // Send data
    dmaSend(eChannelId_, (void *)source, sendSize);
    dmaWait(eChannelId_);

    source += sendSize * 16;
    qtotal -= sendSize;
  }

  this->reset();
}
