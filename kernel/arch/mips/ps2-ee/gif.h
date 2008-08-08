#ifndef PS2_GIF_H
#define PS2_GIF_H


#include "inttypes.h"
#include "dma.h"


class CGIFPacket
 : public CSCDMAPacket
{
public:
  CGIFPacket(uint32_t qwSize, EDMAChannel channelId);
  CGIFPacket(void * buffer, uint32_t qwSize, EDMAChannel channelId);
  ~CGIFPacket();

  // Reset packet
  inline void reset();

  // Open GIF Tag
  inline CGIFPacket & gifTagOpen();
  // Close GIF Tag
  inline CGIFPacket & gifTagClose();

  inline void tag(uint64_t EOP, uint64_t PRE, uint64_t PRIM, uint64_t FLG, uint64_t NREG, uint64_t REGS);
  inline void tagAd(uint64_t EOP, uint64_t PRE, uint64_t PRIM, uint64_t FLG);
  inline void addSetGSReg(uint64_t reg, uint64_t data);
  void sendImage(uint32_t source, uint32_t size);

protected:
  void * pGIFTag_;
};


#include "gif.inl"


#endif // PS2_GIF_H
