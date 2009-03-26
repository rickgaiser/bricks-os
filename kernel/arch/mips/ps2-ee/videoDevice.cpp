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
#include "glesContext.h"
#include "bios.h"
#include "dma.h"
#include "gif.h"
#include "string.h"


//---------------------------------------------------------------------------
// Double buffering options in interlaced mode:
//  1: Buffer SWAP with 2 full buffers. FIELD mode.
//      + Frame rate is allowed to drop.
//      - Only half of what is rendered is displayed.
//  2 - Buffer SWAP. 2 half buffers. FRAME mode.
//      + Memory efficient.
//      - Buffers need a half pixel offset.
//      - Keep up the frame rate.
//  3 - Buffer COPY. 1 full, 1 half buffer. FRAME mode.
//      + Vertical aliasing on copy.
//      + 24bit display buffer allows extra 8/4bit textures.

//---------------------------------------------------------------------------
// DISPFB1/2:
//  - Base Pointer (>> 13) / (/ 2048)
//  - Buffer Width (>>  6) / (/   64)
//  - X Pos (in buffer, in pixels)
//  - Y Pos (in buffer, in pixels)
// DISPLAY1/2:
//  - X Pos (on display, in VCK units)
//  - Y Pos (on display, in pixels)
//  - Hor. Magnification (1..16)
//  - Ver. Magnification (1...4)
//  - Display width (in VCK units)
//  - Display height (in pixels)

//---------------------------------------------------------------------------
// Framebuffer:
//  - Start: aligned to 8 KiB (addr >> 13)
//  - Width: aligned to 64 pixels (width >> 6)
// Display:
//  - Disp_Width = FB_Width * MAGH / VCK

//---------------------------------------------------------------------------
const SPS2CRTCMode cmodes[] =
{
  {0x02, 4}, //       NTSC 0, tested
  {0x03, 4}, //        PAL 1, tested

  {0x1a, 2}, //  VGA, 60Hz 2, tested
  {0x1b, 2}, //  VGA, 72Hz 3, tested
  {0x1c, 2}, //  VGA, 75Hz 4, tested
  {0x1d, 2}, //  VGA, 85Hz 5, ERROR
  {0x2a, 2}, // SVGA, 56Hz 6, tested
  {0x2b, 2}, // SVGA, 60Hz 7, tested
  {0x2c, 2}, // SVGA, 72Hz 8, tested
  {0x2d, 2}, // SVGA, 75Hz 9, tested
  {0x2e, 2}, // SVGA, 85Hz 10, tested
  {0x3b, 2}, //  XGA, 60Hz 11, tested
  {0x3c, 1}, //  XGA, 72Hz 12, tested
  {0x3d, 1}, //  XGA, 75Hz 13, tested
  {0x3e, 1}, //  XGA, 85Hz 14, tested
  {0x4a, 1}, // SXGA, 60Hz 15, tested
  {0x4b, 1}, // SXGA, 75Hz 16, tested

  {0x50, 2}, //       480p 17
  {0x51, 1}, //      1080i 18
  {0x52, 1}, //       720p 19
};

//---------------------------------------------------------------------------
// VGA Offsets tested with Samsung SyncMaster 900nf & Dell 1907FP
SPS2VideoMode vmodes[] =
{
  // NTSC 480i60 (525 scanlines, 21-263 & 283-525 = 486 video lines)
  { 640,  448, GS_INTERLACED,     &cmodes[ 0], 160, 50},
  // PAL  576i50 (625 scanlines, 23.5-310 & 336-622.5 = 574 (+2 half) video lines)
  { 640,  512, GS_INTERLACED,     &cmodes[ 1], 176, 70},

  // EDTV
  { 720,  480, GS_NON_INTERLACED, &cmodes[17],  58, 35}, // 480p60
  // HDTV
  {1920, 1080, GS_INTERLACED,     &cmodes[18], 236, 40}, // 1080i60
  {1280,  720, GS_NON_INTERLACED, &cmodes[19], 302, 24}, // 720p60

  // VGA
  { 640,  480, GS_NON_INTERLACED, &cmodes[ 2], 136, 34}, // 60Hz
//  { 640,  480, GS_NON_INTERLACED, &cmodes[ 3], 160, 30}, // 72Hz
//  { 640,  480, GS_NON_INTERLACED, &cmodes[ 4], 176, 18}, // 75Hz
//  { 640,  480, GS_NON_INTERLACED, &cmodes[ 5], 140, 34}, // 85Hz, ERROR
  // SVGA
//  { 800,  600, GS_NON_INTERLACED, &cmodes[ 6], 192, 23}, // 56Hz
  { 800,  600, GS_NON_INTERLACED, &cmodes[ 7], 208, 26}, // 60Hz
//  { 800,  600, GS_NON_INTERLACED, &cmodes[ 8], 176, 28}, // 72Hz
//  { 800,  600, GS_NON_INTERLACED, &cmodes[ 9], 232, 23}, // 75Hz
//  { 800,  600, GS_NON_INTERLACED, &cmodes[10], 208, 29}, // 85Hz
  // XGA
  {1024,  768, GS_NON_INTERLACED, &cmodes[11], 288, 34}, // 60Hz
//  {1024,  768, GS_NON_INTERLACED, &cmodes[12], 264, 34}, // 70Hz
//  {1024,  768, GS_NON_INTERLACED, &cmodes[13], 258, 30}, // 75Hz
//  {1024,  768, GS_NON_INTERLACED, &cmodes[14], 288, 38}, // 85Hz
  // SXGA
  {1280, 1024, GS_NON_INTERLACED, &cmodes[15], 360, 40}, // 60Hz
//  {1280, 1024, GS_NON_INTERLACED, &cmodes[16], 376, 40}, // 75Hz
};
const uint32_t vmode_count(sizeof(vmodes) / sizeof(SPS2VideoMode));

//---------------------------------------------------------------------------
// Note: If the frame size is above 1024KiB, it won't be easy to do 3D:
//
// Front Buffer: 1024KiB
// Back Buffer:  1024KiB
// Z-Buffer:     1024KiB
// Textures:     1024KiB
//               ------- +
// Total:        4096KiB (Entire GS Memory)
//---------------------------------------------------------------------------
#define DEFAULT_VIDEO_MODE_PAL  videoModes[5]  // PAL:  640x512x32
#define DEFAULT_VIDEO_MODE_NTSC videoModes[11] // NTSC: 640x480x32
static const SVideoMode videoModes[] =
{
  // PAL
  { 640,  256,  640,  256, 16, cfA1B5G5R5}, //  320KiB  0
  { 640,  256,  640,  256, 24, cfB8G8R8},   //  480KiB
  { 640,  256,  640,  256, 32, cfA8B8G8R8}, //  640KiB
  { 640,  512,  640,  512, 16, cfA1B5G5R5}, //  320/ 640KiB
  { 640,  512,  640,  512, 24, cfB8G8R8},   //  480/ 960KiB
  { 640,  512,  640,  512, 32, cfA8B8G8R8}, //  640/1280KiB  5
  // NTSC
  { 640,  224,  640,  224, 16, cfA1B5G5R5}, //  280KiB  6
  { 640,  224,  640,  224, 24, cfB8G8R8},   //  420KiB
  { 640,  224,  640,  224, 32, cfA8B8G8R8}, //  560KiB
  { 640,  448,  640,  448, 16, cfA1B5G5R5}, //  280/ 560KiB
  { 640,  448,  640,  448, 24, cfB8G8R8},   //  420/ 840KiB
  { 640,  448,  640,  448, 32, cfA8B8G8R8}, //  560/1120KiB 11

  // EDTV
  { 768,  480,  720,  480, 16, cfA1B5G5R5}, //  720KiB 12
  { 768,  480,  720,  480, 24, cfB8G8R8},   // 1080KiB
  { 768,  480,  720,  480, 32, cfA8B8G8R8}, // 1440KiB 14
  // HDTV
  {1280,  720, 1280,  720, 16, cfA1B5G5R5}, // 1800KiB 15
  {1280,  720, 1280,  720, 24, cfB8G8R8},   // 2700KiB
  {1280,  720, 1280,  720, 32, cfA8B8G8R8}, // 3600KiB
  {1920, 1080, 1920, 1080, 16, cfA1B5G5R5}, // 2025/4050KiB 18

  // VGA
  { 640,  480,  640,  480, 16, cfA1B5G5R5}, //  600KiB 19
  { 640,  480,  640,  480, 24, cfB8G8R8},   //  900KiB
  { 640,  480,  640,  480, 32, cfA8B8G8R8}, // 1200KiB 21
  // SVGA
  { 832,  600,  800,  600, 16, cfA1B5G5R5}, //  975KiB 22
  { 832,  600,  800,  600, 24, cfB8G8R8},   // 1463KiB
  { 832,  600,  800,  600, 32, cfA8B8G8R8}, // 1950KiB 24
  // XGA
  {1024,  768, 1024,  768, 16, cfA1B5G5R5}, // 1536KiB 25
  {1024,  768, 1024,  768, 24, cfB8G8R8},   // 2304KiB
  {1024,  768, 1024,  768, 32, cfA8B8G8R8}, // 3072KiB 27
  // SXGA
  {1280, 1024, 1280, 1024, 16, cfA1B5G5R5}, // 2560KiB 28
};
static const int videoModeCount(sizeof(videoModes) / sizeof(SVideoMode));


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CPS2Surface::CPS2Surface()
 : CSurface()
{
}

//---------------------------------------------------------------------------
CPS2Surface::~CPS2Surface()
{
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CAPS2Renderer::CAPS2Renderer(CSurface * surface)
 : IRenderer()
 , pSurface_(NULL)
 , packet_(1000, DMAC::Channel::GIF)
 , bDataWaiting_(false)
{
  // Create new packet
  packet_.reset();
  packet_.scTagOpenEnd();
  packet_.gifTagOpenPacked();

  setSurface(surface);
}

//---------------------------------------------------------------------------
CAPS2Renderer::~CAPS2Renderer()
{
  flush();
}

//---------------------------------------------------------------------------
void
CAPS2Renderer::setSurface(CSurface * surface)
{
  IRenderer::setSurface(surface);
  pSurface_ = (CPS2Surface *)surface;

  if(pSurface_ != NULL)
  {
    flush();

    packet_.gifAddPackedAD(GIF::REG::frame_1,   GIF::REG::FRAME((uint32_t)pSurface_->p >> 13, pSurface_->mode.xpitch >> 6, pSurface_->psm_, 0));
  }
}

//---------------------------------------------------------------------------
CSurface *
CAPS2Renderer::getSurface()
{
  return pSurface_;
}

//---------------------------------------------------------------------------
void
CAPS2Renderer::flush()
{
  if(bDataWaiting_ == true)
  {
    // Close GIF & DMA tags
    packet_.gifTagClose();
    packet_.scTagClose();

    // Send packet to GS
    packet_.send();

    // Create new packet
    packet_.reset();
    packet_.scTagOpenEnd();
    packet_.gifTagOpenPacked();

    bDataWaiting_ = false;
  }
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CPS22DRenderer::CPS22DRenderer(CSurface * surface)
 : CAPS2Renderer(surface)
{
}

//---------------------------------------------------------------------------
CPS22DRenderer::~CPS22DRenderer()
{
}

//---------------------------------------------------------------------------
void
CPS22DRenderer::setSurface(CSurface * surface)
{
  CAPS2Renderer::setSurface(surface);

  if(pSurface_ != NULL)
  {
    // (Re)set clipper
    clipper_.xmin = 0;
    clipper_.ymin = 0;
    clipper_.xmax = pSurface_->mode.width  - 1;
    clipper_.ymax = pSurface_->mode.height - 1;
  }
}

//---------------------------------------------------------------------------
void
CPS22DRenderer::setColor(color_t rgb)
{
  colorR_ = BxColorFormat_GetR(cfA8R8G8B8, rgb);
  colorG_ = BxColorFormat_GetG(cfA8R8G8B8, rgb);
  colorB_ = BxColorFormat_GetB(cfA8R8G8B8, rgb);

  colorPS2_ = GIF::REG::RGBAQ(colorR_, colorG_, colorB_, 0x80, 0);
  packet_.gifAddPackedAD(GIF::REG::rgbaq, colorPS2_);
}

//---------------------------------------------------------------------------
void
CPS22DRenderer::setColor(uint8_t r, uint8_t g, uint8_t b)
{
  colorR_ = r;
  colorG_ = g;
  colorB_ = b;

  colorPS2_ = GIF::REG::RGBAQ(colorR_, colorG_, colorB_, 0x80, 0);
  packet_.gifAddPackedAD(GIF::REG::rgbaq, colorPS2_);
}

//---------------------------------------------------------------------------
void
CPS22DRenderer::setPixel(int x, int y)
{
  x += GS_X_BASE;
  y += GS_Y_BASE;

  packet_.gifAddPackedAD(GIF::REG::prim,  GIF::REG::PRIM(GS_PRIM_POINT, 0, 0, 0, 0, 0, 0, 0, 0));
  packet_.gifAddPackedAD(GIF::REG::xyz2,  GIF::REG::XYZ2(x<<4, y<<4, 0));
  bDataWaiting_ = true;
}

//---------------------------------------------------------------------------
void
CPS22DRenderer::fill()
{
  fillRect(clipper_.xmin, clipper_.ymin, clipper_.xmax - clipper_.xmin + 1, clipper_.ymax - clipper_.ymin + 1);
}

//---------------------------------------------------------------------------
void
CPS22DRenderer::fillRect(int x, int y, unsigned int width, unsigned int height)
{
  x += GS_X_BASE;
  y += GS_Y_BASE;

  packet_.gifAddPackedAD(GIF::REG::prim,  GIF::REG::PRIM(GS_PRIM_SPRITE, 0, 0, 0, 0, 0, 0, 0, 0));
  packet_.gifAddPackedAD(GIF::REG::xyz2,  GIF::REG::XYZ2((x         << 4), (y          <<4), 0));
  packet_.gifAddPackedAD(GIF::REG::xyz2,  GIF::REG::XYZ2(((x+width) << 4), ((y+height) <<4), 0));
  bDataWaiting_ = true;
}

//---------------------------------------------------------------------------
// FIXME: The last pixel is never drawn, so we need to make the line 1px longer
void
CPS22DRenderer::drawLine(int x1, int y1, int x2, int y2)
{
  x1 += GS_X_BASE;
  y1 += GS_Y_BASE;
  x2 += GS_X_BASE;
  y2 += GS_Y_BASE;

  packet_.gifAddPackedAD(GIF::REG::prim,  GIF::REG::PRIM(GS_PRIM_LINE, 0, 0, 0, 0, 0, 0, 0, 0));
  packet_.gifAddPackedAD(GIF::REG::xyz2,  GIF::REG::XYZ2(x1<<4, y1<<4, 0));
  packet_.gifAddPackedAD(GIF::REG::xyz2,  GIF::REG::XYZ2(x2<<4, y2<<4, 0));
  bDataWaiting_ = true;
}

//---------------------------------------------------------------------------
void
CPS22DRenderer::drawHLine(int x, int y, unsigned int width)
{
  drawLine(x, y, x + width - 1, y);
}

//---------------------------------------------------------------------------
void
CPS22DRenderer::drawVLine(int x, int y, unsigned int height)
{
  drawLine(x, y, x, y + height - 1);
}

//---------------------------------------------------------------------------
void
CPS22DRenderer::drawRect(int x, int y, unsigned int width, unsigned int height)
{
  x += GS_X_BASE;
  y += GS_Y_BASE;

  packet_.gifAddPackedAD(GIF::REG::prim,  GIF::REG::PRIM(GS_PRIM_LINE_STRIP, 0, 0, 0, 0, 0, 0, 0, 0));
  packet_.gifAddPackedAD(GIF::REG::xyz2,  GIF::REG::XYZ2((x           << 4), (y            << 4), 0)); // Top-Left
  packet_.gifAddPackedAD(GIF::REG::xyz2,  GIF::REG::XYZ2(((x+width-1) << 4), (y            << 4), 0)); // Top-Right
  packet_.gifAddPackedAD(GIF::REG::xyz2,  GIF::REG::XYZ2(((x+width-1) << 4), ((y+height-1) << 4), 0)); // Bottom-Right
  packet_.gifAddPackedAD(GIF::REG::xyz2,  GIF::REG::XYZ2((x           << 4), ((y+height-1) << 4), 0)); // Bottom-Left
  packet_.gifAddPackedAD(GIF::REG::xyz2,  GIF::REG::XYZ2((x           << 4), (y            << 4), 0)); // Top-Left
  bDataWaiting_ = true;
}

//---------------------------------------------------------------------------
// Clip point to surface
#define CLIP_POINT_SURF(x,y) \
{ \
  if(x < 0) x = 0; \
  else if((unsigned int)x >= pSurface_->mode.width)  x = pSurface_->mode.width  - 1; \
  if(y < 0) y = 0; \
  else if((unsigned int)y >= pSurface_->mode.height) y = pSurface_->mode.height - 1; \
}
//---------------------------------------------------------------------------
void
CPS22DRenderer::setClipRect(int x, int y, unsigned int width, unsigned int height)
{
  int x2 = x + width  - 1;
  int y2 = y + height - 1;

  // Clip to surface
  CLIP_POINT_SURF(x,  y);
  CLIP_POINT_SURF(x2, y2);

  clipper_.xmin = x;
  clipper_.ymin = y;
  clipper_.xmax = x2;
  clipper_.ymax = y2;

  packet_.gifAddPackedAD(GIF::REG::scissor_1, GIF::REG::SCISSOR(clipper_.xmin, clipper_.xmax, clipper_.ymin, clipper_.ymax));
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CPS2VideoDevice::CPS2VideoDevice()
 : CAVideoDevice()
 , pSurface_(NULL)
 , packet_(50, DMAC::Channel::GIF)
 , pCurrentMode_(NULL)
 , pCurrentPS2Mode_(NULL)
 , freeMemAddr_(0)
 , iFrameCount_(0)
#ifndef CONFIG_MULTI_THREADING
 , bSwap_(false)
#endif
{
//  CInterruptManager::attach(INT_GS,           this);
  CInterruptManager::attach(INT_VBLANK_START, this);
//  CInterruptManager::attach(INT_VBLANK_END,   this);
}

//---------------------------------------------------------------------------
CPS2VideoDevice::~CPS2VideoDevice()
{
}

//---------------------------------------------------------------------------
int
CPS2VideoDevice::isr(int irq)
{
  switch(irq)
  {
    case INT_GS:
    {
      break;
    }
    case INT_VBLANK_START:
    {
      // Increase framecount
      iFrameCount_++;

      // Sync framecount with odd/even signal
      if(REG_GS_CSR & GS_CSR_ODD)
        iFrameCount_ |= 1;

      // Notify swap function that we have vertical sync
#ifdef CONFIG_MULTI_THREADING
      k_pthread_cond_broadcast(&condVSync_);
#else
      if(bSwap_ == true)
        bSwap_ = false;
#endif

      break;
    }
    case INT_VBLANK_END:
    {
      break;
    }
  };

  return 0;
}

//---------------------------------------------------------------------------
void
CPS2VideoDevice::listModes(const SVideoMode ** modes, int * modeCount)
{
  *modes = videoModes;
  *modeCount = videoModeCount;
}

//---------------------------------------------------------------------------
void
CPS2VideoDevice::getCurrentMode(const SVideoMode ** mode)
{
  *mode = pCurrentMode_;
}

//---------------------------------------------------------------------------
void
CPS2VideoDevice::getDefaultMode(const SVideoMode ** mode)
{
  if(IS_PAL() == true)
    *mode = &DEFAULT_VIDEO_MODE_PAL;
  else
    *mode = &DEFAULT_VIDEO_MODE_NTSC;
}

//---------------------------------------------------------------------------
// Originally from Tony Saveski
void
CPS2VideoDevice::setMode(const SVideoMode * mode)
{
  SPS2VideoMode * pPS2Mode = NULL;

  // Find corresponding PS2 video mode
  //  - Validates width and height
  for(uint32_t i(0); i < vmode_count; i++)
  {
    if(mode->width == vmodes[i].width)
    {
      // Find progressive / interlaced video modes
      if((uint16_t)mode->height == vmodes[i].height)
      {
        actualHeight_       = vmodes[i].height;
        currentDoubleScan_  = 0;
        currentInterlaced_  = vmodes[i].interlaced;
        // GS_FIELD = Display only HALF of the lines! (what a waste of power)
        // GS_FRAME = Display EVERY line. (require special processing)
        currentField_       = GS_FIELD;
        pPS2Mode            = &vmodes[i];
        break;
      }

      // Find fake progressive modes (double scanned interlaced modes)
      if(((uint16_t)mode->height == (vmodes[i].height/2)) && (vmodes[i].interlaced == GS_INTERLACED))
      {
        actualHeight_       = vmodes[i].height / 2;
        currentDoubleScan_  = 1;
        currentInterlaced_  = GS_NON_INTERLACED;
        currentField_       = 0; // Not used in (fake) progressive mode
        pPS2Mode            = &vmodes[i];
        break;
      }
    }
  }

  if(pPS2Mode == NULL)
    return;

  // Validate BPP
  switch(mode->bpp)
  {
    case 16: currentPSM_ = GS_PSM_16S; break;
    case 24: currentPSM_ = GS_PSM_24;  break;
    case 32: currentPSM_ = GS_PSM_32;  break;
    default:
      return;
  };

  // Validate total memory requirement of framebuffer
  if((mode->xpitch * mode->height * (mode->bpp / 8)) > GS_MEMORY_SIZE)
    return;

  pCurrentMode_    = mode;
  pCurrentPS2Mode_ = pPS2Mode;

  // Reset the GS
  REG_GS_CSR = GS_CSR_RESET();

  // Sync
  __asm__("sync.p\nnop");

  // Setup CRTC for video mode
  bios::SetGsCrt(currentInterlaced_, pCurrentPS2Mode_->crtcMode->biosMode, currentField_);

  // Enable read circuit 1
  REG_GS_PMODE = GS_PMODE(
    1,        // ReadCircuit1 ON
    0,        // ReadCircuit2 OFF
    1,        // Use ALP register for Alpha Blending
    1,        // Alpha Value of ReadCircuit2 for output selection
    0,        // Blend Alpha with the output of ReadCircuit2
    0xff      // Alpha Value = 1.0
  );

  // Setup read circuit 1
  REG_GS_DISPLAY1 = GS_DISPLAY_CREATE(pCurrentPS2Mode_->crtcMode->vck,
                                      pCurrentPS2Mode_->xoffset,
                                      pCurrentPS2Mode_->yoffset >> currentDoubleScan_,
                                      1 * pCurrentPS2Mode_->crtcMode->vck, // MAGH
                                      1,                                   // MAGV
                                      pCurrentPS2Mode_->width,
                                      actualHeight_);

//  REG_GS_BGCOLOR = GS_BGCOLOR(128, 128, 128);

  // Setup read circuit 2
  //REG_GS_DISPLAY2 = ...;

  packet_.scTagOpenEnd();
    packet_.gifTagOpenPacked();
      // Use drawing parameters from PRIM register
      packet_.gifAddPackedAD(GIF::REG::prmodecont, 1);
      // Setup frame buffers. Point to 0 initially.
      packet_.gifAddPackedAD(GIF::REG::frame_1,    GIF::REG::FRAME(0, pCurrentPS2Mode_->width >> 6, currentPSM_, 0));
      // Displacement between Primitive and Window coordinate systems.
      packet_.gifAddPackedAD(GIF::REG::xyoffset_1, GIF::REG::XYOFFSET(GS_X_BASE<<4, GS_Y_BASE<<4));
      // Clip to frame buffer.
      packet_.gifAddPackedAD(GIF::REG::scissor_1,  GIF::REG::SCISSOR(0, pCurrentPS2Mode_->width - 1, 0, pCurrentPS2Mode_->height - 1));
      // Clamp colors
      packet_.gifAddPackedAD(GIF::REG::colclamp,   1);
      // Dithering
      if(mode->bpp == 16)
      {
        // -4 == 100b == 4
        // -3 == 101b == 5
        // -2 == 110b == 6
        // -1 == 111b == 7
        //  0 == 000b == 0
        //  1 == 001b == 1
        //  2 == 010b == 2
        //  3 == 011b == 3
        // Enable dithering
        packet_.gifAddPackedAD(GIF::REG::dimx, GIF::REG::DIMX(4,2,5,3,0,6,1,7,5,3,4,2,1,7,0,6));
        packet_.gifAddPackedAD(GIF::REG::dthe, 1);
      }
      else
      {
        // Disable dithering
        packet_.gifAddPackedAD(GIF::REG::dthe, 0);
      }
    packet_.gifTagClose();
  packet_.scTagClose();
  packet_.send();
  packet_.reset();
}

//---------------------------------------------------------------------------
void
CPS2VideoDevice::getSurface(CSurface ** surface, int width, int height)
{
  void * pAddr;

  if(allocFramebuffer((uint32_t &)pAddr, pCurrentMode_->xpitch, pCurrentMode_->ypitch, currentPSM_) == true)
  {
    CPS2Surface * pSurface = new CPS2Surface;

    pSurface->mode        = *pCurrentMode_;
    pSurface->psm_        = currentPSM_;
    pSurface->p           = pAddr;

    *surface = pSurface;
  }
}

//---------------------------------------------------------------------------
void
CPS2VideoDevice::get2DRenderer(I2DRenderer ** renderer)
{
  *renderer = new CPS22DRenderer;
}

//---------------------------------------------------------------------------
void
CPS2VideoDevice::get3DRenderer(I3DRenderer ** renderer)
{
  *renderer = new CPS2GLESContext(*this);
}

//---------------------------------------------------------------------------
uint32_t
CPS2VideoDevice::getFrameNr()
{
  return iFrameCount_;
}

//---------------------------------------------------------------------------
uint32_t
CPS2VideoDevice::waitVSync()
{
#ifdef CONFIG_MULTI_THREADING
  k_pthread_cond_wait(&condVSync_, &mutex_);
#else
  #if 0
  bSwap_ = true;
  while(bSwap_ == true){}
  #else
  REG_GS_CSR = REG_GS_CSR & 8;
  while(!(REG_GS_CSR & 8));
  #endif
#endif

  return iFrameCount_;
}

//---------------------------------------------------------------------------
void
CPS2VideoDevice::displaySurface(CSurface * surface)
{
  CPS2Surface * pNewSurface = (CPS2Surface *)surface;

  // Set new surface
  if(pNewSurface != NULL)
  {
    pSurface_ = pNewSurface;

    // Set visible frame
    REG_GS_DISPFB1 = GS_DISPFB((uint32_t)pSurface_->p >> 13, pSurface_->mode.xpitch >> 6, pSurface_->psm_, 0, 0);
  }
}

//---------------------------------------------------------------------------
void
CPS2VideoDevice::bitBlt(CSurface * dest, int dx, int dy, int w, int h, CSurface * source, int sx, int sy)
{
  CPS2Surface * pDest   = (CPS2Surface *)dest;
  CPS2Surface * pSource = (CPS2Surface *)source;

  packet_.scTagOpenEnd();
    packet_.gifTagOpenPacked();
      packet_.gifAddPackedAD(GIF::REG::bitbltbuf, GIF::REG::BITBLTBUF(
        ((uint32_t)pSource->p)>>8,
        pSource->mode.xpitch>>6,
        pSource->psm_,
        ((uint32_t)pDest->p)>>8,
        pDest->mode.xpitch>>6,
        pDest->psm_));
      packet_.gifAddPackedAD(GIF::REG::trxpos,    GIF::REG::TRXPOS(sx, sy, dx, dy, 0));
      packet_.gifAddPackedAD(GIF::REG::trxreg,    GIF::REG::TRXREG(w, h));
      packet_.gifAddPackedAD(GIF::REG::trxdir,    GIF::REG::TRXDIR(GS_TRXDIR_GS_TO_GS));
    packet_.gifTagClose();
  packet_.scTagClose();
  packet_.send();
  packet_.reset();
}

//---------------------------------------------------------------------------
bool
CPS2VideoDevice::allocFramebuffer(uint32_t & addr, int w, int h, uint16_t psm)
{
  // Width must be multiple of 64
  if((w & 0x3f) != 0)
    return false;

  // Limit the max size
  if((w > 1920) || (h > 1080))
    return false;

  uint32_t iSize = w * h;

  switch(psm)
  {
    case GS_PSM_16:
    case GS_PSM_16S:
      iSize *= 2;
      break;
    case GS_PSM_24:
    case GS_PSM_32:
      iSize *= 4;
      break;
    default:
      return false;
  };

  // Align to 8KiB
  addr = (freeMemAddr_ + 0x00001fff) & (~0x00001fff);

  if((addr + iSize) > GS_MEMORY_SIZE)
    return false;

  // Save new addr
  freeMemAddr_ = addr + iSize;

  return true;
}

//---------------------------------------------------------------------------
bool
CPS2VideoDevice::allocTexture(uint32_t & addr, int w, int h, uint16_t psm)
{
  // Width must be multiple of 64
  //if((w & 0x3f) != 0)
  //  return false;

  // Limit the max size
  if((w > 1024) || (h > 1024))
    return false;

  uint32_t iSize = w * h;

  switch(psm)
  {
//    case GS_PSM_4HL:
//    case GS_PSM_4HH:
//    case GS_PSM_8H:
    case GS_PSM_4:
      iSize /= 2;
      break;
    case GS_PSM_8:
      break;
    case GS_PSM_16:
    case GS_PSM_16S:
      iSize *= 2;
      break;
    case GS_PSM_24:
    case GS_PSM_32:
      iSize *= 4;
      break;
    default:
      return false;
  };

  // Align to 256Byte
  addr = (freeMemAddr_ + 0x000000ff) & (~0x000000ff);

  if((addr + iSize) > GS_MEMORY_SIZE)
    return false;

  // Save new addr
  freeMemAddr_ = addr + iSize;

  return true;
}

//---------------------------------------------------------------------------
bool
CPS2VideoDevice::hasPositioning()
{
  return true;
}

//---------------------------------------------------------------------------
uint16_t
CPS2VideoDevice::getMaxHorizontalOffset()
{
  return (704 - pCurrentMode_->width);
}

//---------------------------------------------------------------------------
uint16_t
CPS2VideoDevice::getHorizontalOffset()
{
  return iCurrentHOffset_;
}

#define HADD 140
#define VADD  40
//---------------------------------------------------------------------------
void
CPS2VideoDevice::setHorizontalOffset(uint16_t x)
{
  iCurrentHOffset_ = x;
/*
  // Setup read circuit 1
  REG_GS_DISPLAY1 = GS_DISPLAY_CREATE(pCurrentPS2Mode_->crtcMode->vck,
                                      (iCurrentHOffset_ + HADD),
                                      (iCurrentVOffset_ + VADD) >> currentDoubleScan_,
                                      1 * pCurrentPS2Mode_->crtcMode->vck, // MAGH
                                      1,                                   // MAGV
                                      pCurrentPS2Mode_->width,
                                      actualHeight_);
*/
}

//---------------------------------------------------------------------------
uint16_t
CPS2VideoDevice::getMaxVerticalOffset()
{
//  if(pCurrentMode_->height == 480)
//  {
    // NTSC
    return (486 - pCurrentMode_->height);
//  }
//  else
//  {
//    // PAL
//    return (576 - pCurrentMode_->height);
//  }
}

//---------------------------------------------------------------------------
uint16_t
CPS2VideoDevice::getVerticalOffset()
{
  return iCurrentVOffset_;
}

//---------------------------------------------------------------------------
void
CPS2VideoDevice::setVerticalOffset(uint16_t y)
{
  iCurrentVOffset_ = y;
/*
  // Setup read circuit 1
  REG_GS_DISPLAY1 = GS_DISPLAY_CREATE(pCurrentPS2Mode_->crtcMode->vck,
                                      (iCurrentHOffset_ + HADD),
                                      (iCurrentVOffset_ + VADD) >> currentDoubleScan_,
                                      1 * pCurrentPS2Mode_->crtcMode->vck, // MAGH
                                      1,                                   // MAGV
                                      pCurrentPS2Mode_->width,
                                      actualHeight_);
*/
}
