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
#include "asm/arch/registers.h"


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
    #define FLOAT_TO_INT(F) (*((uint32_t *)&(F)))
    inline uint64_t PRIM (uint8_t prim, bool IIP = false, bool TME = false, bool FGE = false, bool ABE = false, bool AA1 = false, bool FST = false, bool CTXT = false, bool FIX = false){return GS_PRIM(prim,IIP,TME,FGE,ABE,AA1,FST,CTXT,FIX);}
    inline uint64_t RGBAQ(uint8_t r, uint8_t g, uint8_t b,  uint8_t a, float q){return GS_RGBAQ(r, g, b, a, FLOAT_TO_INT(q));}
    inline uint64_t ST   (float s, float t)                                    {return GS_ST(FLOAT_TO_INT(s), FLOAT_TO_INT(t));}
    inline uint64_t UV   (uint16_t u, uint16_t v)                              {return GS_UV(u, v);}
    inline uint64_t XYZF2(uint16_t x, uint16_t y, uint32_t z, uint8_t f)         {return GS_XYZF2(x, y, z, f);}
    inline uint64_t XYZ2 (uint16_t x, uint16_t y, uint32_t z           )         {return GS_XYZ2 (x, y, z   );}
  };
  namespace PACKED
  {
/*
    // X&Y: 16bit signed fixed point integer 12.4
    // z:   24bit unsigned integer
    // f:    8bit unsigned integer
    #define XYZF2(X,Y,Z,F) \
      (((uint128_t)(X) <<   0) | \
       ((uint128_t)(Y) <<  32) | \
       ((uint128_t)(Z) <<  68) | \
       ((uint128_t)(F) << 100))
    #define XYZF3(X,Y,Z,F) \
      (((uint128_t)(X) <<   0) | \
       ((uint128_t)(Y) <<  32) | \
       ((uint128_t)(Z) <<  68) | \
       ((uint128_t)(F) << 100) | \
       ((uint128_t)(1) << 111))
*/
  };
};


//-------------------------------------------------------------------------
// Interlacing
#define NON_INTERLACED          0
#define INTERLACED              1
// Field mode (only if INTERLACED)
#define FIELD                   0       // Scan every other line
#define FRAME                   1       // Scan every line

// Pixel Storage Modes
//   NOTE, how to fully fill the video memory:
//     GRAPH_PSM_24 + GRAPH_PSM_8H = 32bit
//     GRAPH_PSM_24 + GRAPH_PSM_4HH + GRAPH_PSM_4HL = 32bit
//   NOTE2, alignment:
//     framebuffer:  8KiB
//     z-buffer:     8KiB
//     textures:   256Bytes
//     clut:       256Bytes
//   NOTE3, 16bit frame- and z-buffers
//     Always use GRAPH_PSM_16S when only one is 16bit
//     When both frame- and z-buffer are 16bit, use GRAPH_PSM_16
// Frame-, Z- and Textures
#define GRAPH_PSM_32            0x00    // 32 bits per pixel.
#define GRAPH_PSM_24            0x01    // 24 bits per pixel. Stored in bits 0..23 of a 32 bit value.
#define GRAPH_PSM_16            0x02    // 16 bits per pixel.
#define GRAPH_PSM_16S           0x0A    // 16 bits per pixel.
// Textures only
#define GRAPH_PSM_8             0x13    // 8 bits per pixel, palettized.
#define GRAPH_PSM_4             0x14    // 4 bits per pixel, palettized.
#define GRAPH_PSM_8H            0x1B    // 8 bits per pixel, palettized. Stored int bits 24..31 of a 32 bit value
#define GRAPH_PSM_4HH           0x24    // 4 bits per pixel, palettized. Stored int bits 28..31 of a 32 bit value
#define GRAPH_PSM_4HL           0x2C    // 4 bits per pixel, palettized. Stored int bits 24..27 of a 32 bit value

#define GS_X_BASE               1024
#define GS_Y_BASE               1024
#define GS_MEMORY_SIZE          (4*1024*1024)

#define DRAW_FIELD_EVEN()       (GS_REG_CSR & (1 << 13))
#define DISPLAY_FIELD_EVEN()    (!DRAW_FIELD_EVEN)
#define WAIT_VSYNC()            {REG_GS_CSR = REG_GS_CSR & 8; while(!(REG_GS_CSR & 8));}
#define IS_PAL()                (*((char *)0x1fc80000 - 0xae) == 'E')
#define IS_NTSC()               (!IS_PAL())

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
