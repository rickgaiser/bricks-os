/*
 * Bricks-OS, Operating System for Game Consoles
 * Copyright (C) 2008 Maximus32 <Maximus32@bricks-os.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA
 */


#ifndef ASM_ARCH_REGISTERS_H
#define ASM_ARCH_REGISTERS_H


#include "stdint.h"
#include "asm/cpu.h"
#include "asm/arch/config.h"


#ifdef CONFIG_KERNEL_MODE
  // KSEG1_START: Kernel segment 1, Unmapped, Uncached
  #define REG32(R) (*(vuint32_t *)((R) | KSEG1_START))
  #define REG64(R) (*(vuint64_t *)((R) | KSEG1_START))
#else
  // KUSEG_START: User segment: Mapped
  #define REG32(R) (*(vuint32_t *)((R) | KUSEG_START))
  #define REG64(R) (*(vuint64_t *)((R) | KUSEG_START))
#endif

//
// Timers
//
// FIXME: Really 64bit registers?
typedef struct
{
  vuint64_t count;
  vuint64_t pad0;
  vuint64_t mode;
  vuint64_t pad1;
  vuint64_t comp;
  vuint64_t pad2;
  vuint64_t hold;
  vuint64_t pad3;
} STimerReg;
// Timer0
#define REG_TIMER0_COUNT      REG64(0x10000000)
#define REG_TIMER0_MODE       REG64(0x10000010)
#define REG_TIMER0_COMP       REG64(0x10000020)
#define REG_TIMER0_HOLD       REG64(0x10000030)
#define REGS_TIMER0           (*(STimerReg *)&REG_TIMER0_COUNT)
// Timer1
#define REG_TIMER1_COUNT      REG64(0x10000800)
#define REG_TIMER1_MODE       REG64(0x10000810)
#define REG_TIMER1_COMP       REG64(0x10000820)
#define REG_TIMER1_HOLD       REG64(0x10000830)
#define REGS_TIMER1           (*(STimerReg *)&REG_TIMER1_COUNT)
// Timer2
#define REG_TIMER2_COUNT      REG64(0x10001000)
#define REG_TIMER2_MODE       REG64(0x10001010)
#define REG_TIMER2_COMP       REG64(0x10001020)
#define REG_TIMER2_HOLD       REG64(0x10001030)
#define REGS_TIMER2           (*(STimerReg *)&REG_TIMER2_COUNT)
// Timer3
#define REG_TIMER3_COUNT      REG64(0x10001800)
#define REG_TIMER3_MODE       REG64(0x10001810)
#define REG_TIMER3_COMP       REG64(0x10001820)
#define REG_TIMER3_HOLD       REG64(0x10001830)
#define REGS_TIMER3           (*(STimerReg *)&REG_TIMER3_COUNT)

// Graphics InterFace (GIF) registers
#define REG_GIF_CTRL          REG32(0x10003000) // Control Register
#define REG_GIF_MODE          REG32(0x10003010) // Mode Setting Register
#define REG_GIF_STAT          REG32(0x10003020) // Status Register
#define REG_GIF_TAG0          REG32(0x10003040) // GIFtag Save Register
#define REG_GIF_TAG1          REG32(0x10003050) // GIFtag Save Register
#define REG_GIF_TAG2          REG32(0x10003060) // GIFtag Save Register
#define REG_GIF_TAG3          REG32(0x10003070) // GIFtag Save Register
#define REG_GIF_CNT           REG32(0x10003080) // Count Register
#define REG_GIF_P3CNT         REG32(0x10003090) // PATH3 Count Register
#define REG_GIF_P3TAG         REG32(0x100030a0) // PATH3 Tag Register

//
// DMA controller
//
typedef struct
{
  vuint32_t chcr;
  vuint32_t pad0[3];
  vuint32_t madr;
  vuint32_t pad1[3];
  vuint32_t qwc;
  vuint32_t pad2[3];
  vuint32_t tadr;
  vuint32_t pad3[3];
} SDMAChannelReg;
// DMA VIF0 channel registers
#define REG_DMA_VIF0_CHCR     REG32(0x10008000)
#define REG_DMA_VIF0_MADR     REG32(0x10008010)
#define REG_DMA_VIF0_QWC      REG32(0x10008020)
#define REG_DMA_VIF0_TADR     REG32(0x10008030)
#define REGS_DMA_VIF0         (*(SDMAChannelReg *)&REG_DMA_VIF0_CHCR)
// DMA VIF1 channel registers
#define REG_DMA_VIF1_CHCR     REG32(0x10009000)
#define REG_DMA_VIF1_MADR     REG32(0x10009010)
#define REG_DMA_VIF1_QWC      REG32(0x10009020)
#define REG_DMA_VIF1_TADR     REG32(0x10009030)
#define REGS_DMA_VIF1         (*(SDMAChannelReg *)&REG_DMA_VIF1_CHCR)
// DMA GIF channel registers
#define REG_DMA_GIF_CHCR      REG32(0x1000a000)
#define REG_DMA_GIF_MADR      REG32(0x1000a010)
#define REG_DMA_GIF_QWC       REG32(0x1000a020)
#define REG_DMA_GIF_TADR      REG32(0x1000a030)
#define REGS_DMA_GIF          (*(SDMAChannelReg *)&REG_DMA_GIF_CHCR)
// DMA fromIPU channel registers
#define REG_DMA_fromIPU_CHCR  REG32(0x1000b000)
#define REG_DMA_fromIPU_MADR  REG32(0x1000b010)
#define REG_DMA_fromIPU_QWC   REG32(0x1000b020)
#define REG_DMA_fromIPU_TADR  REG32(0x1000b030)
#define REGS_DMA_fromIPU      (*(SDMAChannelReg *)&REG_DMA_fromIPU_CHCR)
// DMA toIPU channel registers
#define REG_DMA_toIPU_CHCR    REG32(0x1000b400)
#define REG_DMA_toIPU_MADR    REG32(0x1000b410)
#define REG_DMA_toIPU_QWC     REG32(0x1000b420)
#define REG_DMA_toIPU_TADR    REG32(0x1000b430)
#define REGS_DMA_toIPU        (*(SDMAChannelReg *)&REG_DMA_toIPU_CHCR)
// DMA SIF0 channel registers
#define REG_DMA_SIF0_CHCR     REG32(0x1000c000)
#define REG_DMA_SIF0_MADR     REG32(0x1000c010)
#define REG_DMA_SIF0_QWC      REG32(0x1000c020)
#define REG_DMA_SIF0_TADR     REG32(0x1000c030)
#define REGS_DMA_SIF0         (*(SDMAChannelReg *)&REG_DMA_SIF0_CHCR)
// DMA SIF1 channel registers
#define REG_DMA_SIF1_CHCR     REG32(0x1000c400)
#define REG_DMA_SIF1_MADR     REG32(0x1000c410)
#define REG_DMA_SIF1_QWC      REG32(0x1000c420)
#define REG_DMA_SIF1_TADR     REG32(0x1000c430)
#define REGS_DMA_SIF1         (*(SDMAChannelReg *)&REG_DMA_SIF1_CHCR)
// DMA SIF2 channel registers
#define REG_DMA_SIF2_CHCR     REG32(0x1000c800)
#define REG_DMA_SIF2_MADR     REG32(0x1000c810)
#define REG_DMA_SIF2_QWC      REG32(0x1000c820)
#define REG_DMA_SIF2_TADR     REG32(0x1000c830)
#define REGS_DMA_SIF2         (*(SDMAChannelReg *)&REG_DMA_SIF2_CHCR)
// DMA fromSPR channel registers
#define REG_DMA_fromSPR_CHCR  REG32(0x1000d000)
#define REG_DMA_fromSPR_MADR  REG32(0x1000d010)
#define REG_DMA_fromSPR_QWC   REG32(0x1000d020)
#define REG_DMA_fromSPR_TADR  REG32(0x1000d030)
#define REGS_DMA_fromSPR      (*(SDMAChannelReg *)&REG_DMA_fromSPR_CHCR)
// DMA toSPR channel registers
#define REG_DMA_toSPR_CHCR    REG32(0x1000d400)
#define REG_DMA_toSPR_MADR    REG32(0x1000d410)
#define REG_DMA_toSPR_QWC     REG32(0x1000d420)
#define REG_DMA_toSPR_TADR    REG32(0x1000d430)
#define REGS_DMA_toSPR        (*(SDMAChannelReg *)&REG_DMA_toSPR_CHCR)
// DMA registers
#define REG_DMA_CTRL          REG32(0x1000e000) // DMA Control Register
#define REG_DMA_STAT          REG32(0x1000e010) // Interrupt Status Register
#define REG_DMA_PCR           REG32(0x1000e020) // Priority Control Register
#define REG_DMA_SQWC          REG32(0x1000e030) // Interleave Size Register
#define REG_DMA_RBSR          REG32(0x1000e040) // Ring Buffer Size Register
#define REG_DMA_RBOR          REG32(0x1000e050) // Ring Buffer Address Register
#define REG_DMA_STADR         REG32(0x1000e060) // Stall Address Register
#define REG_DMA_ENABLER       REG32(0x1000f520) // DMA Hold State Register
#define REG_DMA_ENABLEW       REG32(0x1000f590) // DMA Hold Control Register

// Graphics Synthesizer (GS) privileged registers
#define REG_GS_PMODE          REG64(0x12000000) // Setup CRT Controller
#define REG_GS_SMODE1         REG64(0x12000010) // VHP,VCKSEL,SLCK2,NVCK,CLKSEL,PEVS,PEHS,PVS,PHS,GCONT,SPML,PCK2,XPCK,SINT,PRST,EX,CMOD,SLCK,T1248,LC,RC
#define REG_GS_SMODE2         REG64(0x12000020) // CRTC Video Settings: PAL/NTCS, Interlace, etc.
#define REG_GS_SRFSH          REG64(0x12000030) // DRAM Refresh Settings
#define REG_GS_SYNCH1         REG64(0x12000040) // HS,HSVS,HSEQ,HBP,HFP
#define REG_GS_SYNCH2         REG64(0x12000050) // HB,HF
#define REG_GS_SYNCHV         REG64(0x12000060) // VS,VDP,VBPE,VBP,VFPE,VFP
#define REG_GS_DISPFB1        REG64(0x12000070) // Setup the CRTC's Read Circuit 1 data source settings
#define REG_GS_DISPLAY1       REG64(0x12000080) // RC1 display output settings
#define REG_GS_DISPFB2        REG64(0x12000090) // Setup the CRTC's Read Circuit 2 data source settings
#define REG_GS_DISPLAY2       REG64(0x120000a0) // RC2 display output settings
#define REG_GS_EXTBUF         REG64(0x120000b0)
#define REG_GS_EXTDATA        REG64(0x120000c0)
#define REG_GS_EXTWRITE       REG64(0x120000d0)
#define REG_GS_BGCOLOR        REG64(0x120000e0) // Set CRTC background color
#define REG_GS_CSR            REG64(0x12001000) // System status and reset
#define REG_GS_IMR            REG64(0x12001010) // Interrupt Mask Register
#define REG_GS_BUSDIR         REG64(0x12001040)
#define REG_GS_SIGLBLID       REG64(0x12001080)

//
#define REG_INT_STAT          REG64(0x1000f000) // Interrupt status
#define REG_INT_MASK          REG64(0x1000f010) // Interrupt mask

// SIF (Main == EE, Sub == IOP)
#define REG_SIF_MAINADDR      REG32(0x1000f200) // EE side data addr
#define REG_SIF_SUBADDR       REG32(0x1000f210) // IOP side data addr
#define REG_SIF_MSFLAG        REG32(0x1000f220) // Main->Sub (EE->IOP) flags
#define REG_SIF_SMFLAG        REG32(0x1000f230) // Sub->Main (IOP->EE) flags
#define REG_SIF_SUBCTRL       REG32(0x1000f240)
#define REG_SIF_F260          REG32(0x1000f260)


#endif
