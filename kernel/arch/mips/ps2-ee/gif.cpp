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
CGIFPacket::addSendImage(const void * source, uint32_t size)
{
  uint32_t qtotal = size / 16; // total number of quadwords to transfer.

  while(qtotal > 0)
  {
    // Total send size of data
    uint32_t sendSize = (qtotal > DMA_MAX_QWTRANSFER) ? DMA_MAX_QWTRANSFER : qtotal;

    // Send GIF tag
    this->scTagOpenCnt();
      this->add(DMA_GIF_TAG(sendSize, 1, 0, 0, GIF::DataFormat::image, 0));
      this->add((uint64_t)0);
    this->scTagClose();

    // Send data
    this->scTagOpenRef(source, sendSize);
    this->scTagClose();

    (uint8_t *)source += sendSize * 16;
    qtotal -= sendSize;
  }
}
