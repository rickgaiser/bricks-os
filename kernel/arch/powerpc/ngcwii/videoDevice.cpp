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


#include "videoDevice.h"
#include "asm/arch/registers.h"
#include <gccore.h>
#include "gx.h"


// NOTE:
//   Embedded FrameBuffer (EFB) is limited to 640x528!
//   eXternal FrameBuffer (XFB) needs to have the same width as EFB
//   Video Interface is limited to 480 lines for NTSC and 574 lines for PAL
// Resulting Maximum resolutions:
//   NTSC: (EFB) 640x480 --> (XFB) 640x480 --> (VI) 720x480
//   PAL:  (EFB) 640x528 --> (XFB) 640x574 --> (VI) 720x574


#define SCREEN_WIDTH           640
#define SCREEN_XPOS            (((720 - SCREEN_WIDTH) / 2) + 0)
#define COMPONENT_CABLE        ((REG_VI_SEL & 0x01) != NULL)


// Video registers for different modes
#define VIDEO_REGISTERS_NTSC   0
#define VIDEO_REGISTERS_PAL    1
#define VIDEO_REGISTERS_MPAL   0
#define VIDEO_REGISTERS_PAL60  0
static const uint32_t videoRegs[6][32] =
{
  { // NTSC 480i60
    ((VI_VTR(480>>1, 6) << 16) |                             // Vertical Timing
      VI_DCR(VI_DCR_NTSC, VI_DCR_ENABLE)),                   // Display Configuration
    VI_HTR0(71, 105, 429),                                   // Horizontal Timing 0
    VI_HTR1_CREATE(413, 122, 64, SCREEN_XPOS, SCREEN_WIDTH), // Horizontal Timing 1  // 0x02EA5140
    VI_VT(3, 24),                                            // Odd Field Vertical Timing
    VI_VT(2, 25),                                            // Even Field Vertical Timing
    VI_BBI(520, 12, 520, 12),
    VI_BBI(519, 13, 519, 13),
    0x00435a4e,
    0x00000000, 0x00435a4e, 0x00000000, 0x00000000,
    0x110701ae, 0x10010001, 0x00010001, 0x00010001,
    0x00000000, 0x00000000,
    ((0x2850) << 16) | VI_HSR_ENABLE | VI_HSR_SCALE(640, SCREEN_WIDTH),
    0x1ae771f0,
    0x0db4a574, 0x00c1188e, 0xc4c0cbe2, 0xfcecdecf,
    0x13130f08, 0x00080c0f, 0x00ff0000, 0x00000000,
    0x02800000, 0x000000ff, 0x00ff00ff, 0x00ff00ff
  },
  { // PAL 576i50
    ((VI_VTR(574>>1, 5) << 16) |                             // Vertical Timing
      VI_DCR(VI_DCR_PAL, VI_DCR_ENABLE)),                    // Display Configuration
    VI_HTR0(75, 106, 432),                                   // Horizontal Timing 0
    VI_HTR1_CREATE(420, 132, 64, SCREEN_XPOS, SCREEN_WIDTH), // Horizontal Timing 1
    VI_VT(1, 35),                                            // Odd Field Vertical Timing
    VI_VT(0, 36),                                            // Even Field Vertical Timing
    VI_BBI(617, 11, 619, 13),
    VI_BBI(620, 10, 618, 12),
    0x00435a4e,
    0x00000000, 0x00435a4e, 0x00000000, 0x013c0144,
    0x113901b1, 0x10010001, 0x00010001, 0x00010001,
    0x00000000, 0x00000000,
    ((0x2850) << 16) | VI_HSR_ENABLE | VI_HSR_SCALE(640, SCREEN_WIDTH),
    0x1ae771f0,
    0x0db4a574, 0x00c1188e, 0xc4c0cbe2, 0xfcecdecf,
    0x13130f08, 0x00080c0f, 0x00ff0000, 0x00000000,
    0x02800000, 0x000000ff, 0x00ff00ff, 0x00ff00ff
  },
};

// NOTE: The sizes used here are for XFB
#define DEFAULT_VIDEO_MODE_NTSC   videoModes[1]
#define DEFAULT_VIDEO_MODE_PAL    videoModes[3]
#define DEFAULT_VIDEO_MODE_MPAL   videoModes[1]
#define DEFAULT_VIDEO_MODE_PAL60  videoModes[1]
static const SVideoMode videoModes[] =
{
  // NTSC
  {640, 480, 640, 480, 16, cfR5G6B5},
  {640, 480, 640, 480, 32, cfA8R8G8B8},
  // PAL (Max. XFB sizes)
  {640, 574, 640, 574, 16, cfR5G6B5},
  {640, 574, 640, 574, 32, cfA8R8G8B8},
  // PAL (Max. EFB sizes)
  {640, 528, 640, 528, 16, cfR5G6B5},
  {640, 528, 640, 528, 32, cfA8R8G8B8},
};
static const int videoModeCount(sizeof(videoModes) / sizeof(SVideoMode));


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
#define RGB2YUV_SHIFT   16
#define RGB2YUV_LUMA    16
#define RGB2YUV_CHROMA 128

#define RGB2YUV_MUL_Yr ((int)( 0.299*(1<<RGB2YUV_SHIFT)))
#define RGB2YUV_MUL_Yg ((int)( 0.587*(1<<RGB2YUV_SHIFT)))
#define RGB2YUV_MUL_Yb ((int)( 0.114*(1<<RGB2YUV_SHIFT)))

#define RGB2YUV_MUL_Ur ((int)(-0.169*(1<<RGB2YUV_SHIFT)))
#define RGB2YUV_MUL_Ug ((int)(-0.331*(1<<RGB2YUV_SHIFT)))
#define RGB2YUV_MUL_Ub ((int)( 0.500*(1<<RGB2YUV_SHIFT)))

#define RGB2YUV_MUL_Vr ((int)( 0.500*(1<<RGB2YUV_SHIFT)))
#define RGB2YUV_MUL_Vg ((int)(-0.419*(1<<RGB2YUV_SHIFT)))
#define RGB2YUV_MUL_Vb ((int)(-0.081*(1<<RGB2YUV_SHIFT)))

#define clamp(x, y, z) ((z < x) ? x : ((z > y) ? y : z))

#define RGB2YUV_Y(r,g,b)  clamp(16, 235, ((RGB2YUV_MUL_Yr * r + RGB2YUV_MUL_Yg * g + RGB2YUV_MUL_Yb * b) >> RGB2YUV_SHIFT) + RGB2YUV_LUMA);
#define RGB2YUV_Cb(r,g,b) clamp(16, 240, ((RGB2YUV_MUL_Ur * r + RGB2YUV_MUL_Ug * g + RGB2YUV_MUL_Ub * b) >> RGB2YUV_SHIFT) + RGB2YUV_CHROMA);
#define RGB2YUV_Cr(r,g,b) clamp(16, 240, ((RGB2YUV_MUL_Vr * r + RGB2YUV_MUL_Vg * g + RGB2YUV_MUL_Vb * b) >> RGB2YUV_SHIFT) + RGB2YUV_CHROMA);


// -----------------------------------------------------------------------------
void
copyEFBToXFB(void * xfbAddr, bool clear)
{
//  GX_LOAD_BP_REG(GX::REG::zmode,        0x1f);
//  GX_LOAD_BP_REG(GX::REG::cmode0,      0x4bc);

  // Set display copy source
  GX_LOAD_BP_REG(GX::REG::cp_src_off,  GX_XY(0, 0));         // Source top left
  GX_LOAD_BP_REG(GX::REG::cp_src_size, GX_XY(640-1, 480)); // Source bottom right
  // Set display copy destination
  GX_LOAD_BP_REG(GX::REG::cp_dst_addr, ((uint32_t)xfbAddr >> 5))
  GX_LOAD_BP_REG(GX::REG::cp_dst_w,    (640>>4));
  // Scaling
  GX_LOAD_BP_REG(GX::REG::cp_yscale,   (1<<8));

//  uint8_t  a(0),r(0),g(0),b(0);
//  uint32_t z(0x00ffffff); // 24bit z-buffer?
//  GX_LOAD_BP_REG(GX::REG::cp_clr_ar,   (a << 8) | r);
//  GX_LOAD_BP_REG(GX::REG::cp_clr_gb,   (g << 8) | b);
//  GX_LOAD_BP_REG(GX::REG::cp_clr_z,    (z));

  // Start copying
  GX_LOAD_BP_REG(GX::REG::cp_ctrl, (clear ? 4803 : 4003));
}

// -----------------------------------------------------------------------------
// 2 rgb pixels --> 2 yuv pixels (yuyv)
inline uint32_t
rgbrgb2yuyv(uint8_t r1, uint8_t g1, uint8_t b1, uint8_t r2, uint8_t g2, uint8_t b2)
{
  uint8_t Y1, Cb, Y2, Cr;

  Y1 = RGB2YUV_Y(r1, g1, b1);
  Y2 = RGB2YUV_Y(r2, g2, b2);

  r1 = (r1 + r2) >> 1;
  g1 = (g1 + g2) >> 1;
  b1 = (b1 + b2) >> 1;

  Cb = RGB2YUV_Cb(r1, g1, b1);
  Cr = RGB2YUV_Cr(r1, g1, b1);

  return ((((uint8_t)Y1) << 24) |
          (((uint8_t)Cb) << 16) |
          (((uint8_t)Y2) <<  8) |
          (((uint8_t)Cr)      ));
}

// -----------------------------------------------------------------------------
// 1 rgb pixel --> 2 yuv pixels (yuyv)
inline uint32_t
rgb2yuyv(uint8_t r, uint8_t g, uint8_t b)
{
  uint8_t Y, Cb, Cr;

  Y  = RGB2YUV_Y(r, g, b);
  Cb = RGB2YUV_Cb(r, g, b);
  Cr = RGB2YUV_Cr(r, g, b);

  return ((((uint8_t)Y ) << 24) |
          (((uint8_t)Cb) << 16) |
          (((uint8_t)Y ) <<  8) |
          (((uint8_t)Cr)      ));
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CNGC2DRenderer::CNGC2DRenderer(CNGCVideoDevice * dev)
 : C2DRenderer()
 , pDev_(dev)
{
}

//---------------------------------------------------------------------------
CNGC2DRenderer::~CNGC2DRenderer()
{
}

//---------------------------------------------------------------------------
void
CNGC2DRenderer::flush()
{
  // BitBlt RGB Surface to Native Surface
  pDev_->flush(pSurface_);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CNGC3DRenderer::CNGC3DRenderer(CNGCVideoDevice * dev)
 : CSoftGLESFloat()
 , pDev_(dev)
{
}

//---------------------------------------------------------------------------
CNGC3DRenderer::~CNGC3DRenderer()
{
}

//---------------------------------------------------------------------------
void
CNGC3DRenderer::flush()
{
  // BitBlt RGB Surface to Native Surface
  pDev_->flush(renderSurface);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CNGCVideoDevice::CNGCVideoDevice()
 : CAVideoDevice()
 , pSurface_(NULL)
 , pSurfaceData_(NULL)
 , pNativeSurface_(NULL)
 , pCurrentMode_(NULL)
 , iFrameCount_(0)
{
}

//---------------------------------------------------------------------------
CNGCVideoDevice::~CNGCVideoDevice()
{
}

//---------------------------------------------------------------------------
void
CNGCVideoDevice::listModes(const SVideoMode ** modes, int * modeCount)
{
  *modes = videoModes;
  *modeCount = videoModeCount;
}

//---------------------------------------------------------------------------
void
CNGCVideoDevice::getCurrentMode(const SVideoMode ** mode)
{
  *mode = pCurrentMode_;
}

//---------------------------------------------------------------------------
void
CNGCVideoDevice::getDefaultMode(const SVideoMode ** mode)
{
  switch(BIOS_TV_MODE)
  {
    case BTM_NTSC:  *mode = &DEFAULT_VIDEO_MODE_NTSC;  break;
    case BTM_MPAL:  *mode = &DEFAULT_VIDEO_MODE_MPAL;  break;
    case BTM_PAL60: *mode = &DEFAULT_VIDEO_MODE_PAL60; break;
    case BTM_PAL:
    default:        *mode = &DEFAULT_VIDEO_MODE_PAL;
  };
}

//---------------------------------------------------------------------------
void
CNGCVideoDevice::setMode(const SVideoMode * mode)
{
  pCurrentMode_ = mode;

  if(mode->height == 480) // NTSC
  {
    // Copy to registers
    for(int i(0); i < 0x20; i++)
      REG_VI_BASE[i] = videoRegs[VIDEO_REGISTERS_NTSC][i];

    // Set horizontal registers (centered, width of 640)
    setHorizontal((720 - 640) / 2, 640);
    // Set vertical registers (centered, height of 480)
    setVertical(0, 480);
    // Set interlacing
    setInterlaced(true);
  }
  else // PAL
  {
    // Copy to registers
    for(int i(0); i < 0x20; i++)
      REG_VI_BASE[i] = videoRegs[VIDEO_REGISTERS_PAL][i];

    // Set horizontal registers (centered, width of 640)
    setHorizontal((720 - 640) / 2, 640);
    // Set vertical registers (centered, height of 574)
    setVertical(1, 574);
    // Set interlacing
    setInterlaced(true);
  }

  // Allocate new aligned YUV surface
  if(pSurfaceData_ != NULL)
    delete pSurfaceData_;
  pSurfaceData_   = new uint8_t[(mode->width * mode->height * 2) + 0x1f];
  pNativeSurface_ = (void *)MEM_K0_TO_K1((uint32_t)(pSurfaceData_ + 0x1f) & (~0x1f));

  REG_VI_XFB1 = (uint32_t)pNativeSurface_;
  REG_VI_XFB2 = ((uint32_t)pNativeSurface_) + (mode->width * 2);
}

//---------------------------------------------------------------------------
void
CNGCVideoDevice::getSurface(CSurface ** surface, int width, int height)
{
  CSurface * pSurface = new CSurface;

  pSurface->mode = *pCurrentMode_;
  pSurface->p    = new uint8_t[width * height * (pCurrentMode_->bpp / 8)];

  *surface = pSurface;
}

//---------------------------------------------------------------------------
void
CNGCVideoDevice::get2DRenderer(I2DRenderer ** renderer)
{
  *renderer = new CNGC2DRenderer(this);
}

//---------------------------------------------------------------------------
void
CNGCVideoDevice::get3DRenderer(I3DRenderer ** renderer)
{
  *renderer = new CNGC3DRenderer(this);
}

//---------------------------------------------------------------------------
uint32_t
CNGCVideoDevice::getFrameNr()
{
  return iFrameCount_;
}

//---------------------------------------------------------------------------
uint32_t
CNGCVideoDevice::waitVSync()
{
  if(pCurrentMode_->height == 480)
  {
    // NTSC REG_VI_HLINE counts from 1 to 575?
    // Field1:  21 - 263
    // Field2: 283 - 525
    while((REG_VI_HLINE != 264) && (REG_VI_HLINE != 526));
  }
  else
  {
    // PAL: REG_VI_HLINE counts from 1 to 625
    // Field1:  23 - 310
    // Field2: 336 - 623
    while((REG_VI_HLINE != 311) && (REG_VI_HLINE != 624));
  }

  return iFrameCount_;
}

//---------------------------------------------------------------------------
void
CNGCVideoDevice::displaySurface(CSurface * surface)
{
  // FIXME: isr should update this, but we don't have interrupts
  iFrameCount_++;

  // Set new surface if it changed
  if((surface != NULL) && (surface != pSurface_))
  {
    pSurface_ = surface;
  }
}

//---------------------------------------------------------------------------
void
CNGCVideoDevice::flush(CSurface * surface)
{
  // FIXME: isr should update this, but we don't have interrupts
  iFrameCount_++;

  if(surface == pSurface_)
  {
    uint32_t * source = (uint32_t *)surface->p;
    uint32_t * dest   = (uint32_t *)pNativeSurface_;
    uint32_t   pixelCount = (surface->mode.width >> 1) * surface->mode.height;
    SColor     pixel1;
    SColor     pixel2;

    while(pixelCount--)
    {
      pixel1.color = *source++;
      pixel2.color = *source++;

      *dest++ = rgbrgb2yuyv(pixel1.r,
                            pixel1.g,
                            pixel1.b,
                            pixel2.r,
                            pixel2.g,
                            pixel2.b);
    }
  }
}

//---------------------------------------------------------------------------
void
CNGCVideoDevice::setHorizontal(uint16_t xoffset, uint16_t width)
{
  // The maximum width is 720
  if(width > 720)
    width = 720;
  // The minimum width is 640
  if(width < 640)
    width = 640;

  if((xoffset + width) > 720)
    xoffset = 720 - width;

  iCurrentHOffset_ = xoffset;

  // Set the horizontal register
  if(pCurrentMode_->height == 480) // NTSC
    REG_VI_HTR1 = VI_HTR1_CREATE(413, 122, 64, xoffset, width);
  else // PAL
    REG_VI_HTR1 = VI_HTR1_CREATE(420, 132, 64, xoffset, width);

  // The maximum framebuffer width is 640, so scale if it has to be larger
  if(width > 640)
    REG_VI_HSR = VI_HSR_ENABLE | VI_HSR_SCALE(640, width);
}

//---------------------------------------------------------------------------
void
CNGCVideoDevice::setVertical(uint16_t yoffset, uint16_t height)
{
  // FIXME
  bool interlaced = true;
  uint8_t  equ;
  uint16_t maxHeight;
  uint8_t  oprb, eprb, opob, epob;
  if(pCurrentMode_->height == 480) // NTSC
  {
    maxHeight = 480;
    equ       =  12;
    oprb      =  24;
    eprb      =  25;
    opob      =   3;
    epob      =   2;
  }
  else // PAL
  {
    maxHeight = 574;
    equ       =  10;
    oprb      =  35;
    eprb      =  36;
    opob      =   1;
    epob      =   0;
  }

  // The maximum height is 480
  if(height > maxHeight)
    height = maxHeight;

  if((yoffset + height) > maxHeight)
    yoffset = maxHeight - height;

  iCurrentVOffset_ = yoffset;

  // Calculate Pre-Blanking
  uint16_t oddPreBlanking   = oprb + (yoffset);
  uint16_t evenPreBlanking  = eprb + (yoffset);
  // Calculate Post-Blanking
  uint16_t oddPostBlanking  = opob + (maxHeight - height - yoffset);
  uint16_t evenPostBlanking = epob + (maxHeight - height - yoffset);

  if(interlaced == true)
  {
    REG_VI_VTR = VI_VTR(height/2, equ/2);
    REG_VI_VTO = (yoffset % 2) ? VI_VT(evenPostBlanking, evenPreBlanking) : VI_VT(oddPostBlanking,  oddPreBlanking);
    REG_VI_VTE = (yoffset % 2) ? VI_VT(oddPostBlanking,  oddPreBlanking)  : VI_VT(evenPostBlanking, evenPreBlanking);
  }
  else
  {
    REG_VI_VTR = VI_VTR(height, equ);
    REG_VI_VTO = (yoffset % 2) ? VI_VT(2 * evenPostBlanking, 2 * evenPreBlanking) : VI_VT(2 * oddPostBlanking,  2 * oddPreBlanking);
    REG_VI_VTE = (yoffset % 2) ? VI_VT(2 * oddPostBlanking,  2 * oddPreBlanking)  : VI_VT(2 * evenPostBlanking, 2 * evenPreBlanking);
  }
}

//---------------------------------------------------------------------------
void
CNGCVideoDevice::setInterlaced(bool interlaced)
{
  if(pCurrentMode_->height == 480) // NTSC
  {
    if(interlaced == true)
    {
      REG_VI_VTR = VI_VTR(pCurrentMode_->height/2, 12/2);
      REG_VI_DCR = VI_DCR(VI_DCR_NTSC, VI_DCR_ENABLE);
    }
    else
    {
      REG_VI_VTR = VI_VTR(pCurrentMode_->height, 12);
      REG_VI_DCR = VI_DCR(VI_DCR_NTSC, VI_DCR_ENABLE | VI_DCR_PROGRESSIVE);
    }
  }
  else // PAL
  {
    if(interlaced == true)
    {
      REG_VI_VTR = VI_VTR(pCurrentMode_->height/2, 10/2);
      REG_VI_DCR = VI_DCR(VI_DCR_PAL, VI_DCR_ENABLE);
    }
    else
    {
      REG_VI_VTR = VI_VTR(pCurrentMode_->height, 10);
      REG_VI_DCR = VI_DCR(VI_DCR_PAL, VI_DCR_ENABLE | VI_DCR_PROGRESSIVE);
    }
  }

  if(interlaced == true)
    REG_VI_CLK = VI_CLK_27MHZ;
  else
    REG_VI_CLK = VI_CLK_54MHZ;
}

//---------------------------------------------------------------------------
bool
CNGCVideoDevice::hasPositioning()
{
  return true;
}

//---------------------------------------------------------------------------
uint16_t
CNGCVideoDevice::getMaxHorizontalOffset()
{
  return (720 - pCurrentMode_->width);
}

//---------------------------------------------------------------------------
uint16_t
CNGCVideoDevice::getHorizontalOffset()
{
  return iCurrentHOffset_;
}

//---------------------------------------------------------------------------
void
CNGCVideoDevice::setHorizontalOffset(uint16_t x)
{
  this->setHorizontal(x, pCurrentMode_->width);
}

//---------------------------------------------------------------------------
uint16_t
CNGCVideoDevice::getMaxVerticalOffset()
{
  if(pCurrentMode_->height == 480)
  {
    // NTSC
    return (486 - pCurrentMode_->height);
  }
  else
  {
    // PAL
    return (576 - pCurrentMode_->height);
  }
}

//---------------------------------------------------------------------------
uint16_t
CNGCVideoDevice::getVerticalOffset()
{
  return iCurrentVOffset_;
}

//---------------------------------------------------------------------------
void
CNGCVideoDevice::setVerticalOffset(uint16_t y)
{
  this->setVertical(y, pCurrentMode_->height);
}
