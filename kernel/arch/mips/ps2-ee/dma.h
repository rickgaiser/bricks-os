#ifndef PS2_DMA_H
#define PS2_DMA_H


#include "bios.h"
#include "inttypes.h"
#include "asm/arch/registers.h"


#define DMA_CHANNEL_VIF0        0x00
#define DMA_CHANNEL_VIF1        0x01
#define DMA_CHANNEL_GIF         0x02
#define DMA_CHANNEL_fromIPU     0x03
#define DMA_CHANNEL_toIPU       0x04
#define DMA_CHANNEL_SIF0        0x05
#define DMA_CHANNEL_SIF1        0x06
#define DMA_CHANNEL_SIF2        0x07
#define DMA_CHANNEL_fromSPR     0x08
#define DMA_CHANNEL_toSPR       0x09

//#define DMA_FLAG_NORMAL         0x00
//#define DMA_FLAG_CHAIN          0x01
//#define DMA_FLAG_SCRATCHPAD     0x02
//#define DMA_FLAG_INTERRUPTSAFE  0x04

#define DMA_MAX_QWTRANSFER  (16 * 1024)                  // Max amount of qwords (16 bytes / 128 bits) to transfer  (16K)
#define DMA_MAX_TRANSFER    (DMA_MAX_QWTRANSFER * 16)    // Max amount of bytes to transfer                        (256KiB)


// Initialize the DMA comtroller
inline void dmaInitialize();
// Inititialize DMA channel
inline void dmaInitialize(int channel, dmaCallBack handler);
// Shutdown DMA channel
inline void dmaShutdown  (int channel);
// Send data over channel
inline void dmaSendASync (int channel, void * data, int size);
// Send data over channel, and wait for transfer to complete
inline void dmaSendSync  (int channel, void * data, int size);
// Wait for trasfer to complete
inline void dmaWait      (int channel);


#include "dma.inl"


#endif // PS2_DMA_H
