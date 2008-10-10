#include "videoDevice.h"
#include "glesContext.h"
#include "bios.h"
#include "dma.h"
#include "gs.h"
#include "string.h"


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
  {0x02, 4}, //       NTSC 0
  {0x03, 4}, //        PAL 1

  {0x1a, 2}, //  VGA, 60Hz 2
  {0x1b, 2}, //  VGA, 72Hz 3
  {0x1c, 2}, //  VGA, 75Hz 4
  {0x1d, 2}, //  VGA, 85Hz 5
  {0x2a, 2}, // SVGA, 56Hz 6
  {0x2b, 2}, // SVGA, 60Hz 7
  {0x2c, 2}, // SVGA, 72Hz 8
  {0x2d, 2}, // SVGA, 75Hz 9
  {0x2e, 2}, // SVGA, 85Hz 10
  {0x3b, 1}, //  XGA, 60Hz 11
  {0x3c, 1}, //  XGA, 72Hz 12
  {0x3d, 1}, //  XGA, 75Hz 13
  {0x3e, 1}, //  XGA, 85Hz 14
  {0x4a, 1}, // SXGA, 60Hz 15
  {0x4b, 1}, // SXGA, 75Hz 16

  {0x50, 2}, //       480p 17
  {0x51, 1}, //      1080i 18
  {0x52, 1}, //       720p 19
};

//---------------------------------------------------------------------------
SPS2VideoMode vmodes[] =
{
  // SDTV
  { 640,  448, INTERLACED,     &cmodes[ 0], 158, 50}, // NTSC 480i60
  { 640,  512, INTERLACED,     &cmodes[ 1], 163, 72}, // PAL  576i50
  // VGA
//  { 640,  480, NON_INTERLACED, &cmodes[ 2], 138, 34}, // 60Hz
//  { 640,  480, NON_INTERLACED, &cmodes[ 3], 138, 34}, // 72Hz
  { 640,  480, NON_INTERLACED, &cmodes[ 4], 138, 34}, // 75Hz
//  { 640,  480, NON_INTERLACED, &cmodes[ 5], 138, 34}, // 85Hz
  // SVGA
//  { 800,  600, NON_INTERLACED, &cmodes[ 6], 210, 26}, // 56Hz
//  { 800,  600, NON_INTERLACED, &cmodes[ 7], 210, 26}, // 60Hz
//  { 800,  600, NON_INTERLACED, &cmodes[ 8], 210, 26}, // 72Hz
  { 800,  600, NON_INTERLACED, &cmodes[ 9], 210, 26}, // 75Hz
//  { 800,  600, NON_INTERLACED, &cmodes[10], 210, 26}, // 85Hz
  // XGA
//  {1024,  768, NON_INTERLACED, &cmodes[11], 270, 34}, // 60Hz
//  {1024,  768, NON_INTERLACED, &cmodes[12], 270, 34}, // 70Hz
  {1024,  768, NON_INTERLACED, &cmodes[13], 270, 34}, // 75Hz
//  {1024,  768, NON_INTERLACED, &cmodes[14], 270, 34}, // 85Hz
  // SXGA
//  {1280, 1024, NON_INTERLACED, &cmodes[15], 348, 40}, // 60Hz
  {1280, 1024, NON_INTERLACED, &cmodes[16], 348, 40}, // 75Hz
  // EDTV
  { 720,  480, NON_INTERLACED, &cmodes[17], 116, 35}, // 480p60
  // HDTV
  {1920, 1080, INTERLACED,     &cmodes[18], 238, 40}, // 1080i60
  {1280,  720, NON_INTERLACED, &cmodes[19], 302, 24}, // 720p60
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
  { 720,  480,  768,  480, 16, cfA1B5G5R5}, //  720KiB 12
  { 720,  480,  768,  480, 24, cfB8G8R8},   // 1080KiB
  { 720,  480,  768,  480, 32, cfA8B8G8R8}, // 1440KiB 13
  // HDTV
  {1280,  720, 1280,  720, 16, cfA1B5G5R5}, // 1800KiB 15
  {1280,  720, 1280,  720, 24, cfB8G8R8},   // 2700KiB
  {1280,  720, 1280,  720, 32, cfA8B8G8R8}, // 3600KiB
  {1920,  540, 1920,  540, 16, cfA1B5G5R5}, // 2025KiB
  {1920,  540, 1920,  540, 24, cfB8G8R8},   // 3038KiB
  {1920,  540, 1920,  540, 32, cfA8B8G8R8}, // 4050KiB
  {1920, 1080, 1920, 1080, 16, cfA1B5G5R5}, // 2025/4050KiB 21

  // VGA
  { 640,  480,  640,  480, 16, cfA1B5G5R5}, //  600KiB 22
  { 640,  480,  640,  480, 24, cfB8G8R8},   //  900KiB
  { 640,  480,  640,  480, 32, cfA8B8G8R8}, // 1200KiB 24
  // SVGA
  { 800,  600,  832,  600, 16, cfA1B5G5R5}, //  975KiB 25
  { 800,  600,  832,  600, 24, cfB8G8R8},   // 1463KiB
  { 800,  600,  832,  600, 32, cfA8B8G8R8}, // 1950KiB 27
  // XGA
  {1024,  768, 1024,  768, 16, cfA1B5G5R5}, // 1536KiB 28
  {1024,  768, 1024,  768, 24, cfB8G8R8},   // 2304KiB
  {1024,  768, 1024,  768, 32, cfA8B8G8R8}, // 3072KiB 30
  // SXGA
  {1280, 1024, 1280, 1024, 16, cfA1B5G5R5}, // 2560KiB 31
};
static const int videoModeCount(sizeof(videoModes) / sizeof(SVideoMode));


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CPS2Surface::CPS2Surface()
 : CSurface()
 , psm_(GRAPH_PSM_16)
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

    packet_.gifAddPackedAD(GIF::REG::frame_1, GS_FRAME((uint32_t)pSurface_->p >> 13, pSurface_->mode.xpitch >> 6, pSurface_->psm_, 0));
    packet_.gifAddPackedAD(GIF::REG::scissor_1, GS_SCISSOR(0, pSurface_->mode.width, 0, pSurface_->mode.height));
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
CPS22DRenderer::setColor(color_t rgb)
{
  color_.r = BxColorFormat_GetR(cfA8R8G8B8, rgb);
  color_.g = BxColorFormat_GetG(cfA8R8G8B8, rgb);
  color_.b = BxColorFormat_GetB(cfA8R8G8B8, rgb);
  color_.a = BxColorFormat_GetA(cfA8R8G8B8, rgb);
}

//---------------------------------------------------------------------------
void
CPS22DRenderer::setColor(uint8_t r, uint8_t g, uint8_t b)
{
  color_.r = r;
  color_.g = g;
  color_.b = b;
  color_.a = 255;
}

//---------------------------------------------------------------------------
void
CPS22DRenderer::setPixel(int x, int y)
{
  x += GS_X_BASE;
  y += GS_Y_BASE;

  packet_.gifAddPackedAD(GIF::REG::prim,  GIF::REG::PRIM(PRIM_POINT, 0, 0, 0, 0, 0, 0, 0, 0));
  packet_.gifAddPackedAD(GIF::REG::rgbaq, GIF::REG::RGBAQ(color_.r, color_.g, color_.b, 0x80, 0));
  packet_.gifAddPackedAD(GIF::REG::xyz2,  GIF::REG::XYZ2(x<<4, y<<4, 0));
  bDataWaiting_ = true;
}

//---------------------------------------------------------------------------
void
CPS22DRenderer::fill()
{
  fillRect(0, 0, pSurface_->mode.width, pSurface_->mode.height);
}

//---------------------------------------------------------------------------
void
CPS22DRenderer::fillRect(int x, int y, unsigned int width, unsigned int height)
{
  x += GS_X_BASE;
  y += GS_Y_BASE;

  packet_.gifAddPackedAD(GIF::REG::prim,  GIF::REG::PRIM(PRIM_SPRITE, 0, 0, 0, 0, 0, 0, 0, 0));
  packet_.gifAddPackedAD(GIF::REG::rgbaq, GIF::REG::RGBAQ(color_.r, color_.g, color_.b, 0x80, 0));
  packet_.gifAddPackedAD(GIF::REG::xyz2,  GIF::REG::XYZ2(x<<4, y<<4, 0));
  packet_.gifAddPackedAD(GIF::REG::xyz2,  GIF::REG::XYZ2((x+width)<<4, (y+height)<<4, 0));
  bDataWaiting_ = true;
}

//---------------------------------------------------------------------------
void
CPS22DRenderer::drawLine(int x1, int y1, int x2, int y2)
{
  x1 += GS_X_BASE;
  y1 += GS_Y_BASE;
  x2 += GS_X_BASE;
  y2 += GS_Y_BASE;

  packet_.gifAddPackedAD(GIF::REG::prim,  GIF::REG::PRIM(PRIM_LINE, 0, 0, 0, 0, 0, 0, 0, 0));
  packet_.gifAddPackedAD(GIF::REG::rgbaq, GIF::REG::RGBAQ(color_.r, color_.g, color_.b, 0x80, 0));
  packet_.gifAddPackedAD(GIF::REG::xyz2,  GIF::REG::XYZ2(x1<<4, y1<<4, 0));
  packet_.gifAddPackedAD(GIF::REG::xyz2,  GIF::REG::XYZ2(x2<<4, y2<<4, 0));
  bDataWaiting_ = true;
}

//---------------------------------------------------------------------------
void
CPS22DRenderer::drawHLine(int x, int y, unsigned int width)
{
  drawLine(x, y, x + width, y);
}

//---------------------------------------------------------------------------
void
CPS22DRenderer::drawVLine(int x, int y, unsigned int height)
{
  drawLine(x, y, x, y + height);
}

//---------------------------------------------------------------------------
void
CPS22DRenderer::drawRect(int x, int y, unsigned int width, unsigned int height)
{
  x += GS_X_BASE;
  y += GS_Y_BASE;

  packet_.gifAddPackedAD(GIF::REG::prim,  GIF::REG::PRIM(PRIM_LINE_STRIP, 0, 0, 0, 0, 0, 0, 0, 0));
  packet_.gifAddPackedAD(GIF::REG::rgbaq, GIF::REG::RGBAQ(color_.r, color_.g, color_.b, 0x80, 0));
  packet_.gifAddPackedAD(GIF::REG::xyz2,  GIF::REG::XYZ2(x<<4, y<<4, 0));
  packet_.gifAddPackedAD(GIF::REG::xyz2,  GIF::REG::XYZ2(x+width<<4, y<<4, 0));
  packet_.gifAddPackedAD(GIF::REG::xyz2,  GIF::REG::XYZ2(x+width<<4, y+height<<4, 0));
  packet_.gifAddPackedAD(GIF::REG::xyz2,  GIF::REG::XYZ2(x<<4, y+height<<4, 0));
  packet_.gifAddPackedAD(GIF::REG::xyz2,  GIF::REG::XYZ2(x<<4, y<<4, 0));
  bDataWaiting_ = true;
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
{
}

//---------------------------------------------------------------------------
CPS2VideoDevice::~CPS2VideoDevice()
{
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
        currentField_       = FIELD;
        pPS2Mode            = &vmodes[i];
        break;
      }

      // Find fake progressive modes (double scanned interlaced modes)
      if(((uint16_t)mode->height == (vmodes[i].height/2)) && (vmodes[i].interlaced == INTERLACED))
      {
        actualHeight_       = vmodes[i].height / 2;
        currentDoubleScan_  = 1;
        currentInterlaced_  = NON_INTERLACED;
        currentField_       = FIELD;
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
    case 16: currentPSM_ = GRAPH_PSM_16S; break;
    case 24: currentPSM_ = GRAPH_PSM_24;  break;
    case 32: currentPSM_ = GRAPH_PSM_32;  break;
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

  // Clear GS interrupt mask register
  bios::GsPutIMR(0);

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

  // Setup read circuit 2
  //REG_GS_DISPLAY2 = ...;

  packet_.scTagOpenEnd();
    packet_.gifTagOpenPacked();
      // Use drawing parameters from PRIM register
      packet_.gifAddPackedAD(GIF::REG::prmodecont, 1);
      // Setup frame buffers. Point to 0 initially.
      packet_.gifAddPackedAD(GIF::REG::frame_1,    GS_FRAME(0, pCurrentPS2Mode_->width >> 6, currentPSM_, 0));
      // Displacement between Primitive and Window coordinate systems.
      packet_.gifAddPackedAD(GIF::REG::xyoffset_1, GS_XYOFFSET(GS_X_BASE<<4, GS_Y_BASE<<4));
      // Clip to frame buffer.
      packet_.gifAddPackedAD(GIF::REG::scissor_1,  GS_SCISSOR(0, pCurrentPS2Mode_->width, 0, actualHeight_));
    packet_.gifTagClose();
  packet_.scTagClose();
  packet_.send();
  packet_.reset();
}

//---------------------------------------------------------------------------
void
CPS2VideoDevice::getSurface(CSurface ** surface, int width, int height)
{
  void * pAddr = allocFramebuffer(pCurrentMode_->xpitch, pCurrentMode_->ypitch, currentPSM_);

  if(pAddr != NULL)
  {
    CPS2Surface * pSurface = new CPS2Surface;

    pSurface->mode        = *pCurrentMode_;
    pSurface->mode.width  = width;
    pSurface->mode.height = height;
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
  WAIT_VSYNC();

  return iFrameCount_;
}

//---------------------------------------------------------------------------
void
CPS2VideoDevice::displaySurface(CSurface * surface)
{
  CPS2Surface * pNewSurface = (CPS2Surface *)surface;

  // Always VSync, even if the frame is not new.
  if(vSync_ == true)
    WAIT_VSYNC();

  // FIXME: isr should update this, but we don't have interrupts
  iFrameCount_++;

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
      packet_.gifAddPackedAD(GIF::REG::bitbltbuf, GS_BITBLTBUF(
        ((uint32_t)pSource->p)>>8,
        pSource->mode.xpitch>>6,
        pSource->psm_,
        ((uint32_t)pDest->p)>>8,
        pDest->mode.xpitch>>6,
        pDest->psm_));
      packet_.gifAddPackedAD(GIF::REG::trxpos,    GS_TRXPOS(sx, sy, dx, dy, 0));
      packet_.gifAddPackedAD(GIF::REG::trxreg,    GS_TRXREG(w, h));
      packet_.gifAddPackedAD(GIF::REG::trxdir,    GS_TRXDIR(XDIR_GS_GS));
    packet_.gifTagClose();
  packet_.scTagClose();
  packet_.send();
  packet_.reset();
}

//---------------------------------------------------------------------------
void *
CPS2VideoDevice::allocFramebuffer(int w, int h, uint16_t psm)
{
  uint32_t addr;

  // Width must be multiple of 64
  if((w & 0x3f) != 0)
    return NULL;

  // Limit the max size
  if((w > 1920) || (h > 1080))
    return NULL;

  uint32_t iSize = w * h;

  switch(psm)
  {
    case GRAPH_PSM_16:
    case GRAPH_PSM_16S:
      iSize *= 2;
      break;
    case GRAPH_PSM_24:
    case GRAPH_PSM_32:
      iSize *= 4;
      break;
    default:
      return NULL;
  };

  // Align to 8KiB
  freeMemAddr_ = (freeMemAddr_ + 0x00001fff) & (~0x00001fff);

  // Get addr
  addr = freeMemAddr_;

  // Add the bytes we just used
  freeMemAddr_ += iSize;

  return (void *)addr;
}

//---------------------------------------------------------------------------
void *
CPS2VideoDevice::allocTexture(int w, int h, uint16_t psm)
{
  uint32_t addr;

  // Width must be multiple of 64
  if((w & 0x3f) != 0)
    return NULL;

  // Limit the max size
  if((w > 1024) || (h > 1024))
    return NULL;

  uint32_t iSize = w * h;

  switch(psm)
  {
//    case GRAPH_PSM_4HL:
//    case GRAPH_PSM_4HH:
//    case GRAPH_PSM_8H:
    case GRAPH_PSM_4:
      iSize /= 2;
      break;
    case GRAPH_PSM_8:
      break;
    case GRAPH_PSM_16:
    case GRAPH_PSM_16S:
      iSize *= 2;
      break;
    case GRAPH_PSM_24:
    case GRAPH_PSM_32:
      iSize *= 4;
      break;
    default:
      return NULL;
  };

  // Align to 256Byte
  freeMemAddr_ = (freeMemAddr_ + 0x000000ff) & (~0x000000ff);

  // Get addr
  addr = freeMemAddr_;

  // Add the bytes we just used
  freeMemAddr_ += iSize;

  return (void *)addr;
}
