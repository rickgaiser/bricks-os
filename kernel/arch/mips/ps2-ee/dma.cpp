#include "dma.h"


//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
CDMAPacket::CDMAPacket(uint32_t qwSize, EDMAChannel channelId)
 : bHasOwnership_(true)
 , eChannelId_(channelId)
{
  // Clip qwSize to DMA_MAX_QWTRANSFER
  if(qwSize > DMA_MAX_QWTRANSFER)
    qwSize = DMA_MAX_QWTRANSFER;
  iQWSize_ = qwSize;

  // Allocate data (+63 bytes for alignment)
  pRawData_ = new uint8_t[(qwSize * 16) + (64 - 1)];

  // Align to 64 byte boundry
  pData_    = (uint8_t *)((((uint32_t)pRawData_) + (64 - 1)) & (~(64 - 1)));
  pCurrent_ = pData_;
  pEnd_     = pData_ + (qwSize * 16);
}

//-------------------------------------------------------------------------
CDMAPacket::CDMAPacket(void * buffer, uint32_t qwSize, EDMAChannel channelId)
 : pRawData_((uint8_t *)buffer)
 , bHasOwnership_(true)
 , eChannelId_(channelId)
{
  // Clip qwSize to DMA_MAX_QWTRANSFER
  if(qwSize > DMA_MAX_QWTRANSFER)
    qwSize = DMA_MAX_QWTRANSFER;
  iQWSize_ = qwSize;

  // Align to 64 byte boundry
  pData_    = (uint8_t *)((((uint32_t)pRawData_) + (64 - 1)) & (~(64 - 1)));
  pCurrent_ = pData_;
  pEnd_     = pData_ + (qwSize * 16);
}

//-------------------------------------------------------------------------
CDMAPacket::~CDMAPacket()
{
  if((bHasOwnership_ == true) && (pRawData_ != NULL))
    delete pRawData_;
}

//-------------------------------------------------------------------------
void
CDMAPacket::send(bool waitComplete)
{
  // Reset total send size of data
  ((uint64_t *)pData_)[0] &= (~0x7fff);
  // Calculate amount of qwords to transfer
  uint32_t size = ((uint32_t)pCurrent_ - (uint32_t)pData_) / 16;
  // Set total send size of data (-1)
  ((uint64_t *)pData_)[0] |= size - 1;

  // Flush caches before transfer
  bios::FlushCache(0);

  // Send
  dmaSend(eChannelId_, pData_, size);

  // Wait for completion
  if(waitComplete == true)
    dmaWait(eChannelId_);
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
CSCDMAPacket::CSCDMAPacket(uint32_t qwSize, EDMAChannel channelId)
 : CDMAPacket(qwSize, channelId)
{
}

//-------------------------------------------------------------------------
CSCDMAPacket::CSCDMAPacket(void * buffer, uint32_t qwSize, EDMAChannel channelId)
 : CDMAPacket(buffer, qwSize, channelId)
{
}

//-------------------------------------------------------------------------
CSCDMAPacket::~CSCDMAPacket()
{
}