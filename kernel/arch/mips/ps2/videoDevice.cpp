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
SPS2VideoMode vmodes[] =
{
  // PAL (Why not 704x576 or 720x576 (PAL DVD))
  { 640,  256, 16, 0x03, GRAPH_PSM_16, NON_INTERLACED, 0, GS_SET_DISPLAY(652, 36, 3, 0, 2559,  255)},
  { 640,  256, 24, 0x03, GRAPH_PSM_24, NON_INTERLACED, 0, GS_SET_DISPLAY(652, 36, 3, 0, 2559,  255)},
  { 640,  256, 32, 0x03, GRAPH_PSM_32, NON_INTERLACED, 0, GS_SET_DISPLAY(652, 36, 3, 0, 2559,  255)},
  { 640,  512, 16, 0x03, GRAPH_PSM_16, INTERLACED, FIELD, GS_SET_DISPLAY(652, 72, 3, 0, 2559,  511)},
  { 640,  512, 24, 0x03, GRAPH_PSM_24, INTERLACED, FIELD, GS_SET_DISPLAY(652, 72, 3, 0, 2559,  511)},
  { 640,  512, 32, 0x03, GRAPH_PSM_32, INTERLACED, FIELD, GS_SET_DISPLAY(652, 72, 3, 0, 2559,  511)},
  // NTSC (Why not 640x480 (square) or 720x480 (NTSC DVD))
  { 640,  224, 16, 0x02, GRAPH_PSM_16, NON_INTERLACED, 0, GS_SET_DISPLAY(632, 25, 3, 0, 2559,  223)},
  { 640,  224, 24, 0x02, GRAPH_PSM_24, NON_INTERLACED, 0, GS_SET_DISPLAY(632, 25, 3, 0, 2559,  223)},
  { 640,  224, 32, 0x02, GRAPH_PSM_32, NON_INTERLACED, 0, GS_SET_DISPLAY(632, 25, 3, 0, 2559,  223)},
  { 640,  448, 16, 0x02, GRAPH_PSM_16, INTERLACED, FIELD, GS_SET_DISPLAY(632, 50, 3, 0, 2559,  447)},
  { 640,  448, 24, 0x02, GRAPH_PSM_24, INTERLACED, FIELD, GS_SET_DISPLAY(632, 50, 3, 0, 2559,  447)},
  { 640,  448, 32, 0x02, GRAPH_PSM_32, INTERLACED, FIELD, GS_SET_DISPLAY(632, 50, 3, 0, 2559,  447)},

  // EDTV
  { 720,  480, 16, 0x50, GRAPH_PSM_16, NON_INTERLACED, 0, GS_SET_DISPLAY(232, 35, 1, 0, 1439,  479)}, // 480p
  { 720,  480, 24, 0x50, GRAPH_PSM_24, NON_INTERLACED, 0, GS_SET_DISPLAY(232, 35, 1, 0, 1439,  479)}, // 480p
  { 720,  480, 32, 0x50, GRAPH_PSM_32, NON_INTERLACED, 0, GS_SET_DISPLAY(232, 35, 1, 0, 1439,  479)}, // 480p
  // HDTV
  {1280,  720, 16, 0x52, GRAPH_PSM_16, NON_INTERLACED, 0, GS_SET_DISPLAY(302, 24, 0, 0, 1279,  719)}, // 720p
  {1280,  720, 24, 0x52, GRAPH_PSM_24, NON_INTERLACED, 0, GS_SET_DISPLAY(302, 24, 0, 0, 1279,  719)}, // 720p
  {1280,  720, 32, 0x52, GRAPH_PSM_32, NON_INTERLACED, 0, GS_SET_DISPLAY(302, 24, 0, 0, 1279,  719)}, // 720p
  {1920,  540, 16, 0x51, GRAPH_PSM_16, NON_INTERLACED, 0, GS_SET_DISPLAY(238, 20, 0, 0, 1919,  539)},
  {1920,  540, 24, 0x51, GRAPH_PSM_24, NON_INTERLACED, 0, GS_SET_DISPLAY(238, 20, 0, 0, 1919,  539)},
  {1920,  540, 32, 0x51, GRAPH_PSM_32, NON_INTERLACED, 0, GS_SET_DISPLAY(238, 20, 0, 0, 1919,  539)},
  {1920, 1080, 16, 0x51, GRAPH_PSM_16, INTERLACED, FIELD, GS_SET_DISPLAY(238, 40, 0, 0, 1919, 1079)}, // 1080i

  // VGA
//  { 640,  480, 16, 0x1A, GRAPH_PSM_16, NON_INTERLACED, 0, GS_SET_DISPLAY(276, 34, 1, 0, 1279,  479)}, // 60Hz
//  { 640,  480, 24, 0x1A, GRAPH_PSM_24, NON_INTERLACED, 0, GS_SET_DISPLAY(276, 34, 1, 0, 1279,  479)}, // 60Hz
//  { 640,  480, 32, 0x1A, GRAPH_PSM_32, NON_INTERLACED, 0, GS_SET_DISPLAY(276, 34, 1, 0, 1279,  479)}, // 60Hz
//  { 640,  480, 16, 0x1B, GRAPH_PSM_16, NON_INTERLACED, 0, GS_SET_DISPLAY(276, 34, 1, 0, 1279,  479)}, // 72Hz
//  { 640,  480, 24, 0x1B, GRAPH_PSM_24, NON_INTERLACED, 0, GS_SET_DISPLAY(276, 34, 1, 0, 1279,  479)}, // 72Hz
//  { 640,  480, 32, 0x1B, GRAPH_PSM_32, NON_INTERLACED, 0, GS_SET_DISPLAY(276, 34, 1, 0, 1279,  479)}, // 72Hz
  { 640,  480, 16, 0x1C, GRAPH_PSM_16, NON_INTERLACED, 0, GS_SET_DISPLAY(276, 34, 1, 0, 1279,  479)}, // 75Hz
  { 640,  480, 24, 0x1C, GRAPH_PSM_24, NON_INTERLACED, 0, GS_SET_DISPLAY(276, 34, 1, 0, 1279,  479)}, // 75Hz
  { 640,  480, 32, 0x1C, GRAPH_PSM_32, NON_INTERLACED, 0, GS_SET_DISPLAY(276, 34, 1, 0, 1279,  479)}, // 75Hz
//  { 640,  480, 16, 0x1D, GRAPH_PSM_16, NON_INTERLACED, 0, GS_SET_DISPLAY(276, 34, 1, 0, 1279,  479)}, // 85Hz
//  { 640,  480, 24, 0x1D, GRAPH_PSM_24, NON_INTERLACED, 0, GS_SET_DISPLAY(276, 34, 1, 0, 1279,  479)}, // 85Hz
//  { 640,  480, 32, 0x1D, GRAPH_PSM_32, NON_INTERLACED, 0, GS_SET_DISPLAY(276, 34, 1, 0, 1279,  479)}, // 85Hz
  // SVGA
//  { 800,  600, 16, 0x2A, GRAPH_PSM_16, NON_INTERLACED, 0, GS_SET_DISPLAY(420, 26, 1, 0, 1599,  599)}, // 56Hz
//  { 800,  600, 24, 0x2A, GRAPH_PSM_24, NON_INTERLACED, 0, GS_SET_DISPLAY(420, 26, 1, 0, 1599,  599)}, // 56Hz
//  { 800,  600, 32, 0x2A, GRAPH_PSM_32, NON_INTERLACED, 0, GS_SET_DISPLAY(420, 26, 1, 0, 1599,  599)}, // 56Hz
//  { 800,  600, 16, 0x2B, GRAPH_PSM_16, NON_INTERLACED, 0, GS_SET_DISPLAY(420, 26, 1, 0, 1599,  599)}, // 60Hz
//  { 800,  600, 24, 0x2B, GRAPH_PSM_24, NON_INTERLACED, 0, GS_SET_DISPLAY(420, 26, 1, 0, 1599,  599)}, // 60Hz
//  { 800,  600, 32, 0x2B, GRAPH_PSM_32, NON_INTERLACED, 0, GS_SET_DISPLAY(420, 26, 1, 0, 1599,  599)}, // 60Hz
//  { 800,  600, 16, 0x2C, GRAPH_PSM_16, NON_INTERLACED, 0, GS_SET_DISPLAY(420, 26, 1, 0, 1599,  599)}, // 72Hz
//  { 800,  600, 24, 0x2C, GRAPH_PSM_24, NON_INTERLACED, 0, GS_SET_DISPLAY(420, 26, 1, 0, 1599,  599)}, // 72Hz
//  { 800,  600, 32, 0x2C, GRAPH_PSM_32, NON_INTERLACED, 0, GS_SET_DISPLAY(420, 26, 1, 0, 1599,  599)}, // 72Hz
  { 800,  600, 16, 0x2D, GRAPH_PSM_16, NON_INTERLACED, 0, GS_SET_DISPLAY(420, 26, 1, 0, 1599,  599)}, // 75Hz
  { 800,  600, 24, 0x2D, GRAPH_PSM_24, NON_INTERLACED, 0, GS_SET_DISPLAY(420, 26, 1, 0, 1599,  599)}, // 75Hz
  { 800,  600, 32, 0x2D, GRAPH_PSM_32, NON_INTERLACED, 0, GS_SET_DISPLAY(420, 26, 1, 0, 1599,  599)}, // 75Hz
//  { 800,  600, 16, 0x2E, GRAPH_PSM_16, NON_INTERLACED, 0, GS_SET_DISPLAY(420, 26, 1, 0, 1599,  599)}, // 85Hz
//  { 800,  600, 24, 0x2E, GRAPH_PSM_24, NON_INTERLACED, 0, GS_SET_DISPLAY(420, 26, 1, 0, 1599,  599)}, // 85Hz
//  { 800,  600, 32, 0x2E, GRAPH_PSM_32, NON_INTERLACED, 0, GS_SET_DISPLAY(420, 26, 1, 0, 1599,  599)}, // 85Hz
  // XGA
//  {1024,  768, 16, 0x3B, GRAPH_PSM_16, NON_INTERLACED, 0, GS_SET_DISPLAY(580, 34, 1, 0, 2047,  767)}, // 60Hz
//  {1024,  768, 24, 0x3B, GRAPH_PSM_24, NON_INTERLACED, 0, GS_SET_DISPLAY(580, 34, 1, 0, 2047,  767)}, // 60Hz
//  {1024,  768, 32, 0x3B, GRAPH_PSM_32, NON_INTERLACED, 0, GS_SET_DISPLAY(580, 34, 1, 0, 2047,  767)}, // 60Hz
//  {1024,  768, 16, 0x3C, GRAPH_PSM_16, NON_INTERLACED, 0, GS_SET_DISPLAY(580, 34, 1, 0, 2047,  767)}, // 70Hz
//  {1024,  768, 24, 0x3C, GRAPH_PSM_24, NON_INTERLACED, 0, GS_SET_DISPLAY(580, 34, 1, 0, 2047,  767)}, // 70Hz
//  {1024,  768, 32, 0x3C, GRAPH_PSM_32, NON_INTERLACED, 0, GS_SET_DISPLAY(580, 34, 1, 0, 2047,  767)}, // 70Hz
  {1024,  768, 16, 0x3D, GRAPH_PSM_16, NON_INTERLACED, 0, GS_SET_DISPLAY(580, 34, 1, 0, 2047,  767)}, // 75Hz
  {1024,  768, 24, 0x3D, GRAPH_PSM_24, NON_INTERLACED, 0, GS_SET_DISPLAY(580, 34, 1, 0, 2047,  767)}, // 75Hz
  {1024,  768, 32, 0x3D, GRAPH_PSM_32, NON_INTERLACED, 0, GS_SET_DISPLAY(580, 34, 1, 0, 2047,  767)}, // 75Hz
//  {1024,  768, 16, 0x3E, GRAPH_PSM_16, NON_INTERLACED, 0, GS_SET_DISPLAY(580, 34, 1, 0, 2047,  767)}, // 85Hz
//  {1024,  768, 24, 0x3E, GRAPH_PSM_24, NON_INTERLACED, 0, GS_SET_DISPLAY(580, 34, 1, 0, 2047,  767)}, // 85Hz
//  {1024,  768, 32, 0x3E, GRAPH_PSM_32, NON_INTERLACED, 0, GS_SET_DISPLAY(580, 34, 1, 0, 2047,  767)}, // 85Hz
  // SXGA
//  {1280, 1024, 16, 0x4A, GRAPH_PSM_16, NON_INTERLACED, 0, GS_SET_DISPLAY(348, 40, 0, 0, 1279, 1023)}, // 60Hz
  {1280, 1024, 16, 0x4B, GRAPH_PSM_16, NON_INTERLACED, 0, GS_SET_DISPLAY(348, 40, 0, 0, 1279, 1023)}, // 75Hz
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

DECLARE_GS_PACKET(Gs2DCmdBuffer,1000);
DECLARE_GS_PACKET(GsVideoCmdBuffer,50);


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
CPS22DRenderer::CPS22DRenderer(CSurface * surface)
 : pSurface_(NULL)
 , bDataWaiting_(false)
{
  INIT_GS_PACKET(Gs2DCmdBuffer,1000);

  // Create new packet
  BEGIN_GS_PACKET(Gs2DCmdBuffer);
  GIF_TAG_AD(Gs2DCmdBuffer, 1, 0, 0, 0);

  setSurface(surface);
}

//---------------------------------------------------------------------------
CPS22DRenderer::~CPS22DRenderer()
{
}

//---------------------------------------------------------------------------
void
CPS22DRenderer::setSurface(CSurface * surface)
{
  pSurface_ = (CPS2Surface *)surface;

  if(pSurface_ != NULL)
  {
    // Set destination
    GIF_DATA_AD(Gs2DCmdBuffer, frame_1, GS_FRAME((uint32_t)pSurface_->p >> 13, pSurface_->mode.width >> 6, pSurface_->psm_, 0));
    // Set clipping
    GIF_DATA_AD(Gs2DCmdBuffer, scissor_1, GS_SCISSOR(0, pSurface_->mode.width, 0, pSurface_->mode.height));
  }
}

//---------------------------------------------------------------------------
CSurface *
CPS22DRenderer::getSurface()
{
  return pSurface_;
}

//---------------------------------------------------------------------------
void
CPS22DRenderer::flush()
{
  if(bDataWaiting_ == true)
  {
    // Send packet to GS
    SEND_GS_PACKET(Gs2DCmdBuffer);
    bDataWaiting_ = false;

    // Create new packet
    BEGIN_GS_PACKET(Gs2DCmdBuffer);
    GIF_TAG_AD(Gs2DCmdBuffer, 1, 0, 0, 0);

    if(pSurface_ != 0)
    {
      // Set destination
      GIF_DATA_AD(Gs2DCmdBuffer, frame_1, GS_FRAME((uint32_t)pSurface_->p >> 13, pSurface_->mode.width >> 6, pSurface_->psm_, 0));
      // Set clipping
      GIF_DATA_AD(Gs2DCmdBuffer, scissor_1, GS_SCISSOR(0, pSurface_->mode.width, 0, pSurface_->mode.height));
    }
  }
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

  GIF_DATA_AD(Gs2DCmdBuffer, prim, GS_PRIM(PRIM_POINT, 0, 0, 0, 0, 0, 0, 0, 0));
  GIF_DATA_AD(Gs2DCmdBuffer, rgbaq, GS_RGBAQ(color_.r, color_.g, color_.b, 0x80, 0));
  GIF_DATA_AD(Gs2DCmdBuffer, xyz2, GS_XYZ2(x<<4, y<<4, 0));
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

  GIF_DATA_AD(Gs2DCmdBuffer, prim, GS_PRIM(PRIM_SPRITE, 0, 0, 0, 0, 0, 0, 0, 0));
  GIF_DATA_AD(Gs2DCmdBuffer, rgbaq, GS_RGBAQ(color_.r, color_.g, color_.b, 0x80, 0));
  GIF_DATA_AD(Gs2DCmdBuffer, xyz2, GS_XYZ2(x<<4, y<<4, 0));
  GIF_DATA_AD(Gs2DCmdBuffer, xyz2, GS_XYZ2((x+width)<<4, (y+height)<<4, 0));
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

  GIF_DATA_AD(Gs2DCmdBuffer, prim, GS_PRIM(PRIM_LINE, 0, 0, 0, 0, 0, 0, 0, 0));
  GIF_DATA_AD(Gs2DCmdBuffer, rgbaq, GS_RGBAQ(color_.r, color_.g, color_.b, 0x80, 0));
  GIF_DATA_AD(Gs2DCmdBuffer, xyz2, GS_XYZ2(x1<<4, y1<<4, 0));
  GIF_DATA_AD(Gs2DCmdBuffer, xyz2, GS_XYZ2(x2<<4, y2<<4, 0));
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

  GIF_DATA_AD(Gs2DCmdBuffer, prim, GS_PRIM(PRIM_LINE_STRIP, 0, 0, 0, 0, 0, 0, 0, 0));
  GIF_DATA_AD(Gs2DCmdBuffer, rgbaq, GS_RGBAQ(color_.r, color_.g, color_.b, 0x80, 0));
  GIF_DATA_AD(Gs2DCmdBuffer, xyz2, GS_XYZ2(x<<4, y<<4, 0));
  GIF_DATA_AD(Gs2DCmdBuffer, xyz2, GS_XYZ2(x+width<<4, y<<4, 0));
  GIF_DATA_AD(Gs2DCmdBuffer, xyz2, GS_XYZ2(x+width<<4, y+height<<4, 0));
  GIF_DATA_AD(Gs2DCmdBuffer, xyz2, GS_XYZ2(x<<4, y+height<<4, 0));
  GIF_DATA_AD(Gs2DCmdBuffer, xyz2, GS_XYZ2(x<<4, y<<4, 0));
  bDataWaiting_ = true;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CPS2VideoDevice::CPS2VideoDevice()
 : CAVideoDevice()
 , pSurface_(NULL)
 , pCurrentMode_(NULL)
 , pCurrentPS2Mode_(NULL)
{
  INIT_GS_PACKET(GsVideoCmdBuffer,50);
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

  // - Initialize the DMA.
  // - Writes a 0 to most of the DMA registers.
  dma_reset();

  // - Sets the RESET bit if the GS CSR register.
  GS_RESET();

  // - Can someone please tell me what the sync.p
  // instruction does. Synchronizes something :-)
  __asm__("sync.p\nnop");

  // - Sets up the GS IMR register (i guess).
  // - The IMR register is used to mask and unmask certain interrupts,
  //   for example VSync and HSync. We'll use this properly in Tutorial 2.
  // - Does anyone have code to do this without using the 0x71 syscall?
  // - I havn't gotten around to looking at any PS2 bios code yet.
  setGsIMR(0);

  // - Use syscall 0x02 to setup some video mode stuff.
  // - Pretty self explanatory I think.
  // - Does anyone have code to do this without using the syscall? It looks
  //   like it should only set the SMODE2 register, but if I remove this syscall
  //   and set the SMODE2 register myself, it donesn't work. What else does
  //   syscall 0x02 do?
  setGsCrt(pCurrentPS2Mode_->interlace, pCurrentPS2Mode_->mode, pCurrentPS2Mode_->field);

  // - I havn't attempted to understand what the Alpha parameters can do. They
  //   have been blindly copied from the 3stars demo (although they don't seem
  //   do have any impact in this simple 2D code.
  GS_SET_PMODE(
      1,        // ReadCircuit1 ON
      0,        // ReadCircuit2 OFF
      1,        // Use ALP register for Alpha Blending
      1,        // Alpha Value of ReadCircuit2 for output selection
      0,        // Blend Alpha with the output of ReadCircuit2
      0xFF  // Alpha Value = 1.0
  );

  REG_GS_DISPLAY1 = pCurrentPS2Mode_->display;
  //REG_GS_DISPLAY2 = pCurrentPS2Mode_->display;

  BEGIN_GS_PACKET(GsVideoCmdBuffer);
  GIF_TAG_AD(GsVideoCmdBuffer, 1, 0, 0, 0);

  // Use drawing parameters from PRIM register
  GIF_DATA_AD(GsVideoCmdBuffer, prmodecont, 1);
  // Setup frame buffers. Point to 0 initially.
  GIF_DATA_AD(GsVideoCmdBuffer, frame_1, GS_FRAME(0, pCurrentPS2Mode_->width >> 6, pCurrentPS2Mode_->psm, 0));
  // Displacement between Primitive and Window coordinate systems.
  GIF_DATA_AD(GsVideoCmdBuffer, xyoffset_1, GS_XYOFFSET(GS_X_BASE<<4, GS_Y_BASE<<4));
  // Clip to frame buffer.
  GIF_DATA_AD(GsVideoCmdBuffer, scissor_1, GS_SCISSOR(0, pCurrentPS2Mode_->width, 0, pCurrentPS2Mode_->height));

  SEND_GS_PACKET(GsVideoCmdBuffer);
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
    REG_GS_DISPFB1 = GS_SET_DISPFB((uint32_t)pSurface_->p >> 13, pSurface_->mode.width >> 6, pSurface_->psm_, 0, 0);
  }
}

//---------------------------------------------------------------------------
void
CPS2VideoDevice::bitBlt(CSurface * dest, int dx, int dy, int w, int h, CSurface * source, int sx, int sy)
{
  CPS2Surface * pDest   = (CPS2Surface *)dest;
  CPS2Surface * pSource = (CPS2Surface *)source;

  BEGIN_GS_PACKET(GsVideoCmdBuffer);
  GIF_TAG_AD(GsVideoCmdBuffer, 1, 0, 0, 0);

  GIF_DATA_AD(GsVideoCmdBuffer, bitbltbuf,
    GS_BITBLTBUF(
      ((uint32_t)pSource->p)>>8,      // source address
      pSource->mode.width>>6,         // source buffer width
      pSource->psm_,
      ((uint32_t)pDest->p)>>8,        // dest address
      pDest->mode.width>>6,           // dest buffer width
      pDest->psm_));
  GIF_DATA_AD(GsVideoCmdBuffer, trxpos,
    GS_TRXPOS(
      sx,
      sy,
      dx,
      dy,
      0));                      // left to right/top to bottom
  GIF_DATA_AD(GsVideoCmdBuffer, trxreg,
    GS_TRXREG(
      w,
      h));
  GIF_DATA_AD(GsVideoCmdBuffer, trxdir, GS_TRXDIR(XDIR_GS_GS));

  SEND_GS_PACKET(GsVideoCmdBuffer);
}
