#ifndef ARCH_REGISTERS_H
#define ARCH_REGISTERS_H


#include "inttypes.h"


// "BIOS_TV_MODE"
enum EBiosTVMode
{
  BTM_NTSC      = 0,
  BTM_PAL       = 1,
  BTM_DEBUG     = 2,
  BTM_DEBUG_PAL = 3,
  BTM_MPAL      = 4,
  BTM_PAL60     = 5
};

// "REG_SI_CHANNEL?_INBUFH" bits
#define KEY_START                 0x10000000
#define KEY_Y                     0x08000000
#define KEY_X                     0x04000000
#define KEY_B                     0x02000000
#define KEY_A                     0x01000000
#define KEY_L                     0x00200000
#define KEY_R                     0x00100000
#define KEY_UP                    0x00080000
#define KEY_DOWN                  0x00040000
#define KEY_LEFT                  0x00020000
#define KEY_RIGHT                 0x00010000
//#define KEY_X_MASK                0x0000ff00
//#define KEY_Y_MASK                0x000000ff

// "REG_SI_CHANNEL?_INBUFL" bits
//#define KEY_X_MASK                0xff000000
//#define KEY_Y_MASK                0x00ff0000
//#define KEY_L_MASK                0x0000ff00
//#define KEY_R_MASK                0x000000ff

//---------------------------------------------------------------------------
// Register Constructors
//---------------------------------------------------------------------------
// Vertical Timing
#define VI_VTR(height,equ) \
  (((height) << 4) | (equ))

// Display Configuration
#define VI_DCR_NTSC 0
#define VI_DCR_PAL  1
#define VI_DCR_MPAL 2
#define VI_DCR_ENABLE       0x01
#define VI_DCR_RESET        0x02
#define VI_DCR_PROGRESSIVE  0x04
#define VI_DCR_ENABLE_3D    0x08
#define VI_DCR(mode,flags) \
  (((mode) << 8) | (flags))

// Horizontal Timing 0
#define VI_HTR0(s,e,w) \
  (((s) << 24) | ((e) << 16) | (w))

// Horizontal Timing 1
#define VI_HTR1(s,e,w) \
  (((s) << 17) | ((e) << 7) | (w))
#define VI_HTR1_CREATE(hbs,hbe,hsw,pos,width) \
  VI_HTR1(hbs + pos - (720 - width), hbe + pos, hsw)

// Odd/Even Field Vertical Timing
#define VI_VT(postbl,prebl) \
  (((postbl) << 16) | (prebl))

// Odd/Even Burst Blanking Interval
#define VI_BBI(end2,start2,end1,start1) \
  (((end2) << 21) | ((start2) << 16) | ((end1) << 5) | (start1))

// Horizontal Scaling
#define VI_HSR_ENABLE          (1<<12)
#define VI_HSR_SCALE(from,to)  ((uint16_t)((((float)(from))/((float)(to)))*(1<<8)))  // Convert float to 1.8 fixed point scaling value

//VI Clock Select
#define VI_CLK_27MHZ 0
#define VI_CLK_54MHZ 1

//---------------------------------------------------------------------------
// Registers
//---------------------------------------------------------------------------
#define BIOS_TV_MODE              (*(vuint32_t*)0x800000cc)

#define REG_AI_DMA_STARTH         (*(vuint16_t*)0xcc005030)
#define REG_AI_DMA_STARTL         (*(vuint16_t*)0xcc005032)
#define REG_AI_DMA_LENGTH         (*(vuint16_t*)0xcc005036)
#define REG_AI_STREAM_STATUS      (*(vuint32_t*)0xcc006C00)
#define REG_AI_DSP_STATUS         (*(vuint16_t*)0xcc00500a)

#define REG_AR_DMA_MMADDR_H       (*(vuint16_t*)0xcc005020)
#define REG_AR_DMA_MMADDR_L       (*(vuint16_t*)0xcc005022)
#define REG_AR_DMA_ARADDR_H       (*(vuint16_t*)0xcc005024)
#define REG_AR_DMA_ARADDR_L       (*(vuint16_t*)0xcc005026)
#define REG_AR_DMA_CNT_H          (*(vuint16_t*)0xcc005028)
#define REG_AR_DMA_CNT_L          (*(vuint16_t*)0xcc00502a)

#define REG_EXI_BASE              ( (vuint32_t*)0xcc006800)
#define REG_EXI_STATUS0           (*(vuint32_t*)0xcc006800)
#define REG_EXI_DMABUF0           (*(vuint32_t*)0xcc006804)
#define REG_EXI_DMALEN0           (*(vuint32_t*)0xcc006808)
#define REG_EXI_DMACNT0           (*(vuint32_t*)0xcc00680c)
#define REG_EXI_IMMDAT0           (*(vuint32_t*)0xcc006810)
#define REG_EXI_STATUS1           (*(vuint32_t*)0xcc006814)
#define REG_EXI_DMABUF1           (*(vuint32_t*)0xcc006818)
#define REG_EXI_DMALEN1           (*(vuint32_t*)0xcc00681c)
#define REG_EXI_DMACNT1           (*(vuint32_t*)0xcc006820)
#define REG_EXI_IMMDAT1           (*(vuint32_t*)0xcc006824)
#define REG_EXI_STATUS2           (*(vuint32_t*)0xcc006828)
#define REG_EXI_DMABUF2           (*(vuint32_t*)0xcc00682c)
#define REG_EXI_DMALEN2           (*(vuint32_t*)0xcc006830)
#define REG_EXI_DMACNT2           (*(vuint32_t*)0xcc006834)
#define REG_EXI_IMMDAT2           (*(vuint32_t*)0xcc006838)

#define REG_INTERRUPT_CAUSE       (*(vuint32_t*)0xcc003000)
#define REG_INTERRUPT_MASK        (*(vuint32_t*)0x800000c8)
#define REG_INTERRUPT_CAUSE_CACHE (*(vuint32_t*)0x800000c4)
#define REG_INTERRUPT_MASK_CACHE  (*(vuint32_t*)0x800000c8)

// SI (Serial Interface, 4 ports at the front)
#define REG_SI_BASE               ( (vuint32_t*)0xcc006400)
#define REG_SI_CHANNEL0_BASE      ( (vuint32_t*)0xcc006400)
#define REG_SI_CHANNEL0_OUTBUF    (*(vuint32_t*)0xcc006400)
#define REG_SI_CHANNEL0_INBUFH    (*(vuint32_t*)0xcc006404)
#define REG_SI_CHANNEL0_INBUFL    (*(vuint32_t*)0xcc006408)
#define REG_SI_CHANNEL1_BASE      ( (vuint32_t*)0xcc00640c)
#define REG_SI_CHANNEL1_OUTBUF    (*(vuint32_t*)0xcc00640c)
#define REG_SI_CHANNEL1_INBUFH    (*(vuint32_t*)0xcc006410)
#define REG_SI_CHANNEL1_INBUFL    (*(vuint32_t*)0xcc006414)
#define REG_SI_CHANNEL2_BASE      ( (vuint32_t*)0xcc006418)
#define REG_SI_CHANNEL2_OUTBUF    (*(vuint32_t*)0xcc006418)
#define REG_SI_CHANNEL2_INBUFH    (*(vuint32_t*)0xcc00641c)
#define REG_SI_CHANNEL2_INBUFL    (*(vuint32_t*)0xcc006420)
#define REG_SI_CHANNEL3_BASE      ( (vuint32_t*)0xcc006424)
#define REG_SI_CHANNEL3_OUTBUF    (*(vuint32_t*)0xcc006424)
#define REG_SI_CHANNEL3_INBUFH    (*(vuint32_t*)0xcc006428)
#define REG_SI_CHANNEL3_INBUFL    (*(vuint32_t*)0xcc00642c)
#define REG_SI_POLL               (*(vuint32_t*)0xcc006430)
#define REG_SI_COMCSR             (*(vuint32_t*)0xcc006434)
#define REG_SI_STATUS             (*(vuint32_t*)0xcc006438)
#define REG_SI_EXILK              (*(vuint32_t*)0xcc00643c)
#define REG_SI_IO_BUFFER          (*(vuint32_t*)0xcc006480)

#define REG_GC_System_Time        (*(vuint32_t*)0x800030d8)

// VI - Video Interface
#define REG_VI_VTR                (*(vuint16_t*)0xcc002000) // Vertical Timing
#define REG_VI_DCR                (*(vuint16_t*)0xcc002002) // Display Configuration
#define REG_VI_HTR0               (*(vuint32_t*)0xcc002004) // Horizontal Timing 0
#define REG_VI_HTR1               (*(vuint32_t*)0xcc002008) // Horizontal Timing 0
#define REG_VI_VTO                (*(vuint32_t*)0xcc00200c) // Odd Field Vertical Timing
#define REG_VI_VTE                (*(vuint32_t*)0xcc002010) // Even Field Vertical Timing
#define REG_VI_BBIO               (*(vuint32_t*)0xcc002014) // Odd Field Burst Blanking Interval
#define REG_VI_BBIE               (*(vuint32_t*)0xcc002018) // Even Field Burst Blanking Interval
#define REG_VI_TFBL               (*(vuint32_t*)0xcc00201c) // Top Field Base Register (L) (External Framebuffer Half 1)
#define REG_VI_TFBR               (*(vuint32_t*)0xcc002020) // Top Field Base Register (R) (Only valid in 3D Mode)
#define REG_VI_BFBL               (*(vuint32_t*)0xcc002024) // Bottom Field Base Register (L) (External Framebuffer Half 2)
#define REG_VI_BFBR               (*(vuint32_t*)0xcc002028) // Bottom Field Base Register (R) (Only valid in 3D Mode)
#define REG_VI_DPV                (*(vuint16_t*)0xcc00202c) // Current Vertical Position
#define REG_VI_DPH                (*(vuint16_t*)0xcc00202e) // Current Horizontal Position
#define REG_VI_DI0                (*(vuint32_t*)0xcc002030) // Display Interrupt 0
#define REG_VI_DI1                (*(vuint32_t*)0xcc002034) // Display Interrupt 1
#define REG_VI_DI2                (*(vuint32_t*)0xcc002038) // Display Interrupt 2
#define REG_VI_DI3                (*(vuint32_t*)0xcc00203c) // Display Interrupt 3
#define REG_VI_DL0                (*(vuint32_t*)0xcc002040) // Display Latch 0
#define REG_VI_DL1                (*(vuint32_t*)0xcc002044) // Display Latch 1
#define REG_VI_HSW                (*(vuint16_t*)0xcc002048) // Scaling Width
#define REG_VI_HSR                (*(vuint16_t*)0xcc00204a) // Horizontal Scaling
#define REG_VI_FTC0               (*(vuint32_t*)0xcc00204c) // Filter Coefficient Table 0
#define REG_VI_FTC1               (*(vuint32_t*)0xcc002050) // Filter Coefficient Table 1
#define REG_VI_FTC2               (*(vuint32_t*)0xcc002054) // Filter Coefficient Table 2
#define REG_VI_FTC3               (*(vuint32_t*)0xcc002058) // Filter Coefficient Table 3
#define REG_VI_FTC4               (*(vuint32_t*)0xcc00205c) // Filter Coefficient Table 4
#define REG_VI_FTC5               (*(vuint32_t*)0xcc002060) // Filter Coefficient Table 5
#define REG_VI_FTC6               (*(vuint32_t*)0xcc002064) // Filter Coefficient Table 6
//#define REG_VI_                   (*(vuint32_t*)0xcc002068) // ???
#define REG_VI_CLK              (*(vuint16_t*)0xcc00206c) // Clock Select (for progressive mode)
#define REG_VI_SEL              (*(vuint16_t*)0xcc00206e) // DTV Status (read io pins)
//#define REG_VI_                   (*(vuint16_t*)0xcc002070) // ??
#define REG_VI_HBE                (*(vuint16_t*)0xcc002072) // Border Horizontal Blank End (for debug mode)
#define REG_VI_HBS                (*(vuint16_t*)0xcc002074) // Border Horizontal Blank Start (for debug mode)
//#define REG_VI_                   (*(vuint16_t*)0xcc002076) // ???
//#define REG_VI_                   (*(vuint32_t*)0xcc002078) // ???
//#define REG_VI_                   (*(vuint32_t*)0xcc00207c) // ???
// For compatibility
#define REG_VI_BASE               ( (vuint32_t*)0xcc002000)
#define REG_VI_XFB1               REG_VI_TFBL
#define REG_VI_XFB2               REG_VI_BFBL
#define REG_VI_HLINE              REG_VI_DPV


#endif
