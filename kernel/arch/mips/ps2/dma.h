#ifndef PS2_DMA_H
#define PS2_DMA_H


#include "inttypes.h"
#include "asm/arch/registers.h"


#define DMA_MAX_QWTRANSFER  (16 * 1024)                  // Max amount of qwords (16 bytes / 128 bits) to transfer  (16K)
#define DMA_MAX_TRANSFER    (DMA_MAX_QWTRANSFER * 16)    // Max amount of bytes to transfer                        (256KiB)

// Start DMA transfer to GS
#define DMA_TO_GS_START(data, size) \
  REG_DMA_GIF_QWC  = DMA_QWC(size); \
  REG_DMA_GIF_MADR = DMA_MADR(data, 0); \
  REG_DMA_GIF_CHCR = DMA_CHCR(1, 0, 0, 0, 0, 1, 0)
// Wait for DMA transfer to complete
#define DMA_WAIT() \
  while(REG_DMA_GIF_CHCR & (1<<8))
// Start DMA transfer to GS, and wait for completion
#define DMA_TO_GS_SEND(data, size) \
  DMA_TO_GS_START(data, size); \
  DMA_WAIT()


#ifdef __cplusplus
extern "C" {
#endif

extern void dma_reset(void);

#ifdef __cplusplus
}
#endif


#endif // PS2_DMA_H
