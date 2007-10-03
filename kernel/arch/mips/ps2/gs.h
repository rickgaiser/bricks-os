//---------------------------------------------------------------------------
// File:    gs.h
// Author:  Tony Saveski, t_saveski@yahoo.com
// Notes:   Playstation2 GS Convenience Routines
//---------------------------------------------------------------------------
#ifndef GS_H
#define GS_H


#include "inttypes.h"
#include "asm/arch/registers.h"


//---------------------------------------------------------------------------
// Privileged Register Macros
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// CSR Register
//---------------------------------------------------------------------------
#define GS_SET_CSR(SIGNAL,FINISH,HSINT,VSINT,EDWINT,FLUSH,RESET,NFIELD,FIELD,FIFO,REV,ID) \
  REG_GS_CSR = \
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

#define GS_RESET() \
  REG_GS_CSR = ((uint64_t)(1) << 9)

//---------------------------------------------------------------------------
// PMODE Register
//---------------------------------------------------------------------------
#define GS_SET_PMODE(EN1,EN2,MMOD,AMOD,SLBG,ALP) \
  REG_GS_PMODE = \
  ((uint64_t)(EN1)  << 0) | \
  ((uint64_t)(EN2)  << 1) | \
  ((uint64_t)(001)  << 2) | \
  ((uint64_t)(MMOD) << 5) | \
  ((uint64_t)(AMOD) << 6) | \
  ((uint64_t)(SLBG) << 7) | \
  ((uint64_t)(ALP)  << 8)

//---------------------------------------------------------------------------
// SMODE2 Register
//---------------------------------------------------------------------------
#define GS_SET_SMODE2(INT,FFMD,DPMS) \
  REG_GS_SMODE2 = \
  ((uint64_t)(INT)  << 0) | \
  ((uint64_t)(FFMD) << 1) | \
  ((uint64_t)(DPMS) << 2)

//---------------------------------------------------------------------------
// DISPFP1 Register
//---------------------------------------------------------------------------
#define GS_SET_DISPFB1(FBP,FBW,PSM,DBX,DBY) \
  REG_GS_DISPFB1 = \
  ((uint64_t)(FBP) << 0)  | \
  ((uint64_t)(FBW) << 9)  | \
  ((uint64_t)(PSM) << 15) | \
  ((uint64_t)(DBX) << 32) | \
  ((uint64_t)(DBY) << 43)

//---------------------------------------------------------------------------
// DISPLAY1 Register
//---------------------------------------------------------------------------
#define GS_SET_DISPLAY1(DX,DY,MAGH,MAGV,DW,DH) \
  REG_GS_DISPLAY1 = \
  ((uint64_t)(DX)   <<  0) | \
  ((uint64_t)(DY)   << 12) | \
  ((uint64_t)(MAGH) << 23) | \
  ((uint64_t)(MAGV) << 27) | \
  ((uint64_t)(DW)   << 32) | \
  ((uint64_t)(DH)   << 44)

//---------------------------------------------------------------------------
// DISPFP2 Register
//---------------------------------------------------------------------------
#define GS_SET_DISPFB2(FBP,FBW,PSM,DBX,DBY) \
  REG_GS_DISPFB2 = \
  ((uint64_t)(FBP) <<  0) | \
  ((uint64_t)(FBW) <<  9) | \
  ((uint64_t)(PSM) << 15) | \
  ((uint64_t)(DBX) << 32) | \
  ((uint64_t)(DBY) << 43)

//---------------------------------------------------------------------------
// DISPLAY2 Register
//---------------------------------------------------------------------------
#define GS_SET_DISPLAY2(DX,DY,MAGH,MAGV,DW,DH) \
  REG_GS_DISPLAY2 = \
  ((uint64_t)(DX)   <<  0) | \
  ((uint64_t)(DY)   << 12) | \
  ((uint64_t)(MAGH) << 23) | \
  ((uint64_t)(MAGV) << 27) | \
  ((uint64_t)(DW)   << 32) | \
  ((uint64_t)(DH)   << 44)

//---------------------------------------------------------------------------
// BGCOLOR Register
//---------------------------------------------------------------------------
#define GS_SET_BGCOLOR(R,G,B) \
  REG_GS_BGCOLOR = \
  ((uint64_t)(R) <<  0) | \
  ((uint64_t)(G) <<  8) | \
  ((uint64_t)(B) << 16)

//---------------------------------------------------------------------------
// General Purpose Register Macros
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// ALPHA_x Registers - Setup Alpha Blending Parameters
//   Alpha Formula is: Cv = (A-B)*C>>7 + D
//   For A,B,D - (0=texture, 1=frame buffer, 2=0)
//   For C - (0=texture, 1=frame buffer, 2=use FIX field for Alpha)
//---------------------------------------------------------------------------
#define GS_ALPHA(A,B,C,D,FIX) \
  (((uint64_t)(A)   <<  0) | \
   ((uint64_t)(B)   <<  2) | \
   ((uint64_t)(C)   <<  4) | \
   ((uint64_t)(D)   <<  6) | \
   ((uint64_t)(FIX) << 32))

//---------------------------------------------------------------------------
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
//---------------------------------------------------------------------------
#define GS_BITBLTBUF(SBP,SBW,SPSM,DBP,DBW,DPSM) \
  (((uint64_t)(SBP)  <<  0) | \
   ((uint64_t)(SBW)  << 16) | \
   ((uint64_t)(SPSM) << 24) | \
   ((uint64_t)(DBP)  << 32) | \
   ((uint64_t)(DBW)  << 48) | \
   ((uint64_t)(DPSM) << 56))

//---------------------------------------------------------------------------
// FRAME_x Register
//---------------------------------------------------------------------------
#define GS_FRAME(FBP,FBW,PSM,FBMSK) \
  (((uint64_t)(FBP)   <<  0) | \
   ((uint64_t)(FBW)   << 16) | \
   ((uint64_t)(PSM)   << 24) | \
   ((uint64_t)(FBMSK) << 32))

//---------------------------------------------------------------------------
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
//---------------------------------------------------------------------------
#define PRIM_POINT              0
#define PRIM_LINE               1
#define PRIM_LINE_STRIP         2
#define PRIM_TRI                3
#define PRIM_TRI_STRIP          4
#define PRIM_TRI_FAN            5
#define PRIM_SPRITE             6

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

//---------------------------------------------------------------------------
// RGBAQ Register
//---------------------------------------------------------------------------
#define GS_RGBAQ(R,G,B,A,Q) \
  (((uint64_t)(R) <<  0) | \
   ((uint64_t)(G) <<  8) | \
   ((uint64_t)(B) << 16) | \
   ((uint64_t)(A) << 24) | \
   ((uint64_t)(Q) << 32))

//---------------------------------------------------------------------------
// SCISSOR_x Register
//---------------------------------------------------------------------------
#define GS_SCISSOR(X0,X1,Y0,Y1) \
  (((uint64_t)(X0) <<  0) | \
   ((uint64_t)(X1) << 16) | \
   ((uint64_t)(Y0) << 32) | \
   ((uint64_t)(Y1) << 48))

//---------------------------------------------------------------------------
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
//---------------------------------------------------------------------------
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

//---------------------------------------------------------------------------
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
//---------------------------------------------------------------------------
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

//---------------------------------------------------------------------------
// TEX1_x Register - Set Texture Information
//   LCM   - LOD calculation method
//   MXL   - Maximum MIP level (0-6)
//   MMAG  - Filter when expanding (0=NEAREST, 1=LINEAR)
//   MMIN  - Filter when reducing (0=NEAREST, 1=LINEAR)
//   MTBA  - MIP Base specified by (0=MIPTBP1&2, 1=Automatic)
//   L     - LOD parameter L
//   K     - LOD parameter K
//---------------------------------------------------------------------------
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

//---------------------------------------------------------------------------
// TRXDIR Register - Set Image Transfer Directon, and Start Transfer
//   XDIR - (0=EE->GS, 1=GS->EE, 2=GS->GS, 3=Transmission is deactivated)
//---------------------------------------------------------------------------
#define XDIR_EE_GS              0
#define XDIR_GS_EE              1
#define XDIR_GS_GS              2
#define XDIR_DEACTIVATE         3

#define GS_TRXDIR(XDIR) \
  ((uint64_t)(XDIR))

//---------------------------------------------------------------------------
// TRXPOS Register - Setup Image Transfer Coordinates
//   SSAX - Source Upper Left X
//   SSAY - Source Upper Left Y
//   DSAX - Destionation Upper Left X
//   DSAY - Destionation Upper Left Y
//   DIR  - Pixel Transmission Order (00 = top left -> bottom right)
//
// - When transferring from EE to GS, only the Detination fields
//   need to be set. (Only Source fields for GS->EE, and all for GS->GS).
//---------------------------------------------------------------------------
#define GS_TRXPOS(SSAX,SSAY,DSAX,DSAY,DIR) \
  (((uint64_t)(SSAX) <<  0) | \
   ((uint64_t)(SSAY) << 16) | \
   ((uint64_t)(DSAX) << 32) | \
   ((uint64_t)(DSAY) << 48) | \
   ((uint64_t)(DIR)  << 59))

//---------------------------------------------------------------------------
// TRXREG Register - Setup Image Transfer Size
//   RRW - Image Width
//   RRH - Image Height
//---------------------------------------------------------------------------
#define GS_TRXREG(RRW,RRH) \
  (((uint64_t)(RRW) <<  0) | \
   ((uint64_t)(RRH) << 32))

//---------------------------------------------------------------------------
// UV Register - Specify Texture Coordinates
//---------------------------------------------------------------------------
#define GS_UV(U,V) \
  (((uint64_t)(U) << 0) | \
   ((uint64_t)(V) << 16))

//---------------------------------------------------------------------------
// XYZ2 Register
//---------------------------------------------------------------------------
#define GS_XYZ2(X,Y,Z) \
  (((uint64_t)(X) <<  0) | \
   ((uint64_t)(Y) << 16) | \
   ((uint64_t)(Z) << 32))

//---------------------------------------------------------------------------
// XYOFFSET_x Register
//---------------------------------------------------------------------------
#define GS_XYOFFSET(OFX,OFY) \
  (((uint64_t)(OFX) <<  0) | \
   ((uint64_t)(OFY) << 32))

//---------------------------------------------------------------------------
// ZBUF_x Register
//---------------------------------------------------------------------------
#define GS_ZBUF(ZBP,PSM,ZMSK) \
  (((uint64_t)(ZBP)  <<  0) | \
   ((uint64_t)(PSM)  << 24) | \
   ((uint64_t)(ZMSK) << 32))


#ifdef __cplusplus
extern "C" {
#endif

extern void gs_load_texture(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t data_adr, uint32_t dest_adr, uint16_t dest_w);
extern uint16_t gs_texture_wh(uint16_t n);
extern uint8_t gs_is_ntsc(void);
extern uint8_t gs_is_pal(void);

#ifdef __cplusplus
}
#endif


#endif // GS_H
