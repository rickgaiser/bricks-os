#ifndef ASM_ARCH_MACROS_H
#define ASM_ARCH_MACROS_H


#include "inttypes.h"
#include "registers.h"


// DMA busy macros
// -----------------------------------------------------------------------------
#define DMABUSY(channel) (REG_DMACNT(channel) & DMA_BUSY)
#define DMA0BUSY() DMABUSY(0)
#define DMA1BUSY() DMABUSY(1)
#define DMA2BUSY() DMABUSY(2)
#define DMA3BUSY() DMABUSY(3)
inline bool dmaBusy(){return DMA3BUSY();}

// DMA wait macros
// -----------------------------------------------------------------------------
#define DMAWAIT(channel) while(DMABUSY(channel)){}
#define DMA0WAIT() DMAWAIT(0)
#define DMA1WAIT() DMAWAIT(1)
#define DMA2WAIT() DMAWAIT(2)
#define DMA3WAIT() DMAWAIT(3)
inline void dmaWait(){DMA3WAIT();}

// DMA copy macros
// -----------------------------------------------------------------------------
#define DMACOPY(channel, source, dest, mode)   \
{                                              \
  REG_DMASAD(channel) = (uint32_t)(source);    \
  REG_DMADAD(channel) = (uint32_t)(dest);      \
  REG_DMACNT(channel) = DMA_ENABLE | (mode);   \
}
#define DMA0COPY(source, dest, mode) DMACOPY(0, source, dest, mode)
#define DMA1COPY(source, dest, mode) DMACOPY(1, source, dest, mode)
#define DMA2COPY(source, dest, mode) DMACOPY(2, source, dest, mode)
#define DMA3COPY(source, dest, mode) DMACOPY(3, source, dest, mode)

inline void
dmaCopy(const void * source, void * dest, uint32_t size)
{
  DMA3WAIT();
  DMA3COPY(source, dest, DMA_DST_INC | DMA_SRC_INC | DMA16 | size>>1);
}

// DMA fill macros
// -----------------------------------------------------------------------------
#ifndef NDS9
#define DMAFILL(channel, data, dest, mode)     \
{                                              \
  static uint32_t tmp;                         \
  tmp = data;                                  \
  REG_DMASAD(channel)  = (uint32_t)(&tmp);     \
  REG_DMADAD(channel)  = (uint32_t)(dest);     \
  REG_DMACNT(channel)  = DMA_ENABLE | (mode);  \
}
#else
#define DMAFILL(channel, data, dest, mode)     \
{                                              \
  REG_DMAFILL(channel) = data;                 \
  REG_DMASAD(channel)  = (uint32_t)(&REG_DMAFILL(channel)); \
  REG_DMADAD(channel)  = (uint32_t)(dest);     \
  REG_DMACNT(channel)  = DMA_ENABLE | (mode);  \
}
#endif // NDS9
#define DMA0FILL(data, dest, mode) DMAFILL(0, data, dest, mode)
#define DMA1FILL(data, dest, mode) DMAFILL(1, data, dest, mode)
#define DMA2FILL(data, dest, mode) DMAFILL(2, data, dest, mode)
#define DMA3FILL(data, dest, mode) DMAFILL(3, data, dest, mode)

inline void
dmaFill16(uint16_t data, void * dest, uint32_t count)
{
#ifdef GBA
  DMA3WAIT();
  DMA3FILL(data, dest, DMA_DST_INC | DMA_SRC_FIXED | DMA16 | count);
#else
  for(uint32_t i(0); i < count; i++)
    ((uint16_t *)dest)[i] = data;
#endif // GBA
}

inline void
dmaFill32(uint32_t data, void * dest, uint32_t count)
{
#ifdef GBA
  DMA3WAIT();
  DMA3FILL(data, dest, DMA_DST_INC | DMA_SRC_FIXED | DMA32 | count);
#else
  for(uint32_t i(0); i < count; i++)
    ((uint32_t *)dest)[i] = data;
#endif // GBA
}


#endif
