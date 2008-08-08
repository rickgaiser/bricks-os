#ifndef PS2_GIF_H
#define PS2_GIF_H


#include "inttypes.h"
#include "dma.h"


namespace GIF
{
  namespace DataFormat
  {
    enum EGIFDataFormat {packed, reglist, image, disable};
  };

  namespace REG
  {
    enum EGIFREG {PRIM, RGBAQ, ST, UV, XYZF2, XYZ2, TEX0_1, TEX0_2, CLAMP_1, CLAMP_2, FOG, RESERVED, XYZF3, XYZ3, AD, NOP};
  };
};


//-------------------------------------------------------------------------
#define DMA_GIF_TAG(QWC, EOP, PRE, PRIM, FLG, NREG) \
  ((uint64_t)(QWC)  <<  0) | \
  ((uint64_t)(EOP)  << 15) | \
  ((uint64_t)(PRE)  << 46) | \
  ((uint64_t)(PRIM) << 47) | \
  ((uint64_t)(FLG)  << 58) | \
  ((uint64_t)(NREG) << 60)


//-------------------------------------------------------------------------
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

  inline void addSetGSReg(uint64_t reg, uint64_t data);
  void addSendImage(const void * source, uint32_t size);

protected:
  void * pGIFTag_;
};


#include "gif.inl"


#endif // PS2_GIF_H
