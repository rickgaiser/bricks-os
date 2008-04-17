#include "gif.h"
#include "dma.h"
#include "bios.h"
#include "asm/arch/registers.h"


//-------------------------------------------------------------------------
CGIFPacket::CGIFPacket(uint64_t size, uint64_t * data)
 : iPos_(0)
 , iDMASize_(0)
 , iHeaderSize_(0)
{
  // Clip max size to 0x7fff (32767)
  iMAXSize_ = (size <= 0x7fff) ? size : 0x7fff;

  if(data != 0)
  {
    pData_ = data;
  }
  else
  {
    // Determine how much memory to allocate
    uint64_t allocSize = (iMAXSize_ * 2) + 2;

    // Allocate data
    pRawData_ = (uint64_t *)new uint8_t[(allocSize * 16) + (64 - 1)];

    // Align to 64 byte boundry for DMA
    pData_    = (uint64_t *)((((uint32_t)pRawData_) + (64 - 1)) & (~(64 - 1)));
  }
}

//-------------------------------------------------------------------------
CGIFPacket::~CGIFPacket()
{
  if(pRawData_ != 0)
    delete pRawData_;
}

//-------------------------------------------------------------------------
void
CGIFPacket::send()
{
  // Total send size of data
  pData_[0] |= iDMASize_ - 1; // Data size (DMA - tag)

  // Flush caches before transfer
  bios::FlushCache(0);

  // Send
  dmaSendSync(DMA_CHANNEL_GIF, pData_, iDMASize_);

  // Reset packet information but preserve tag
  pData_[0] &= (~0x7fff);
  iPos_      = iHeaderSize_ << 1;
  iDMASize_  = iHeaderSize_;
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
    pData_[0] &= (~0x7fff);
    pData_[0] |= sendSize;

    // Flush caches before transfer
    bios::FlushCache(0);

    // Send tag
    dmaSendSync(DMA_CHANNEL_GIF, pData_, 1);

    // Send data
    dmaSendSync(DMA_CHANNEL_GIF, (void *)source, sendSize);

    source += sendSize * 16;
    qtotal -= sendSize;
  }

  this->reset();
}
