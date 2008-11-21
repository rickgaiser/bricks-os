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


#ifndef PS2_GIF_H
#define PS2_GIF_H


#include "inttypes.h"
#include "dma.h"
#include "registers.h"


#define FLOAT_TO_INT(F) (*((uint32_t *)&(F)))


// For SetGsCrt function
// Interlacing
#define GS_NON_INTERLACED          0
#define GS_INTERLACED              1
// Field mode (only if INTERLACED)
#define GS_FIELD                   0       // Scan every other line
#define GS_FRAME                   1       // Scan every line

// Pixel Storage Modes
//   NOTE, how to fully fill the video memory:
//     GS_PSM_24 + GS_PSM_8H = 32bit
//     GS_PSM_24 + GS_PSM_4HH + GS_PSM_4HL = 32bit
//   NOTE2, alignment:
//     framebuffer:  8KiB
//     z-buffer:     8KiB
//     textures:   256Bytes
//     clut:       256Bytes
//   NOTE3, 16bit frame- and z-buffers
//     Always use GS_PSM_16S when only one is 16bit
//     When both frame- and z-buffer are 16bit, use GS_PSM_16
// Frame-, Z- and Textures
#define GS_PSM_32               0x00    // 32 bits per pixel.
#define GS_PSM_24               0x01    // 24 bits per pixel. Stored in bits 0..23 of a 32 bit value.
#define GS_PSM_16               0x02    // 16 bits per pixel.
#define GS_PSM_16S              0x0A    // 16 bits per pixel.
// Textures only
#define GS_PSM_8                0x13    // 8 bits per pixel, palettized.
#define GS_PSM_4                0x14    // 4 bits per pixel, palettized.
#define GS_PSM_8H               0x1B    // 8 bits per pixel, palettized. Stored int bits 24..31 of a 32 bit value
#define GS_PSM_4HH              0x24    // 4 bits per pixel, palettized. Stored int bits 28..31 of a 32 bit value
#define GS_PSM_4HL              0x2C    // 4 bits per pixel, palettized. Stored int bits 24..27 of a 32 bit value

#define GS_X_BASE               1024
#define GS_Y_BASE               1024
#define GS_MEMORY_SIZE          (4*1024*1024)

#define IS_PAL()                (*((char *)0x1fc80000 - 0xae) == 'E')
#define IS_NTSC()               (!IS_PAL())

//---------------------------------------------------------------------------
// GS Register Constructors
//---------------------------------------------------------------------------
#define GS_CSR_ODD   (1<<13)
#define GS_CSR_VSYNC (1<< 3)
// REG_GS_CSR constructor
#define GS_CSR(SIGNAL,FINISH,HSINT,VSINT,EDWINT,FLUSH,RESET,NFIELD,FIELD,FIFO,REV,ID) \
  ((uint64_t)(SIGNAL) <<  0) | \
  ((uint64_t)(FINISH) <<  1) | \
  ((uint64_t)(HSINT)  <<  2) | \
  ((uint64_t)(VSINT)  <<  3) | \
  ((uint64_t)(EDWINT) <<  4) | \
  ((uint64_t)(FLUSH)  <<  8) | \
  ((uint64_t)(RESET)  <<  9) | \
  ((uint64_t)(NFIELD) << 12) | \
  ((uint64_t)(FIELD)  << 13) | \
  ((uint64_t)(FIFO)   << 14) | \
  ((uint64_t)(REV)    << 16) | \
  ((uint64_t)(ID)     << 24)
#define GS_CSR_RESET() \
  ((uint64_t)(1)      <<  9)

// REG_GS_PMODE constructor
#define GS_PMODE(EN1,EN2,MMOD,AMOD,SLBG,ALP) \
  ((uint64_t)(EN1)  << 0) | \
  ((uint64_t)(EN2)  << 1) | \
  ((uint64_t)(001)  << 2) | \
  ((uint64_t)(MMOD) << 5) | \
  ((uint64_t)(AMOD) << 6) | \
  ((uint64_t)(SLBG) << 7) | \
  ((uint64_t)(ALP)  << 8)

// REG_GS_SMODE2 constructor
#define GS_SMODE2(INT,FFMD,DPMS) \
  ((uint64_t)(INT)  << 0) | \
  ((uint64_t)(FFMD) << 1) | \
  ((uint64_t)(DPMS) << 2)

// REG_GS_DISPFB constructor
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

// REG_GS_DISPLAY constructor
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

// REG_GS_BGCOLOR constructor
#define GS_BGCOLOR(R,G,B) \
  ((uint64_t)(R) <<  0) | \
  ((uint64_t)(G) <<  8) | \
  ((uint64_t)(B) << 16)

//---------------------------------------------------------------------------
// GS Register Constructors
//---------------------------------------------------------------------------
// GS PRIM Register - Setup Drawing Primitive
// PRI: Primitive type
#define GS_PRIM_POINT                   0
#define GS_PRIM_LINE                    1
#define GS_PRIM_LINE_STRIP              2
#define GS_PRIM_TRI                     3
#define GS_PRIM_TRI_STRIP               4
#define GS_PRIM_TRI_FAN                 5
#define GS_PRIM_SPRITE                  6
// IIP: Shading method (0=flat, 1=gouraud)
#define GS_PRIM_SHADE_FLAT              0
#define GS_PRIM_SHADE_GOURAUD           1
// TME: Texture mapping (0=off, 1=on)
#define GS_PRIM_TEXTURES_OFF            0
#define GS_PRIM_TEXTURES_ON             1
// FGE: Fog (0=off, 1=on)
#define GS_PRIM_FOG_OFF                 0
#define GS_PRIM_FOG_ON                  1
// ABE: Alpha Blending (0=off, 1=on)
#define GS_PRIM_ALPHABLEND_OFF          0
#define GS_PRIM_ALPHABLEND_ON           1
// AA1: Antialiasing (0=off,1=on)
#define GS_PRIM_ALIASING_OFF            0
#define GS_PRIM_ALIASING_ON             1
// FST: Texture coordinate specification (0=use ST/RGBAQ register, 1=use UV register)
#define GS_PRIM_TEXTURES_ST             0 // For 3D
#define GS_PRIM_TEXTURES_UV             1 // no perspective correction, good for 2D
// CTXT: Drawing context (0=1, 1=2)
#define GS_PRIM_CONTEXT0                0
#define GS_PRIM_CONTEXT1                1
// FIX: ?? Fragment value control (use 0)

#define GS_ATST_NEVER                   0
#define GS_ATST_ALWAYS                  1
#define GS_ATST_LESS                    2
#define GS_ATST_LEQUAL                  3
#define GS_ATST_EQUAL                   4
#define GS_ATST_GEQUAL                  5
#define GS_ATST_GREATER                 6
#define GS_ATST_NOTEQUAL                7

#define GS_AFAIL_KEEP                   0
#define GS_AFAIL_FBONLY                 1
#define GS_AFAIL_ZBONLY                 2
#define GS_AFAIL_RGBONLY                3

#define GS_ZTST_NEVER                   0
#define GS_ZTST_ALWAYS                  1
#define GS_ZTST_GEQUAL                  2
#define GS_ZTST_GREATER                 3

#define GS_TEX0_MODULATE                0
#define GS_TEX0_DECAL                   1
#define GS_TEX0_HILIGHT                 2 // Not supported in OpenGL???
#define GS_TEX0_HILIGHT2                3 // Not supported in OpenGL???

#define GS_TEX1_NEAREST                 0
#define GS_TEX1_LINEAR                  1
#define GS_TEX1_NEAREST_MIPMAP_NEAREST  2
#define GS_TEX1_NEAREST_MIPMAP_LINEAR   3
#define GS_TEX1_LINEAR_MIPMAP_NEAREST   4
#define GS_TEX1_LINEAR_MIPMAP_LINEAR    5

#define GS_TRXDIR_EE_TO_GS              0
#define GS_TRXDIR_GS_TO_EE              1
#define GS_TRXDIR_GS_TO_GS              2
#define GS_TRXDIR_DEACTIVATE            3

#define GS_ZBUF_ENABLE                  0
#define GS_ZBUF_DISABLE                 1


//-------------------------------------------------------------------------
namespace GIF
{
  namespace DataFormat
  {
    enum EGIFDataFormat {packed, reglist, image, disable};
  };

  namespace REG
  {
    enum EGIFREG  {prim, rgbaq, st, uv, xyzf2, xyz2, tex0_1, tex0_2, clamp_1, clamp_2, fog, reserved, xyzf3, xyz3, ad, nop};
    enum EGIFREG2 {
      tex1_1     = 0x14,
      tex1_2     = 0x15,
      tex2_1     = 0x16,
      tex2_2     = 0x17,
      xyoffset_1 = 0x18, // Mapping from Primitive to Window coordinate system (Context 1)
      xyoffset_2 = 0x19, // Mapping from Primitive to Window coordinate system (Context 2)
      prmodecont = 0x1a,
      prmode     = 0x1b,
      texclut    = 0x1c,
      scanmsk    = 0x22,
      miptbp1_1  = 0x34,
      miptbp1_2  = 0x35,
      miptbp2_1  = 0x36,
      miptbp2_2  = 0x37,
      texa       = 0x3b,
      fogcol     = 0x3d,
      texflush   = 0x3f, // Write to this register before using newly loaded texture
      scissor_1  = 0x40, // Setup clipping rectangle (Context 1)
      scissor_2  = 0x41, // Setup clipping rectangle (Context 2)
      alpha_1    = 0x42, // Setup Alpha Blending Parameters (Context 1)
      alpha_2    = 0x43, // Setup Alpha Blending Parameters (Context 2)
      dimx       = 0x44,
      dthe       = 0x45,
      colclamp   = 0x46,
      test_1     = 0x47,
      test_2     = 0x48,
      pabe       = 0x49,
      fba_1      = 0x4a,
      fba_2      = 0x4b,
      frame_1    = 0x4c, // Frame buffer settings (Context 1)
      frame_2    = 0x4d, // Frame buffer settings (Context 2)
      zbuf_1     = 0x4e,
      zbuf_2     = 0x4f,
      bitbltbuf  = 0x50, // Setup Image Transfer Between EE and GS
      trxpos     = 0x51, // Setup Image Transfer Coordinates
      trxreg     = 0x52, // Setup Image Transfer Size
      trxdir     = 0x53, // Set Image Transfer Directon + Start Transfer
      hwreg      = 0x54,
      signal     = 0x60,
      finish     = 0x61,
      label      = 0x62
    };

    inline uint64_t PRIM(uint8_t prim, bool IIP = false, bool TME = false, bool FGE = false, bool ABE = false, bool AA1 = false, bool FST = false, bool CTXT = false, bool FIX = false)
    {
      return
        (((uint64_t)(prim) <<  0) |
         ((uint64_t)(IIP)  <<  3) |
         ((uint64_t)(TME)  <<  4) |
         ((uint64_t)(FGE)  <<  5) |
         ((uint64_t)(ABE)  <<  6) |
         ((uint64_t)(AA1)  <<  7) |
         ((uint64_t)(FST)  <<  8) |
         ((uint64_t)(CTXT) <<  9) |
         ((uint64_t)(FIX)  << 10));
    }
    inline uint64_t RGBAQ(uint8_t R, uint8_t G, uint8_t B,  uint8_t A, float Q)
    {
      return
        (((uint64_t)(R) <<  0) |
         ((uint64_t)(G) <<  8) |
         ((uint64_t)(B) << 16) |
         ((uint64_t)(A) << 24) |
         ((uint64_t)FLOAT_TO_INT(Q) << 32));
    }
    inline uint64_t ST(float S, float T)
    {
      return
        (((uint64_t)FLOAT_TO_INT(S) <<  0) |
         ((uint64_t)FLOAT_TO_INT(T) << 32));
    }
    inline uint64_t UV(uint16_t U, uint16_t V)
    {
      return
        (((uint64_t)(U) <<  0) |
         ((uint64_t)(V) << 16));
    }
    inline uint64_t XYZF2(uint16_t X, uint16_t Y, uint32_t Z, uint8_t F)
    {
      // 24bit Z (depth)
      //  8bit F (fog)
      return
        (((uint64_t)(X) <<  0) |
         ((uint64_t)(Y) << 16) |
         ((uint64_t)(Z) << 32) |
         ((uint64_t)(F) << 56));
    }
    inline uint64_t XYZ2(uint16_t X, uint16_t Y, uint32_t Z)
    {
      // 32bit Z (depth)
      return
        (((uint64_t)(X) <<  0) |
         ((uint64_t)(Y) << 16) |
         ((uint64_t)(Z) << 32));
    }
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
    inline uint64_t TEX0(uint16_t TBP0, uint8_t TBW, uint8_t PSM, uint8_t TW, uint8_t TH, uint8_t TCC, uint8_t TFX, uint16_t CBP, uint8_t CPSM, uint8_t CSM, uint8_t CSA, uint8_t CLD)
    {
      return
        (((uint64_t)(TBP0) <<  0) |
         ((uint64_t)(TBW)  << 14) |
         ((uint64_t)(PSM)  << 20) |
         ((uint64_t)(TW)   << 26) |
         ((uint64_t)(TH)   << 30) |
         ((uint64_t)(TCC)  << 34) |
         ((uint64_t)(TFX)  << 35) |
         ((uint64_t)(CBP)  << 37) |
         ((uint64_t)(CPSM) << 51) |
         ((uint64_t)(CSM)  << 55) |
         ((uint64_t)(CSA)  << 56) |
         ((uint64_t)(CLD)  << 61));
    }
    //#define GS_CLAMP
    //#define GS_FOG
    inline uint64_t XYZF3(uint16_t X, uint16_t Y, uint32_t Z, uint8_t F){return XYZF2(X, Y, Z, F);}
    inline uint64_t XYZ3(uint16_t X, uint16_t Y, uint32_t Z){return XYZ2(X, Y, Z);}
    // TEX1_x Register - Set Texture Information
    //   LCM   - LOD calculation method
    //   MXL   - Maximum MIP level (0-6)
    //   MMAG  - Filter when expanding (0=NEAREST, 1=LINEAR)
    //   MMIN  - Filter when reducing (0=NEAREST, 1=LINEAR)
    //   MTBA  - MIP Base specified by (0=MIPTBP1&2, 1=Automatic)
    //   L     - LOD parameter L
    //   K     - LOD parameter K
    inline uint64_t TEX1(uint8_t LCM, uint8_t MXL, uint8_t MMAG, uint8_t MMIN, uint8_t MTBA, uint8_t L, int16_t K)
    {
      return
        (((uint64_t)(LCM)  <<  0) |
         ((uint64_t)(MXL)  <<  2) |
         ((uint64_t)(MMAG) <<  5) |
         ((uint64_t)(MMIN) <<  6) |
         ((uint64_t)(MTBA) <<  9) |
         ((uint64_t)(L)    << 19) |
         ((uint64_t)(K)    << 32));
    }
    //#define GS_TEX2
    inline uint64_t XYOFFSET(uint16_t OFX, uint16_t OFY)
    {
      return
        (((uint64_t)(OFX) <<  0) |
         ((uint64_t)(OFY) << 32));
    }
    //#define GS_PRMODECONT
    //#define GS_PRMODE
    //#define GS_TEXCLUT
    //#define GS_SCANMSK
    inline uint64_t MIPTBP(uint16_t P1, uint8_t W1, uint16_t P2, uint8_t W2, uint16_t P3, uint8_t W3)
    {
      return
        (((uint64_t)(P1)   <<  0) |
         ((uint64_t)(W1)   << 14) |
         ((uint64_t)(P2)   << 20) |
         ((uint64_t)(W2)   << 34) |
         ((uint64_t)(P3)   << 40) |
         ((uint64_t)(W3)   << 54));
    }
    //#define GS_TEXA
    //#define GS_FOGCOL
    //#define GS_TEXFLUSH
    inline uint64_t SCISSOR(uint16_t X0, uint16_t X1, uint16_t Y0, uint16_t Y1)
    {
      return
        (((uint64_t)(X0) <<  0) |
         ((uint64_t)(X1) << 16) |
         ((uint64_t)(Y0) << 32) |
         ((uint64_t)(Y1) << 48));
    }
    inline uint64_t ALPHA(uint8_t A, uint8_t B, uint8_t C, uint8_t D, uint8_t FIX)
    {
      // Alpha Formula is: Cv = (A-B)*C>>7 + D
      // For A,B,D - (0=texture, 1=frame buffer, 2=0)
      // For C - (0=texture, 1=frame buffer, 2=use FIX field for Alpha)
      return
        (((uint64_t)(A)   <<  0) |
         ((uint64_t)(B)   <<  2) |
         ((uint64_t)(C)   <<  4) |
         ((uint64_t)(D)   <<  6) |
         ((uint64_t)(FIX) << 32));
    }
    // 4x4 Dither Matrix
    inline uint64_t DIMX(uint8_t DM00, uint8_t DM01, uint8_t DM02, uint8_t DM03, uint8_t DM10, uint8_t DM11, uint8_t DM12, uint8_t DM13, uint8_t DM20, uint8_t DM21, uint8_t DM22, uint8_t DM23, uint8_t DM30, uint8_t DM31, uint8_t DM32, uint8_t DM33)
    {
      return
        (((uint64_t)(DM00) <<  0) |
         ((uint64_t)(DM01) <<  4) |
         ((uint64_t)(DM02) <<  8) |
         ((uint64_t)(DM03) << 12) |
         ((uint64_t)(DM10) << 16) |
         ((uint64_t)(DM11) << 20) |
         ((uint64_t)(DM12) << 24) |
         ((uint64_t)(DM13) << 28) |
         ((uint64_t)(DM20) << 32) |
         ((uint64_t)(DM21) << 36) |
         ((uint64_t)(DM22) << 40) |
         ((uint64_t)(DM23) << 44) |
         ((uint64_t)(DM30) << 48) |
         ((uint64_t)(DM31) << 52) |
         ((uint64_t)(DM32) << 56) |
         ((uint64_t)(DM33) << 60));
    }
    //#define GS_DTHE
    //#define GS_COLCLAMP
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
    inline uint64_t TEST(uint8_t ATE, uint8_t ATST, uint8_t AREF, uint8_t AFAIL, uint8_t DATE, uint8_t DATM, uint8_t ZTE, uint8_t ZTST)
    {
      return
        (((uint64_t)(ATE)   <<  0)|
         ((uint64_t)(ATST)  <<  1)|
         ((uint64_t)(AREF)  <<  4)|
         ((uint64_t)(AFAIL) << 12)|
         ((uint64_t)(DATE)  << 14)|
         ((uint64_t)(DATM)  << 15)|
         ((uint64_t)(ZTE)   << 16)|
         ((uint64_t)(ZTST)  << 17));
    }
    //#define GS_PABE
    //#define GS_FBA
    inline uint64_t FRAME(uint16_t FBP, uint8_t FBW, uint8_t PSM, uint32_t FBMSK)
    {
      return
        (((uint64_t)(FBP)   <<  0) |
         ((uint64_t)(FBW)   << 16) |
         ((uint64_t)(PSM)   << 24) |
         ((uint64_t)(FBMSK) << 32));
    }
    inline uint64_t ZBUF(uint16_t ZBP, uint8_t PSM, uint8_t ZMSK)
    {
      return
        (((uint64_t)(ZBP)  <<  0) |
         ((uint64_t)(PSM)  << 24) |
         ((uint64_t)(ZMSK) << 32));
    }
    // BITBLTBUF Register - Setup Image Transfer Between EE and GS
    //   SBP  - Source buffer address (Address/256)
    //   SBW  - Source buffer width (Pixels/64)
    //   SPSM - Source pixel format (0 = 32bit RGBA)
    //   DBP  - Destination buffer address (Address/256)
    //   DBW  - Destination buffer width (Pixels/64)
    //   DPSM - Destination pixel format (0 = 32bit RGBA)
    // When transferring from EE to GS, only the Detination fields
    // need to be set. (Only Source fields for GS->EE, and all for GS->GS).
    inline uint64_t BITBLTBUF(uint16_t SBP, uint8_t SBW, uint8_t SPSM, uint16_t DBP, uint8_t DBW, uint8_t DPSM)
    {
      return
        (((uint64_t)(SBP)  <<  0) |
         ((uint64_t)(SBW)  << 16) |
         ((uint64_t)(SPSM) << 24) |
         ((uint64_t)(DBP)  << 32) |
         ((uint64_t)(DBW)  << 48) |
         ((uint64_t)(DPSM) << 56));
    }
    // TRXPOS Register - Setup Image Transfer Coordinates
    //   SSAX - Source Upper Left X
    //   SSAY - Source Upper Left Y
    //   DSAX - Destionation Upper Left X
    //   DSAY - Destionation Upper Left Y
    //   DIR  - Pixel Transmission Order (00 = top left -> bottom right)
    //
    // - When transferring from EE to GS, only the Detination fields
    //   need to be set. (Only Source fields for GS->EE, and all for GS->GS).
    inline uint64_t TRXPOS(uint16_t SSAX, uint16_t SSAY, uint16_t DSAX, uint16_t DSAY, uint8_t DIR)
    {
      return
        (((uint64_t)(SSAX) <<  0) |
         ((uint64_t)(SSAY) << 16) |
         ((uint64_t)(DSAX) << 32) |
         ((uint64_t)(DSAY) << 48) |
         ((uint64_t)(DIR)  << 59));
    }
    // TRXREG Register - Setup Image Transfer Size
    //   RRW - Image Width
    //   RRH - Image Height
    inline uint64_t TRXREG(uint16_t RRW, uint16_t RRH)
    {
      return
        (((uint64_t)(RRW) <<  0) |
         ((uint64_t)(RRH) << 32));
    }
    // TRXDIR Register - Set Image Transfer Directon, and Start Transfer
    //   XDIR - (0=EE->GS, 1=GS->EE, 2=GS->GS, 3=Transmission is deactivated)
    inline uint64_t TRXDIR(uint8_t XDIR)
    {
      return
        ((uint64_t)(XDIR));
    }
    //#define GS_HWREG
    //#define GS_SIGNAL
    //#define GS_FINISH
    //#define GS_LABEL
  };
  namespace PREG // Packed Registers (GIF will unpack them)
  {
    inline uint64_t RGBA(uint8_t R, uint8_t G, uint8_t B,  uint8_t A)
    {
      return
        (((uint128_t)(R) <<  0) |
         ((uint128_t)(G) << 32) |
         ((uint128_t)(B) << 64) |
         ((uint128_t)(A) << 96));
    }
    inline uint64_t XYZ2(uint16_t X, uint16_t Y, uint32_t Z)
    {
      // X&Y: 16bit signed fixed point integer 12.4
      // z:   32bit unsigned integer
      return
        (((uint128_t)(X) <<  0) |
         ((uint128_t)(Y) << 32) |
         ((uint128_t)(Z) << 68));
    }
    inline uint64_t XYZ3(uint16_t X, uint16_t Y, uint32_t Z)
    {
      // X&Y: 16bit signed fixed point integer 12.4
      // z:   32bit unsigned integer
      return
        (((uint128_t)(X) <<  0) |
         ((uint128_t)(Y) << 32) |
         ((uint128_t)(Z) << 68) |
         ((uint128_t)(1) <<111));
    }
    inline uint64_t XYZF2(uint16_t X, uint16_t Y, uint32_t Z, uint8_t F)
    {
      // X&Y: 16bit signed fixed point integer 12.4
      // z:   24bit unsigned integer
      // f:    8bit unsigned integer
      return
        (((uint128_t)(X) <<   0) |
         ((uint128_t)(Y) <<  32) |
         ((uint128_t)(Z) <<  68) |
         ((uint128_t)(F) << 100));
    }
    inline uint64_t XYZF3(uint16_t X, uint16_t Y, uint32_t Z, uint8_t F)
    {
      // X&Y: 16bit signed fixed point integer 12.4
      // z:   24bit unsigned integer
      // f:    8bit unsigned integer
      return
        (((uint128_t)(X) <<   0) |
         ((uint128_t)(Y) <<  32) |
         ((uint128_t)(Z) <<  68) |
         ((uint128_t)(F) << 100) |
         ((uint128_t)(1) << 111));
    }
  };
};


//-------------------------------------------------------------------------
#define DMA_GIF_TAG(NLOOP, EOP, PRE, PRIM, FLG, NREG) \
  ((uint64_t)(NLOOP) <<  0) | \
  ((uint64_t)(EOP)   << 15) | \
  ((uint64_t)(PRE)   << 46) | \
  ((uint64_t)(PRIM)  << 47) | \
  ((uint64_t)(FLG)   << 58) | \
  ((uint64_t)(NREG)  << 60)


//-------------------------------------------------------------------------
void ee_to_gsBitBlt(uint32_t daddr, int dw, int dpsm, int dx, int dy, int w, int h, uint32_t source);
void gs_to_gsBitBlt(uint32_t daddr, int dw, int dpsm, int dx, int dy, int w, int h, uint32_t source, int sw, int spsm, int sx, int sy);

//-------------------------------------------------------------------------
class CGIFPacket
 : public CSCDMAPacket
{
public:
  CGIFPacket(uint32_t qwSize, EDMAChannel channelId);
  CGIFPacket(void * buffer, uint32_t qwSize, EDMAChannel channelId);
  ~CGIFPacket();

  // Reset packet
  inline void reset();

  // Open GIF Tag
  void gifTagOpenPacked(
    bool embedPrim = false,
    uint16_t prim = 0,
    uint8_t regCount = 1,
    uint8_t reg1  = GIF::REG::ad,
    uint8_t reg2  = GIF::REG::nop,
    uint8_t reg3  = GIF::REG::nop,
    uint8_t reg4  = GIF::REG::nop,
    uint8_t reg5  = GIF::REG::nop,
    uint8_t reg6  = GIF::REG::nop,
    uint8_t reg7  = GIF::REG::nop,
    uint8_t reg8  = GIF::REG::nop,
    uint8_t reg9  = GIF::REG::nop,
    uint8_t reg10 = GIF::REG::nop,
    uint8_t reg11 = GIF::REG::nop,
    uint8_t reg12 = GIF::REG::nop,
    uint8_t reg13 = GIF::REG::nop,
    uint8_t reg14 = GIF::REG::nop,
    uint8_t reg15 = GIF::REG::nop,
    uint8_t reg16 = GIF::REG::nop);
  void gifTagOpenReglist(
    bool embedPrim,
    uint16_t prim,
    uint8_t regCount,
    uint8_t reg1,
    uint8_t reg2  = GIF::REG::nop,
    uint8_t reg3  = GIF::REG::nop,
    uint8_t reg4  = GIF::REG::nop,
    uint8_t reg5  = GIF::REG::nop,
    uint8_t reg6  = GIF::REG::nop,
    uint8_t reg7  = GIF::REG::nop,
    uint8_t reg8  = GIF::REG::nop,
    uint8_t reg9  = GIF::REG::nop,
    uint8_t reg10 = GIF::REG::nop,
    uint8_t reg11 = GIF::REG::nop,
    uint8_t reg12 = GIF::REG::nop,
    uint8_t reg13 = GIF::REG::nop,
    uint8_t reg14 = GIF::REG::nop,
    uint8_t reg15 = GIF::REG::nop,
    uint8_t reg16 = GIF::REG::nop);

  // Close GIF Tag
  void gifTagClose();

  inline void gifAddPackedAD(uint64_t reg, uint64_t data);
  void gifAddSendImage(const void * source, uint32_t size);

  // Pad to 128bit (for faster operation)
  inline void pad128();

protected:
  void * pGIFTag_;
  uint32_t nloopDiv_;
};


#include "gif.inl"


#endif // PS2_GIF_H
