#include "videoDevice.h"
#include "glesContext.h"
#include "bios.h"
#include "dma.h"
#include "gs.h"
#include "string.h"


//---------------------------------------------------------------------------
#define DRAW_FIELD_EVEN()    (GS_REG_CSR & (1 << 13))
#define DISPLAY_FIELD_EVEN() (!DRAW_FIELD_EVEN)
#define WAIT_VSYNC()         {REG_GS_CSR = REG_GS_CSR & 8; while(!(REG_GS_CSR & 8));}
#define IS_PAL()             (*((char *)0x1fc80000 - 0xae) == 'E')
#define IS_NTSC()            (!IS_PAL())

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
  {0x3b, 2}, //  XGA, 60Hz 11
  {0x3c, 2}, //  XGA, 72Hz 12
  {0x3d, 2}, //  XGA, 75Hz 13
  {0x3e, 2}, //  XGA, 85Hz 14
  {0x4a, 1}, // SXGA, 60Hz 15
  {0x4b, 1}, // SXGA, 75Hz 16

  {0x50, 2}, //       480p 17
  {0x51, 1}, //      1080i 18
  {0x52, 1}, //       720p 19
};

//---------------------------------------------------------------------------
SPS2VideoMode vmodes[] =
{
  // PAL (Why not 704x576 or 720x576 (PAL DVD))
  { 640,  256, 16, GRAPH_PSM_16, NON_INTERLACED, 0, &cmodes[ 1], 163, 36},
  { 640,  256, 24, GRAPH_PSM_24, NON_INTERLACED, 0, &cmodes[ 1], 163, 36},
  { 640,  256, 32, GRAPH_PSM_32, NON_INTERLACED, 0, &cmodes[ 1], 163, 36},
  { 640,  512, 16, GRAPH_PSM_16, INTERLACED, FIELD, &cmodes[ 1], 163, 72},
  { 640,  512, 24, GRAPH_PSM_24, INTERLACED, FIELD, &cmodes[ 1], 163, 72},
  { 640,  512, 32, GRAPH_PSM_32, INTERLACED, FIELD, &cmodes[ 1], 163, 72},
  // NTSC (Why not 640x480 (square) or 720x480 (NTSC DVD))
  { 640,  224, 16, GRAPH_PSM_16, NON_INTERLACED, 0, &cmodes[ 0], 158, 25},
  { 640,  224, 24, GRAPH_PSM_24, NON_INTERLACED, 0, &cmodes[ 0], 158, 25},
  { 640,  224, 32, GRAPH_PSM_32, NON_INTERLACED, 0, &cmodes[ 0], 158, 25},
  { 640,  448, 16, GRAPH_PSM_16, INTERLACED, FIELD, &cmodes[ 0], 158, 50},
  { 640,  448, 24, GRAPH_PSM_24, INTERLACED, FIELD, &cmodes[ 0], 158, 50},
  { 640,  448, 32, GRAPH_PSM_32, INTERLACED, FIELD, &cmodes[ 0], 158, 50},
  // VGA
//  { 640,  480, 16, GRAPH_PSM_16, NON_INTERLACED, 0, &cmodes[ 2], 138, 34}, // 60Hz
//  { 640,  480, 24, GRAPH_PSM_24, NON_INTERLACED, 0, &cmodes[ 2], 138, 34}, // 60Hz
//  { 640,  480, 32, GRAPH_PSM_32, NON_INTERLACED, 0, &cmodes[ 2], 138, 34}, // 60Hz
//  { 640,  480, 16, GRAPH_PSM_16, NON_INTERLACED, 0, &cmodes[ 3], 138, 34}, // 72Hz
//  { 640,  480, 24, GRAPH_PSM_24, NON_INTERLACED, 0, &cmodes[ 3], 138, 34}, // 72Hz
//  { 640,  480, 32, GRAPH_PSM_32, NON_INTERLACED, 0, &cmodes[ 3], 138, 34}, // 72Hz
  { 640,  480, 16, GRAPH_PSM_16, NON_INTERLACED, 0, &cmodes[ 4], 138, 34}, // 75Hz
  { 640,  480, 24, GRAPH_PSM_24, NON_INTERLACED, 0, &cmodes[ 4], 138, 34}, // 75Hz
  { 640,  480, 32, GRAPH_PSM_32, NON_INTERLACED, 0, &cmodes[ 4], 138, 34}, // 75Hz
//  { 640,  480, 16, GRAPH_PSM_16, NON_INTERLACED, 0, &cmodes[ 5], 138, 34}, // 85Hz
//  { 640,  480, 24, GRAPH_PSM_24, NON_INTERLACED, 0, &cmodes[ 5], 138, 34}, // 85Hz
//  { 640,  480, 32, GRAPH_PSM_32, NON_INTERLACED, 0, &cmodes[ 5], 138, 34}, // 85Hz
  // SVGA
//  { 800,  600, 16, GRAPH_PSM_16, NON_INTERLACED, 0, &cmodes[ 6], 210, 26}, // 56Hz
//  { 800,  600, 24, GRAPH_PSM_24, NON_INTERLACED, 0, &cmodes[ 6], 210, 26}, // 56Hz
//  { 800,  600, 32, GRAPH_PSM_32, NON_INTERLACED, 0, &cmodes[ 6], 210, 26}, // 56Hz
//  { 800,  600, 16, GRAPH_PSM_16, NON_INTERLACED, 0, &cmodes[ 7], 210, 26}, // 60Hz
//  { 800,  600, 24, GRAPH_PSM_24, NON_INTERLACED, 0, &cmodes[ 7], 210, 26}, // 60Hz
//  { 800,  600, 32, GRAPH_PSM_32, NON_INTERLACED, 0, &cmodes[ 7], 210, 26}, // 60Hz
//  { 800,  600, 16, GRAPH_PSM_16, NON_INTERLACED, 0, &cmodes[ 8], 210, 26}, // 72Hz
//  { 800,  600, 24, GRAPH_PSM_24, NON_INTERLACED, 0, &cmodes[ 8], 210, 26}, // 72Hz
//  { 800,  600, 32, GRAPH_PSM_32, NON_INTERLACED, 0, &cmodes[ 8], 210, 26}, // 72Hz
  { 800,  600, 16, GRAPH_PSM_16, NON_INTERLACED, 0, &cmodes[ 9], 210, 26}, // 75Hz
  { 800,  600, 24, GRAPH_PSM_24, NON_INTERLACED, 0, &cmodes[ 9], 210, 26}, // 75Hz
  { 800,  600, 32, GRAPH_PSM_32, NON_INTERLACED, 0, &cmodes[ 9], 210, 26}, // 75Hz
//  { 800,  600, 16, GRAPH_PSM_16, NON_INTERLACED, 0, &cmodes[10], 210, 26}, // 85Hz
//  { 800,  600, 24, GRAPH_PSM_24, NON_INTERLACED, 0, &cmodes[10], 210, 26}, // 85Hz
//  { 800,  600, 32, GRAPH_PSM_32, NON_INTERLACED, 0, &cmodes[10], 210, 26}, // 85Hz
  // XGA
//  {1024,  768, 16, GRAPH_PSM_16, NON_INTERLACED, 0, &cmodes[11], 270, 34}, // 60Hz
//  {1024,  768, 24, GRAPH_PSM_24, NON_INTERLACED, 0, &cmodes[11], 270, 34}, // 60Hz
//  {1024,  768, 32, GRAPH_PSM_32, NON_INTERLACED, 0, &cmodes[11], 270, 34}, // 60Hz
//  {1024,  768, 16, GRAPH_PSM_16, NON_INTERLACED, 0, &cmodes[12], 270, 34}, // 70Hz
//  {1024,  768, 24, GRAPH_PSM_24, NON_INTERLACED, 0, &cmodes[12], 270, 34}, // 70Hz
//  {1024,  768, 32, GRAPH_PSM_32, NON_INTERLACED, 0, &cmodes[12], 270, 34}, // 70Hz
  {1024,  768, 16, GRAPH_PSM_16, NON_INTERLACED, 0, &cmodes[13], 270, 34}, // 75Hz
  {1024,  768, 24, GRAPH_PSM_24, NON_INTERLACED, 0, &cmodes[13], 270, 34}, // 75Hz
  {1024,  768, 32, GRAPH_PSM_32, NON_INTERLACED, 0, &cmodes[13], 270, 34}, // 75Hz
//  {1024,  768, 16, GRAPH_PSM_16, NON_INTERLACED, 0, &cmodes[14], 270, 34}, // 85Hz
//  {1024,  768, 24, GRAPH_PSM_24, NON_INTERLACED, 0, &cmodes[14], 270, 34}, // 85Hz
//  {1024,  768, 32, GRAPH_PSM_32, NON_INTERLACED, 0, &cmodes[14], 270, 34}, // 85Hz
  // SXGA
//  {1280, 1024, 16, GRAPH_PSM_16, NON_INTERLACED, 0, &cmodes[15], 348, 40}, // 60Hz
  {1280, 1024, 16, GRAPH_PSM_16, NON_INTERLACED, 0, &cmodes[16], 348, 40}, // 75Hz
  // EDTV
  { 720,  480, 16, GRAPH_PSM_16, NON_INTERLACED, 0, &cmodes[17], 116, 35}, // 480p
  { 720,  480, 24, GRAPH_PSM_24, NON_INTERLACED, 0, &cmodes[17], 116, 35}, // 480p
  { 720,  480, 32, GRAPH_PSM_32, NON_INTERLACED, 0, &cmodes[17], 116, 35}, // 480p
  // HDTV
  {1920,  540, 16, GRAPH_PSM_16, NON_INTERLACED, 0, &cmodes[18], 238, 20}, // 1080i / 2
  {1920,  540, 24, GRAPH_PSM_24, NON_INTERLACED, 0, &cmodes[18], 238, 20}, // 1080i / 2
  {1920,  540, 32, GRAPH_PSM_32, NON_INTERLACED, 0, &cmodes[18], 238, 20}, // 1080i / 2
  {1920, 1080, 16, GRAPH_PSM_16, INTERLACED, FIELD, &cmodes[18], 238, 40}, // 1080i
  {1280,  720, 16, GRAPH_PSM_16, NON_INTERLACED, 0, &cmodes[19], 302, 24}, // 720p
  {1280,  720, 24, GRAPH_PSM_24, NON_INTERLACED, 0, &cmodes[19], 302, 24}, // 720p
  {1280,  720, 32, GRAPH_PSM_32, NON_INTERLACED, 0, &cmodes[19], 302, 24}, // 720p
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
  { 640,  256,  640,  256, 16, cfA1R5G5B5}, //  320KiB  0
  { 640,  256,  640,  256, 24, cfB8G8R8},   //  480KiB
  { 640,  256,  640,  256, 32, cfA8B8G8R8}, //  640KiB
  { 640,  512,  640,  512, 16, cfA1R5G5B5}, //  640KiB
  { 640,  512,  640,  512, 24, cfB8G8R8},   //  960KiB
  { 640,  512,  640,  512, 32, cfA8B8G8R8}, // 1280KiB  5
  // NTSC
  { 640,  224,  640,  224, 16, cfA1R5G5B5}, //  280KiB  6
  { 640,  224,  640,  224, 24, cfB8G8R8},   //  420KiB
  { 640,  224,  640,  224, 32, cfA8B8G8R8}, //  560KiB
  { 640,  448,  640,  448, 16, cfA1R5G5B5}, //  560KiB
  { 640,  448,  640,  448, 24, cfB8G8R8},   //  840KiB
  { 640,  448,  640,  448, 32, cfA8B8G8R8}, // 1120KiB 11

  // EDTV
  { 720,  480,  720,  480, 16, cfA1R5G5B5}, //  675KiB 12
  { 720,  480,  720,  480, 24, cfB8G8R8},   // 1012KiB
  { 720,  480,  720,  480, 32, cfA8B8G8R8}, // 1350KiB 13
  // HDTV
  {1280,  720, 1280,  720, 16, cfA1R5G5B5}, // 1800KiB 15
  {1280,  720, 1280,  720, 24, cfB8G8R8},   // 2700KiB
  {1280,  720, 1280,  720, 32, cfA8B8G8R8}, // 3600KiB
  {1920,  540, 1920,  540, 16, cfA1R5G5B5}, // 2025KiB
  {1920,  540, 1920,  540, 24, cfB8G8R8},   // 3038KiB
  {1920,  540, 1920,  540, 32, cfA8B8G8R8}, // 4050KiB
  {1920, 1080, 1920, 1080, 16, cfA1R5G5B5}, // 4050KiB 21

  // VGA
  { 640,  480,  640,  480, 16, cfA1R5G5B5}, //  600KiB 22
  { 640,  480,  640,  480, 24, cfB8G8R8},   //  900KiB
  { 640,  480,  640,  480, 32, cfA8B8G8R8}, // 1200KiB 24
  // SVGA
  { 800,  600,  800,  600, 16, cfA1R5G5B5}, //  938KiB 25
  { 800,  600,  800,  600, 24, cfB8G8R8},   // 1406KiB
  { 800,  600,  800,  600, 32, cfA8B8G8R8}, // 1875KiB 27
  // XGA
  {1024,  768, 1024,  768, 16, cfA1R5G5B5}, // 1536KiB 28
  {1024,  768, 1024,  768, 24, cfB8G8R8},   // 2304KiB
  {1024,  768, 1024,  768, 32, cfA8B8G8R8}, // 3072KiB 30
  // SXGA
  {1280, 1024, 1280, 1024, 16, cfA1R5G5B5}, // 2560KiB 31
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
 , packet_(1000)
 , bDataWaiting_(false)
{
  // Reset the packet
  packet_.reset();
  // Set tag
  packet_.tagAd(1, 0, 0, 0);
  // Preserve this data after sending
  packet_.headerSize(1);

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
    // Reset the packet
    packet_.reset();
    // Set tag
    packet_.tagAd(1, 0, 0, 0);
    // Set destination
    packet_.data(frame_1, GS_FRAME((uint32_t)pSurface_->p >> 13, pSurface_->mode.width >> 6, pSurface_->psm_, 0));
    // Set clipping
    packet_.data(scissor_1, GS_SCISSOR(0, pSurface_->mode.width, 0, pSurface_->mode.height));
    // Preserve this data after sending
    packet_.headerSize(3);
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
    // Send packet to GS
    packet_.send();
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

  packet_.data(prim, GS_PRIM(PRIM_POINT, 0, 0, 0, 0, 0, 0, 0, 0));
  packet_.data(rgbaq, GS_RGBAQ(color_.r, color_.g, color_.b, 0x80, 0));
  packet_.data(xyz2, GS_XYZ2(x<<4, y<<4, 0));
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

  packet_.data(prim, GS_PRIM(PRIM_SPRITE, 0, 0, 0, 0, 0, 0, 0, 0));
  packet_.data(rgbaq, GS_RGBAQ(color_.r, color_.g, color_.b, 0x80, 0));
  packet_.data(xyz2, GS_XYZ2(x<<4, y<<4, 0));
  packet_.data(xyz2, GS_XYZ2((x+width)<<4, (y+height)<<4, 0));
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

  packet_.data(prim, GS_PRIM(PRIM_LINE, 0, 0, 0, 0, 0, 0, 0, 0));
  packet_.data(rgbaq, GS_RGBAQ(color_.r, color_.g, color_.b, 0x80, 0));
  packet_.data(xyz2, GS_XYZ2(x1<<4, y1<<4, 0));
  packet_.data(xyz2, GS_XYZ2(x2<<4, y2<<4, 0));
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

  packet_.data(prim, GS_PRIM(PRIM_LINE_STRIP, 0, 0, 0, 0, 0, 0, 0, 0));
  packet_.data(rgbaq, GS_RGBAQ(color_.r, color_.g, color_.b, 0x80, 0));
  packet_.data(xyz2, GS_XYZ2(x<<4, y<<4, 0));
  packet_.data(xyz2, GS_XYZ2(x+width<<4, y<<4, 0));
  packet_.data(xyz2, GS_XYZ2(x+width<<4, y+height<<4, 0));
  packet_.data(xyz2, GS_XYZ2(x<<4, y+height<<4, 0));
  packet_.data(xyz2, GS_XYZ2(x<<4, y<<4, 0));
  bDataWaiting_ = true;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CPS2VideoDevice::CPS2VideoDevice()
 : CAVideoDevice()
 , pSurface_(NULL)
 , packet_(50)
 , pCurrentMode_(NULL)
 , pCurrentPS2Mode_(NULL)
{
  // Reset the packet
  packet_.reset();
  // Set tag
  packet_.tagAd(1, 0, 0, 0);
  // Preserve this data after sending
  packet_.headerSize(1);
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
  pCurrentMode_ = mode;

  for(uint32_t i(0); i < vmode_count; i++)
  {
    if((pCurrentMode_->width  == vmodes[i].width) &&
       (pCurrentMode_->height == vmodes[i].height) &&
       (pCurrentMode_->bpp    == vmodes[i].bpp))
    {
      pCurrentPS2Mode_ = &vmodes[i];
      break;
    }
  }

  if(pCurrentPS2Mode_ == NULL)
    return;

  // Reset the GS
  REG_GS_CSR = GS_CSR_RESET();

  // Sync
  __asm__("sync.p\nnop");

  // Clear GS interrupt mask register
  bios::GsPutIMR(0);

  // Setup CRTC for video mode
  bios::SetGsCrt(pCurrentPS2Mode_->interlace, pCurrentPS2Mode_->crtcMode->biosMode, pCurrentPS2Mode_->field);

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
                                      pCurrentPS2Mode_->yoffset,
                                      pCurrentPS2Mode_->width,
                                      pCurrentPS2Mode_->height);

  // Setup read circuit 2
  //REG_GS_DISPLAY2 = ...;

  // Use drawing parameters from PRIM register
  packet_.data(prmodecont, 1);
  // Setup frame buffers. Point to 0 initially.
  packet_.data(frame_1, GS_FRAME(0, pCurrentPS2Mode_->width >> 6, pCurrentPS2Mode_->psm, 0));
  // Displacement between Primitive and Window coordinate systems.
  packet_.data(xyoffset_1, GS_XYOFFSET(GS_X_BASE<<4, GS_Y_BASE<<4));
  // Clip to frame buffer.
  packet_.data(scissor_1, GS_SCISSOR(0, pCurrentPS2Mode_->width, 0, pCurrentPS2Mode_->height));
  packet_.send();
}

//---------------------------------------------------------------------------
static uint32_t bytesUsed(0);
void
CPS2VideoDevice::getSurface(CSurface ** surface, int width, int height)
{
  CPS2Surface * pSurface = 0;

  pSurface = new CPS2Surface;

  pSurface->mode        = *pCurrentMode_;
  pSurface->mode.width  = width;
  pSurface->mode.height = height;
  pSurface->psm_        = pCurrentPS2Mode_->psm;
  pSurface->p           = (void *)bytesUsed;

  // Add the bytes we just used
  bytesUsed += width * height * (pCurrentMode_->bpp / 8);

  // Alignment:
  //  -   8KiB  for system buffers
  //  - 256Byte for textures
  bytesUsed  = (bytesUsed + 0x1fff) & (~0x1fff); // Align to 8KiB

  *surface = pSurface;
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
  *renderer = new CPS2GLESContext;
}

//---------------------------------------------------------------------------
void
CPS2VideoDevice::waitVSync()
{
  WAIT_VSYNC();
}

//---------------------------------------------------------------------------
void
CPS2VideoDevice::displaySurface(CSurface * surface)
{
  CPS2Surface * pNewSurface = (CPS2Surface *)surface;

  // Always VSync, even if the frame is not new.
  if(vSync_ == true)
    WAIT_VSYNC();

  // Set new surface
  if(pNewSurface != NULL)
  {
    pSurface_ = pNewSurface;

    // Set visible frame
    REG_GS_DISPFB1 = GS_DISPFB((uint32_t)pSurface_->p >> 13, pSurface_->mode.width >> 6, pSurface_->psm_, 0, 0);
  }
}

//---------------------------------------------------------------------------
void
CPS2VideoDevice::bitBlt(CSurface * dest, int dx, int dy, int w, int h, CSurface * source, int sx, int sy)
{
  CPS2Surface * pDest   = (CPS2Surface *)dest;
  CPS2Surface * pSource = (CPS2Surface *)source;

  packet_.data(bitbltbuf, GS_BITBLTBUF(
                            ((uint32_t)pSource->p)>>8,
                            pSource->mode.width>>6,
                            pSource->psm_,
                            ((uint32_t)pDest->p)>>8,
                            pDest->mode.width>>6,
                            pDest->psm_));
  packet_.data(trxpos,    GS_TRXPOS(sx, sy, dx, dy, 0));
  packet_.data(trxreg,    GS_TRXREG(w, h));
  packet_.data(trxdir,    GS_TRXDIR(XDIR_GS_GS));
  packet_.send();
}
