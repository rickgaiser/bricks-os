#ifndef ASM_ARCH_MACROS_H
#define ASM_ARCH_MACROS_H


#include "inttypes.h"
#include "registers.h"


// DMA copy macros
// -----------------------------------------------------------------------------
#define DMACOPY(channel, source, dest, mode)  \
{                                             \
  REG_DMASAD(channel) = (uint32_t)(source);   \
  REG_DMADAD(channel) = (uint32_t)(dest);     \
  REG_DMACNT(channel) = DMA_ENABLE | (mode);  \
}
#define DMA0COPY(source, dest, mode) DMACOPY(0,(source),(dest),(mode))
#define DMA1COPY(source, dest, mode) DMACOPY(1,(source),(dest),(mode))
#define DMA2COPY(source, dest, mode) DMACOPY(2,(source),(dest),(mode))
#define DMA3COPY(source, dest, mode) DMACOPY(3,(source),(dest),(mode))

// DMA wait macros
// -----------------------------------------------------------------------------
#define DMAWAIT(channel) while(REG_DMACNT(channel) & DMA_BUSY){}
#define DMA0WAIT() DMAWAIT(0)
#define DMA1WAIT() DMAWAIT(1)
#define DMA2WAIT() DMAWAIT(2)
#define DMA3WAIT() DMAWAIT(3)

// Sync DMA functions
// -----------------------------------------------------------------------------
inline void
dmaWait()
{
  DMA3WAIT();
}

inline void
dmaCopy(const void * source, void * dest, uint32_t size)
{
  DMA3WAIT();
  DMA3COPY(source, dest, DMA_DST_INC | DMA_SRC_INC | DMA16 | size>>1);
  DMA3WAIT();
}

inline void
dmaFill16(uint16_t source, void * dest, uint32_t count)
{
  DMA3WAIT();
  DMA3COPY(&source, dest, DMA_DST_INC | DMA_SRC_FIXED | DMA16 | count);
  DMA3WAIT();
}

inline void
dmaFill32(uint32_t source, void * dest, uint32_t count)
{
  DMA3WAIT();
  DMA3COPY(&source, dest, DMA_DST_INC | DMA_SRC_FIXED | DMA32 | count);
  DMA3WAIT();
}

// Async DMA functions
// -----------------------------------------------------------------------------
inline void
dmaCopyASync(const void * source, void * dest, uint32_t size)
{
  DMA3COPY(source, dest, DMA_DST_INC | DMA_SRC_INC | DMA16 | size>>1);
}

inline void
dmaFillASync16(uint16_t source, void * dest, uint32_t count)
{
  DMA3COPY(&source, dest, DMA_DST_INC | DMA_SRC_FIXED | DMA16 | count);
}

inline void
dmaFillASync32(uint32_t source, void * dest, uint32_t count)
{
  DMA3COPY(&source, dest, DMA_DST_INC | DMA_SRC_FIXED | DMA32 | count);
}


#endif
