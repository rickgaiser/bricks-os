#ifndef ARCH_REGISTERS_H
#define ARCH_REGISTERS_H


#include "inttypes.h"


// "REG_SI_CHANNEL?_INBUFH" bits
#define KEY_UP    0x00080000
#define KEY_DOWN  0x00040000
#define KEY_START 0x10000000


#define REG_AI_DMA_STARTH     (*(vuint16_t*)0xcc005030)
#define REG_AI_DMA_STARTL     (*(vuint16_t*)0xcc005032)
#define REG_AI_DMA_LENGTH     (*(vuint16_t*)0xcc005036)
#define REG_AI_STREAM_STATUS  (*(vuint32_t*)0xcc006C00)
#define REG_AI_DSP_STATUS     (*(vuint16_t*)0xcc00500a)

#define REG_AR_DMA_MMADDR_H   (*(vuint16_t*)0xcc005020)
#define REG_AR_DMA_MMADDR_L   (*(vuint16_t*)0xcc005022)
#define REG_AR_DMA_ARADDR_H   (*(vuint16_t*)0xcc005024)
#define REG_AR_DMA_ARADDR_L   (*(vuint16_t*)0xcc005026)
#define REG_AR_DMA_CNT_H      (*(vuint16_t*)0xcc005028)
#define REG_AR_DMA_CNT_L      (*(vuint16_t*)0xcc00502a)

#define REG_EXI_BASE          ( (vuint32_t*)0xcc006800)
#define REG_EXI_STATUS0       (*(vuint32_t*)0xcc006800)
#define REG_EXI_DMABUF0       (*(vuint32_t*)0xcc006804)
#define REG_EXI_DMALEN0       (*(vuint32_t*)0xcc006808)
#define REG_EXI_DMACNT0       (*(vuint32_t*)0xcc00680c)
#define REG_EXI_IMMDAT0       (*(vuint32_t*)0xcc006810)
#define REG_EXI_STATUS1       (*(vuint32_t*)0xcc006814)
#define REG_EXI_DMABUF1       (*(vuint32_t*)0xcc006818)
#define REG_EXI_DMALEN1       (*(vuint32_t*)0xcc00681c)
#define REG_EXI_DMACNT1       (*(vuint32_t*)0xcc006820)
#define REG_EXI_IMMDAT1       (*(vuint32_t*)0xcc006824)
#define REG_EXI_STATUS2       (*(vuint32_t*)0xcc006828)
#define REG_EXI_DMABUF2       (*(vuint32_t*)0xcc00682c)
#define REG_EXI_DMALEN2       (*(vuint32_t*)0xcc006830)
#define REG_EXI_DMACNT2       (*(vuint32_t*)0xcc006834)
#define REG_EXI_IMMDAT2       (*(vuint32_t*)0xcc006838)

#define REG_INTERRUPT_CAUSE   (*(vuint32_t*)0xcc003000)
#define REG_INTERRUPT_MASK    (*(vuint32_t*)0x800000C8)
#define REG_INTERRUPT_CAUSE_CACHE (*(vuint32_t*)0x800000c4)
#define REG_INTERRUPT_MASK_CACHE (*(vuint32_t*)0x800000c8)

// SI (Serial Interface, 4 ports at the front)
#define REG_SI_BASE             ( (vuint32_t*)0xcc006400)
#define REG_SI_CHANNEL0_BASE    ( (vuint32_t*)0xcc006400)
#define REG_SI_CHANNEL0_OUTBUF  (*(vuint32_t*)0xcc006400)
#define REG_SI_CHANNEL0_INBUFH  (*(vuint32_t*)0xcc006404)
#define REG_SI_CHANNEL0_INBUFL  (*(vuint32_t*)0xcc006408)
#define REG_SI_CHANNEL1_BASE    ( (vuint32_t*)0xcc00640c)
#define REG_SI_CHANNEL1_OUTBUF  (*(vuint32_t*)0xcc00640c)
#define REG_SI_CHANNEL1_INBUFH  (*(vuint32_t*)0xcc006410)
#define REG_SI_CHANNEL1_INBUFL  (*(vuint32_t*)0xcc006414)
#define REG_SI_CHANNEL2_BASE    ( (vuint32_t*)0xcc006418)
#define REG_SI_CHANNEL2_OUTBUF  (*(vuint32_t*)0xcc006418)
#define REG_SI_CHANNEL2_INBUFH  (*(vuint32_t*)0xcc00641c)
#define REG_SI_CHANNEL2_INBUFL  (*(vuint32_t*)0xcc006420)
#define REG_SI_CHANNEL3_BASE    ( (vuint32_t*)0xcc006424)
#define REG_SI_CHANNEL3_OUTBUF  (*(vuint32_t*)0xcc006424)
#define REG_SI_CHANNEL3_INBUFH  (*(vuint32_t*)0xcc006428)
#define REG_SI_CHANNEL3_INBUFL  (*(vuint32_t*)0xcc00642c)
#define REG_SI_POLL             (*(vuint32_t*)0xcc006430)
#define REG_SI_COMCSR           (*(vuint32_t*)0xcc006434)
#define REG_SI_STATUS           (*(vuint32_t*)0xcc006438)
#define REG_SI_EXILK            (*(vuint32_t*)0xcc00643c)
#define REG_SI_IO_BUFFER        (*(vuint32_t*)0xcc006480)

#define REG_GC_System_Time    (*(vuint32_t*)0x800030d8)

#define REG_VI_BASE           ( (vuint32_t*)0xcc002000)
#define REG_VI_XFB1           (*(vuint32_t*)0xcc00201c)
#define REG_VI_XFB2           (*(vuint32_t*)0xcc002024)
#define REG_VI_HLINE          (*(vint16_t *)0xcc00202c)


#endif
