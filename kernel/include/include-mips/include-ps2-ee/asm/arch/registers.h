#ifndef ASM_ARCH_REGISTERS_H
#define ASM_ARCH_REGISTERS_H


#include "inttypes.h"


//---------------------------------------------------------------------------
// DMA Register Constructors
//---------------------------------------------------------------------------
// CHCR Register - Channel Control Register
#define DMA_CHCR(DIR,MOD,ASP,TTE,TIE,STR,TAG) \
  ((uint32_t)(DIR) << 0) | \
  ((uint32_t)(MOD) << 2) | \
  ((uint32_t)(ASP) << 4) | \
  ((uint32_t)(TTE) << 6) | \
  ((uint32_t)(TIE) << 7) | \
  ((uint32_t)(STR) << 8) | \
  ((uint32_t)(TAG) << 16)

// MADR Register - Transfer Address Register
#define DMA_MADR(ADDR,SPR) \
  (((uint32_t)(ADDR) << 0) | \
   ((uint32_t)(SPR)  << 31))

// TADR Register - Tag Address Register
#define DMA_TADR(ADDR,SPR) \
  (((uint32_t)(ADDR) << 0) | \
   ((uint32_t)(SPR)  << 31))

// QWC Register - Transfer Data Size Register
#define DMA_QWC(SIZE) ((uint32_t)(SIZE))

//---------------------------------------------------------------------------
// GS Register Constructors
//---------------------------------------------------------------------------
// CSR Register
#define GS_CSR(SIGNAL,FINISH,HSINT,VSINT,EDWINT,FLUSH,RESET,NFIELD,FIELD,FIFO,REV,ID) \
  ((uint64_t)(SIGNAL) << 0)  | \
  ((uint64_t)(FINISH) << 1)  | \
  ((uint64_t)(HSINT)  << 2)  | \
  ((uint64_t)(VSINT)  << 3)  | \
  ((uint64_t)(EDWINT) << 4)  | \
  ((uint64_t)(FLUSH)  << 8)  | \
  ((uint64_t)(RESET)  << 9)  | \
  ((uint64_t)(NFIELD) << 12) | \
  ((uint64_t)(FIELD)  << 13) | \
  ((uint64_t)(FIFO)   << 14) | \
  ((uint64_t)(REV)    << 16) | \
  ((uint64_t)(ID)     << 24)

#define GS_CSR_RESET() \
  ((uint64_t)(1) << 9)

// PMODE Register
#define GS_PMODE(EN1,EN2,MMOD,AMOD,SLBG,ALP) \
  ((uint64_t)(EN1)  << 0) | \
  ((uint64_t)(EN2)  << 1) | \
  ((uint64_t)(001)  << 2) | \
  ((uint64_t)(MMOD) << 5) | \
  ((uint64_t)(AMOD) << 6) | \
  ((uint64_t)(SLBG) << 7) | \
  ((uint64_t)(ALP)  << 8)

// SMODE2 Register
#define GS_SMODE2(INT,FFMD,DPMS) \
  ((uint64_t)(INT)  << 0) | \
  ((uint64_t)(FFMD) << 1) | \
  ((uint64_t)(DPMS) << 2)

//---------------------------------------------------------------------------
// VCK: 1 VCK unit == 4 pixels
// Raster: ???
//---------------------------------------------------------------------------
// DISPFP Registers
// FBP  - Framebuffer pointer / 2048
// FBW  - Framebuffer width / 64
// PSM  - Pixel Storage Mode
// DBX  - x pos (in pixels)
// DBY  - y pos (in pixels)
#define GS_DISPFB(FBP,FBW,PSM,DBX,DBY) \
  ((uint64_t)(FBP) << 0)  | \
  ((uint64_t)(FBW) << 9)  | \
  ((uint64_t)(PSM) << 15) | \
  ((uint64_t)(DBX) << 32) | \
  ((uint64_t)(DBY) << 43)
// DISPLAY Registers
// DX   - x pos in display area (in VCK units)
// DY   - y pos in display area (in raster units)
// MAGH - magnification in h direction (1..16) - 1
// MAGV - magnification in v direction (1...4) - 1
// DW   - display area width  (in VCK units)   - 1
// DH   - display area height (in pixels)      - 1
#define GS_DISPLAY(DX,DY,MAGH,MAGV,DW,DH) \
  ((uint64_t)(DX)   <<  0) | \
  ((uint64_t)(DY)   << 12) | \
  ((uint64_t)(MAGH) << 23) | \
  ((uint64_t)(MAGV) << 27) | \
  ((uint64_t)(DW)   << 32) | \
  ((uint64_t)(DH)   << 44)
#define GS_DISPLAY_CREATE(VCK,DX,DY,MAGH,MAGV,DW,DH) \
  GS_DISPLAY((DX)*(VCK), DY, (MAGH)-1, (MAGV)-1, (DW)*(MAGH)-1, (DH)*(MAGV)-1)

// BGCOLOR Register
#define GS_BGCOLOR(R,G,B) \
  ((uint64_t)(R) <<  0) | \
  ((uint64_t)(G) <<  8) | \
  ((uint64_t)(B) << 16)

// ALPHA_x Registers - Setup Alpha Blending Parameters
//   Alpha Formula is: Cv = (A-B)*C>>7 + D
//   For A,B,D - (0=texture, 1=frame buffer, 2=0)
//   For C - (0=texture, 1=frame buffer, 2=use FIX field for Alpha)
#define GS_ALPHA(A,B,C,D,FIX) \
  (((uint64_t)(A)   <<  0) | \
   ((uint64_t)(B)   <<  2) | \
   ((uint64_t)(C)   <<  4) | \
   ((uint64_t)(D)   <<  6) | \
   ((uint64_t)(FIX) << 32))

// BITBLTBUF Register - Setup Image Transfer Between EE and GS
//   SBP  - Source buffer address (Address/256)
//   SBW  - Source buffer width (Pixels/64)
//   SPSM - Source pixel format (0 = 32bit RGBA)
//   DBP  - Destination buffer address (Address/256)
//   DBW  - Destination buffer width (Pixels/64)
//   DPSM - Destination pixel format (0 = 32bit RGBA)
//
// - When transferring from EE to GS, only the Detination fields
//   need to be set. (Only Source fields for GS->EE, and all for GS->GS).
#define GS_BITBLTBUF(SBP,SBW,SPSM,DBP,DBW,DPSM) \
  (((uint64_t)(SBP)  <<  0) | \
   ((uint64_t)(SBW)  << 16) | \
   ((uint64_t)(SPSM) << 24) | \
   ((uint64_t)(DBP)  << 32) | \
   ((uint64_t)(DBW)  << 48) | \
   ((uint64_t)(DPSM) << 56))

// FRAME_x Register
#define GS_FRAME(FBP,FBW,PSM,FBMSK) \
  (((uint64_t)(FBP)   <<  0) | \
   ((uint64_t)(FBW)   << 16) | \
   ((uint64_t)(PSM)   << 24) | \
   ((uint64_t)(FBMSK) << 32))

// PRIM Register - Setup Drawing Primitive
//   PRI  - Primitive type
//   IIP  - Shading method (0=flat, 1=gouraud)
//   TME  - Texture mapping (0=off, 1=on)
//   FGE  - Fog (0=off, 1=on)
//   ABE  - Alpha Blending (0=off, 1=on)
//   AA1  - Antialiasing (0=off,1=on)
//   FST  - Texture coordinate specification (0=use ST/RGBAQ register, 1=use UV register)
//                              (UV means no perspective correction, good for 2D)
//   CTXT - Drawing context (0=1, 1=2)
//   FIX  - ?? Fragment value control (use 0)
#define PRIM_POINT              0
#define PRIM_LINE               1
#define PRIM_LINE_STRIP         2
#define PRIM_TRI                3
#define PRIM_TRI_STRIP          4
#define PRIM_TRI_FAN            5
#define PRIM_SPRITE             6

#define SHADE_FLAT		        0
#define SHADE_GOURAUD	        1

#define TEXTURES_OFF            0
#define TEXTURES_ON             1

#define FOG_OFF                 0
#define FOG_ON                  1

#define ALPHABLEND_OFF          0
#define ALPHABLEND_ON           1

#define ALIASING_OFF            0
#define ALIASING_ON             1

#define TEXTURES_ST             0
#define TEXTURES_UV             1

#define GS_PRIM(PRI,IIP,TME,FGE,ABE,AA1,FST,CTXT,FIX) \
  (((uint64_t)(PRI)  <<  0) | \
   ((uint64_t)(IIP)  <<  3) | \
   ((uint64_t)(TME)  <<  4) | \
   ((uint64_t)(FGE)  <<  5) | \
   ((uint64_t)(ABE)  <<  6) | \
   ((uint64_t)(AA1)  <<  7) | \
   ((uint64_t)(FST)  <<  8) | \
   ((uint64_t)(CTXT) <<  9) | \
   ((uint64_t)(FIX)  << 10))

// RGBAQ Register
#define GS_RGBAQ(R,G,B,A,Q) \
  (((uint64_t)(R) <<  0) | \
   ((uint64_t)(G) <<  8) | \
   ((uint64_t)(B) << 16) | \
   ((uint64_t)(A) << 24) | \
   ((uint64_t)(Q) << 32))

// SCISSOR_x Register
#define GS_SCISSOR(X0,X1,Y0,Y1) \
  (((uint64_t)(X0) <<  0) | \
   ((uint64_t)(X1) << 16) | \
   ((uint64_t)(Y0) << 32) | \
   ((uint64_t)(Y1) << 48))

// TEST_x Register - Pixel Test Settings
//   ATE   - Alpha Test (0=off, 1=on)
//   ATST  - Alpha Test Method
//             0=NEVER:  All pixels fail.
//             1=ALWAYS: All pixels pass.
//             2=LESS:   Pixels with A less than AREF pass.
//             3=LEQUAL, 4=EQUAL, 5=GEQUAL, 6=GREATER, 7=NOTEQUAL
//   AREF  - Alpha value compared to.
//   AFAIL - What to do when a pixel fails a test.
//             0=KEEP:    Don't update anything.
//             1=FBONLY:  Update frame buffer only.
//             2=ZBONLY:  Update z-buffer only.
//             3=RGBONLY: Update only the frame buffer RGB.
//   DATE  - Destination Alpha Test (0=off, 1=on)
//   DATM  - DAT Mode (0=pass pixels whose destination alpha is 0)
//   ZTE   - Depth Test (0=off, 1=on)
//   ZTST  - Depth Test Method.
//             0=NEVER, 1=ALWAYS, 2=GEQUAL, 3=GREATER
#define ATST_NEVER              0
#define ATST_ALWAYS             1
#define ATST_LESS               2
#define ATST_LEQUAL             3
#define ATST_EQUAL              4
#define ATST_GEQUAL             5
#define ATST_GREATER            6
#define ATST_NOTEQUAL           7

#define AFAIL_KEEP              0
#define AFAIL_FBONLY            1
#define AFAIL_ZBONLY            2
#define AFAIL_RGBONLY           3

#define ZTST_NEVER              0
#define ZTST_ALWAYS             1
#define ZTST_GEQUAL             2
#define ZTST_GREATER            3

#define GS_TEST(ATE,ATST,AREF,AFAIL,DATE,DATM,ZTE,ZTST) \
  (((uint64_t)(ATE)   <<  0)| \
   ((uint64_t)(ATST)  <<  1)| \
   ((uint64_t)(AREF)  <<  4)| \
   ((uint64_t)(AFAIL) << 12)| \
   ((uint64_t)(DATE)  << 14)| \
   ((uint64_t)(DATM)  << 15)| \
   ((uint64_t)(ZTE)   << 16)| \
   ((uint64_t)(ZTST)  << 17))

// TEX0_x Register - Set Texture Buffer Information
//   TBP0 - Texture Buffer Base Pointer (Address/256)
//   TBW  - Texture Buffer Width (Texels/64)
//   PSM  - Pixel Storage Format (0 = 32bit RGBA)
//   TW   - Texture Width (Width = 2^TW)
//   TH   - Texture Height (Height = 2^TH)
//   TCC  - Tecture Color Component
//                        0=RGB,
//                        1=RGBA, use Alpha from TEXA reg when not in PSM
//   TFX  - Texture Function (0=modulate, 1=decal, 2=hilight, 3=hilight2)
#define TEX_MODULATE            0
#define TEX_DECAL               1
#define TEX_HILIGHT             2
#define TEX_HILIGHT2            3

#define GS_TEX0(TBP0,TBW,PSM,TW,TH,TCC,TFX,CBP,CPSM,CSM,CSA,CLD) \
  (((uint64_t)(TBP0) <<  0) | \
   ((uint64_t)(TBW)  << 14) | \
   ((uint64_t)(PSM)  << 20) | \
   ((uint64_t)(TW)   << 26) | \
   ((uint64_t)(TH)   << 30) | \
   ((uint64_t)(TCC)  << 34) | \
   ((uint64_t)(TFX)  << 35) | \
   ((uint64_t)(CBP)  << 37) | \
   ((uint64_t)(CPSM) << 51) | \
   ((uint64_t)(CSM)  << 55) | \
   ((uint64_t)(CSA)  << 56) | \
   ((uint64_t)(CLD)  << 61))

// TEX1_x Register - Set Texture Information
//   LCM   - LOD calculation method
//   MXL   - Maximum MIP level (0-6)
//   MMAG  - Filter when expanding (0=NEAREST, 1=LINEAR)
//   MMIN  - Filter when reducing (0=NEAREST, 1=LINEAR)
//   MTBA  - MIP Base specified by (0=MIPTBP1&2, 1=Automatic)
//   L     - LOD parameter L
//   K     - LOD parameter K
#define FILTER_NEAREST          0
#define FILTER_LINEAR           1

#define GS_TEX1(LCM,MXL,MMAG,MMIN,MTBA,L,K) \
  (((uint64_t)(LCM)  <<  0) | \
   ((uint64_t)(MXL)  <<  2) | \
   ((uint64_t)(MMAG) <<  5) | \
   ((uint64_t)(MMIN) <<  6) | \
   ((uint64_t)(MTBA) <<  9) | \
   ((uint64_t)(L)    << 19) | \
   ((uint64_t)(K)    << 32))

// TRXDIR Register - Set Image Transfer Directon, and Start Transfer
//   XDIR - (0=EE->GS, 1=GS->EE, 2=GS->GS, 3=Transmission is deactivated)
#define XDIR_EE_GS              0
#define XDIR_GS_EE              1
#define XDIR_GS_GS              2
#define XDIR_DEACTIVATE         3

#define GS_TRXDIR(XDIR) \
  ((uint64_t)(XDIR))

// TRXPOS Register - Setup Image Transfer Coordinates
//   SSAX - Source Upper Left X
//   SSAY - Source Upper Left Y
//   DSAX - Destionation Upper Left X
//   DSAY - Destionation Upper Left Y
//   DIR  - Pixel Transmission Order (00 = top left -> bottom right)
//
// - When transferring from EE to GS, only the Detination fields
//   need to be set. (Only Source fields for GS->EE, and all for GS->GS).
#define GS_TRXPOS(SSAX,SSAY,DSAX,DSAY,DIR) \
  (((uint64_t)(SSAX) <<  0) | \
   ((uint64_t)(SSAY) << 16) | \
   ((uint64_t)(DSAX) << 32) | \
   ((uint64_t)(DSAY) << 48) | \
   ((uint64_t)(DIR)  << 59))

// TRXREG Register - Setup Image Transfer Size
//   RRW - Image Width
//   RRH - Image Height
#define GS_TRXREG(RRW,RRH) \
  (((uint64_t)(RRW) <<  0) | \
   ((uint64_t)(RRH) << 32))

// ST Register - Set Texture Coordinates
#define GS_ST(S,T) \
  (((uint64_t)(S) <<  0) | \
   ((uint64_t)(T) << 32))

// UV Register - Specify Texture Coordinates
#define GS_UV(U,V) \
  (((uint64_t)(U) <<  0) | \
   ((uint64_t)(V) << 16))

// XYZ2 Register
#define GS_XYZ2(X,Y,Z) \
  (((uint64_t)(X) <<  0) | \
   ((uint64_t)(Y) << 16) | \
   ((uint64_t)(Z) << 32))

// XYOFFSET_x Register
#define GS_XYOFFSET(OFX,OFY) \
  (((uint64_t)(OFX) <<  0) | \
   ((uint64_t)(OFY) << 32))

// ZBUF_x Register
#define ZMSK_ENABLE		0
#define ZMSK_DISABLE	1

#define GS_ZBUF(ZBP,PSM,ZMSK) \
  (((uint64_t)(ZBP)  <<  0) | \
   ((uint64_t)(PSM)  << 24) | \
   ((uint64_t)(ZMSK) << 32))

//---------------------------------------------------------------------------
// Registers
//---------------------------------------------------------------------------
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

// DMA registers: Graphics InterFace (GIF) channel
#define REG_DMA_GIF_CHCR      (*(vuint32_t *)(0x1000a000)) // GIF Channel Control Register
#define REG_DMA_GIF_MADR      (*(vuint32_t *)(0x1000a010)) // Transfer Address Register
#define REG_DMA_GIF_QWC       (*(vuint32_t *)(0x1000a020)) // Transfer Size Register (in qwords)
#define REG_DMA_GIF_TADR      (*(vuint32_t *)(0x1000a030))
// DMA registers: Serial InterFace (SIF) channel
#define REG_DMA_SIF0_CHCR     (*(vuint32_t *)(0x1000c000))
#define REG_DMA_SIF0_MADR     (*(vuint32_t *)(0x1000c010))
#define REG_DMA_SIF0_QWC      (*(vuint32_t *)(0x1000c020))
#define REG_DMA_SIF0_TADR     (*(vuint32_t *)(0x1000c030))
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
