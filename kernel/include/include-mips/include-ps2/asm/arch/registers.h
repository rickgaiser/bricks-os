#ifndef ASM_ARCH_REGISTERS_H
#define ASM_ARCH_REGISTERS_H


#include "inttypes.h"


// Playstation2 GS General Purpose Registers
#define prim            0x00 // Select and configure current drawing primitive
#define rgbaq           0x01 // Setup current vertex color
#define st              0x02 // ...
#define uv              0x03 // Specify Vertex Texture Coordinates
#define xyzf2           0x04 // Set vertex coordinate
#define xyz2            0x05 // Set vertex coordinate and 'kick' drawing
#define tex0_1          0x06 // Texture Buffer Setup (Context 1)
#define tex0_2          0x07 // Texture Buffer Setup (Context 2)
#define clamp_1         0x08 // ...
#define clamp_2         0x09 // ...
#define fog             0x0a // ...
#define xyzf3           0x0c // ...
#define xyz3            0x0d // ...
#define tex1_1          0x14 // ...
#define tex1_2          0x15 // ...
#define tex2_1          0x16 // ...
#define tex2_2          0x17 // ...
#define xyoffset_1      0x18 // Mapping from Primitive to Window coordinate system (Context 1)
#define xyoffset_2      0x19 // Mapping from Primitive to Window coordinate system (Context 2)
#define prmodecont      0x1a // ...
#define prmode          0x1b // ...
#define texclut         0x1c // ...
#define scanmsk         0x22 // ...
#define miptbp1_1       0x34 // ...
#define miptbp1_2       0x35 // ...
#define miptbp2_1       0x36 // ...
#define miptbp2_2       0x37 // ...
#define texa            0x3b // ...
#define fogcol          0x3d // ...
#define texflush        0x3f // Write to this register before using newly loaded texture
#define scissor_1       0x40 // Setup clipping rectangle (Context 1)
#define scissor_2       0x41 // Setup clipping rectangle (Context 2)
#define alpha_1         0x42 // Setup Alpha Blending Parameters (Context 1)
#define alpha_2         0x43 // Setup Alpha Blending Parameters (Context 2)
#define dimx            0x44 // ...
#define dthe            0x45 // ...
#define colclamp        0x46 // ...
#define test_1          0x47 // ...
#define test_2          0x48 // ...
#define pabe            0x49 // ...
#define fba_1           0x4a // ...
#define fba_2           0x4b // ...
#define frame_1         0x4c // Frame buffer settings (Context 1)
#define frame_2         0x4d // Frame buffer settings (Context 2)
#define zbuf_1          0x4e // ...
#define zbuf_2          0x4f // ...
#define bitbltbuf       0x50 // Setup Image Transfer Between EE and GS
#define trxpos          0x51 // Setup Image Transfer Coordinates
#define trxreg          0x52 // Setup Image Transfer Size
#define trxdir          0x53 // Set Image Transfer Directon + Start Transfer
#define hwreg           0x54 // ...
#define signal          0x60 // ...
#define finish          0x61 // ...
#define label           0x62 // ...

// Timer registers
// FIXME: Really 64bit registers?
#define REG_TIMER0_COUNT      (*(vuint64_t *)(0x10000000))
#define REG_TIMER0_MODE       (*(vuint64_t *)(0x10000010))
#define REG_TIMER0_COMP       (*(vuint64_t *)(0x10000020))
#define REG_TIMER0_HOLD       (*(vuint64_t *)(0x10000030))
#define REG_TIMER1_COUNT      (*(vuint64_t *)(0x10000800))
#define REG_TIMER1_MODE       (*(vuint64_t *)(0x10000810))
#define REG_TIMER1_COMP       (*(vuint64_t *)(0x10000820))
#define REG_TIMER1_HOLD       (*(vuint64_t *)(0x10000830))
#define REG_TIMER2_COUNT      (*(vuint64_t *)(0x10001000))
#define REG_TIMER2_MODE       (*(vuint64_t *)(0x10001010))
#define REG_TIMER2_COMP       (*(vuint64_t *)(0x10001020))
#define REG_TIMER3_COUNT      (*(vuint64_t *)(0x10001800))
#define REG_TIMER3_MODE       (*(vuint64_t *)(0x10001810))
#define REG_TIMER3_COMP       (*(vuint64_t *)(0x10001820))

// Graphics InterFace (GIF) registers
#define REG_GIF_CTRL          (*(vuint32_t *)(0x10003000)) // Control Register
#define REG_GIF_MODE          (*(vuint32_t *)(0x10003010)) // Mode Setting Register
#define REG_GIF_STAT          (*(vuint32_t *)(0x10003020)) // Status Register
#define REG_GIF_TAG0          (*(vuint32_t *)(0x10003040)) // GIFtag Save Register
#define REG_GIF_TAG1          (*(vuint32_t *)(0x10003050)) // GIFtag Save Register
#define REG_GIF_TAG2          (*(vuint32_t *)(0x10003060)) // GIFtag Save Register
#define REG_GIF_TAG3          (*(vuint32_t *)(0x10003070)) // GIFtag Save Register
#define REG_GIF_CNT           (*(vuint32_t *)(0x10003080)) // Count Register
#define REG_GIF_P3CNT         (*(vuint32_t *)(0x10003090)) // PATH3 Count Register
#define REG_GIF_P3TAG         (*(vuint32_t *)(0x100030a0)) // PATH3 Tag Register

// Graphics InterFace (GIF) DMA channel registers
#define REG_GIF_CHCR          (*(vuint32_t *)(0x1000a000)) // GIF Channel Control Register
#define REG_GIF_MADR          (*(vuint32_t *)(0x1000a010)) // Transfer Address Register
#define REG_GIF_QWC           (*(vuint32_t *)(0x1000a020)) // Transfer Size Register (in qwords)
#define REG_GIF_TADR          (*(vuint32_t *)(0x1000a030))

// DMA registers
#define REG_DMA_CTRL          (*(vuint32_t *)(0x1000e000)) // DMA Control Register
#define REG_DMA_STAT          (*(vuint32_t *)(0x1000e010)) // Interrupt Status Register
#define REG_DMA_PCR           (*(vuint32_t *)(0x1000e020)) // Priority Control Register
#define REG_DMA_SQWC          (*(vuint32_t *)(0x1000e030)) // Interleave Size Register
#define REG_DMA_RBSR          (*(vuint32_t *)(0x1000e040)) // Ring Buffer Size Register
#define REG_DMA_RBOR          (*(vuint32_t *)(0x1000e050)) // Ring Buffer Address Register
#define REG_DMA_STADR         (*(vuint32_t *)(0x1000e060)) // Stall Address Register
#define REG_DMA_ENABLER       (*(vuint32_t *)(0x1000f520)) // DMA Hold State Register
#define REG_DMA_ENABLEW       (*(vuint32_t *)(0x1000f590)) // DMA Hold Control Register

// Graphics Synthesizer (GS) privileged registers
#define REG_GS_PMODE          (*(vuint64_t *)(0x12000000)) // Setup CRT Controller
#define REG_GS_SMODE1         (*(vuint64_t *)(0x12000010)) // VHP,VCKSEL,SLCK2,NVCK,CLKSEL,PEVS,PEHS,PVS,PHS,GCONT,SPML,PCK2,XPCK,SINT,PRST,EX,CMOD,SLCK,T1248,LC,RC
#define REG_GS_SMODE2         (*(vuint64_t *)(0x12000020)) // CRTC Video Settings: PAL/NTCS, Interlace, etc.
#define REG_GS_SRFSH          (*(vuint64_t *)(0x12000030)) // DRAM Refresh Settings
#define REG_GS_SYNCH1         (*(vuint64_t *)(0x12000040)) // HS,HSVS,HSEQ,HBP,HFP
#define REG_GS_SYNCH2         (*(vuint64_t *)(0x12000050)) // HB,HF
#define REG_GS_SYNCHV         (*(vuint64_t *)(0x12000060)) // VS,VDP,VBPE,VBP,VFPE,VFP
#define REG_GS_DISPFB1        (*(vuint64_t *)(0x12000070)) // Setup the CRTC's Read Circuit 1 data source settings
#define REG_GS_DISPLAY1       (*(vuint64_t *)(0x12000080)) // RC1 display output settings
#define REG_GS_DISPFB2        (*(vuint64_t *)(0x12000090)) // Setup the CRTC's Read Circuit 2 data source settings
#define REG_GS_DISPLAY2       (*(vuint64_t *)(0x120000a0)) // RC2 display output settings
#define REG_GS_EXTBUF         (*(vuint64_t *)(0x120000b0))
#define REG_GS_EXTDATA        (*(vuint64_t *)(0x120000c0))
#define REG_GS_EXTWRITE       (*(vuint64_t *)(0x120000d0))
#define REG_GS_BGCOLOR        (*(vuint64_t *)(0x120000e0)) // Set CRTC background color
#define REG_GS_CSR            (*(vuint64_t *)(0x12001000)) // System status and reset
#define REG_GS_IMR            (*(vuint64_t *)(0x12001010)) // Interrupt Mask Register
#define REG_GS_BUSDIR         (*(vuint64_t *)(0x12001040))
#define REG_GS_SIGLBLID       (*(vuint64_t *)(0x12001080))

//
#define REG_INT_STAT          (*(vuint64_t *)(0x1000f000)) // Interrupt status
#define REG_INT_MASK          (*(vuint64_t *)(0x1000f010)) // Interrupt mask
#define REG_SBUS_SMFLG        (*(vuint64_t *)(0x1000f230))


#endif
