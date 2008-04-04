#ifndef PS2_GIF_H
#define PS2_GIF_H


#include "inttypes.h"


class CGIFPacket
{
public:
  CGIFPacket(uint64_t size, uint64_t * data = 0);
  ~CGIFPacket();

  inline void reset();
  inline void tag(uint64_t EOP, uint64_t PRE, uint64_t PRIM, uint64_t FLG, uint64_t NREG, uint64_t REGS);
  inline void tagAd(uint64_t EOP, uint64_t PRE, uint64_t PRIM, uint64_t FLG);
  inline void data(uint64_t REG, uint64_t DAT);
  void send();
  void sendImage(uint32_t source, uint32_t size);
  inline void headerSize(uint64_t size);

private:
  uint64_t * pRawData_;    // Not aligned data
  uint64_t * pData_;       // Aligned data
  uint64_t   iPos_;        // Current Data Position
  uint64_t   iDMASize_;    // Current DMA Data Size
  uint64_t   iMAXSize_;    // Maximum data size
  uint64_t   iHeaderSize_; // Size to keep after sending (including tag)
};


#include "gif.inl"


#endif // PS2_GIF_H
