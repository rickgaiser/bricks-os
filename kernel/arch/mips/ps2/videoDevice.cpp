#include "videoDevice.h"
#include "bios.h"
#include "dma.h"
#include "gs.h"
#include "string.h"


//---------------------------------------------------------------------------
#define IS_NTSC() \
(*((char *)0x1FC80000 - 0xAE) != 'E')
#define IS_PAL() \
(*((char *)0x1FC80000 - 0xAE) == 'E')

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
static const SVideoMode videoModes[] =
{
  // PAL
//  { 640,  256,  640,  256, 16, cfA1R5G5B5}, //  320KiB
//  { 640,  256,  640,  256, 24, cfB8G8R8},   //  480KiB
  { 640,  256,  640,  256, 32, cfA8B8G8R8}, //  640KiB
//  { 640,  512,  640,  512, 16, cfA1R5G5B5}, //  640KiB
//  { 640,  512,  640,  512, 24, cfB8G8R8},   //  960KiB
//  { 640,  512,  640,  512, 32, cfA8B8G8R8}, // 1280KiB
  // NTSC
//  { 640,  224,  640,  224, 16, cfA1R5G5B5}, //  280KiB
//  { 640,  224,  640,  224, 24, cfB8G8R8},   //  420KiB
  { 640,  224,  640,  224, 32, cfA8B8G8R8}, //  560KiB
//  { 640,  448,  640,  448, 16, cfA1R5G5B5}, //  560KiB
//  { 640,  448,  640,  448, 24, cfB8G8R8},   //  840KiB
//  { 640,  448,  640,  448, 32, cfA8B8G8R8}, // 1120KiB

  // EDTV
//  { 720,  480,  720,  480, 16, cfA1R5G5B5}, //  675KiB
//  { 720,  480,  720,  480, 24, cfB8G8R8},   // 1012KiB
//  { 720,  480,  720,  480, 32, cfA8B8G8R8}, // 1350KiB
  // HDTV
//  {1280,  720, 1280,  720, 16, cfA1R5G5B5}, // 1800KiB
//  {1280,  720, 1280,  720, 24, cfB8G8R8},   // 2700KiB
//  {1280,  720, 1280,  720, 32, cfA8B8G8R8}, // 3600KiB
//  {1920,  540, 1920,  540, 16, cfA1R5G5B5}, // 2025KiB
//  {1920,  540, 1920,  540, 24, cfB8G8R8},   // 3038KiB
//  {1920,  540, 1920,  540, 32, cfA8B8G8R8}, // 4050KiB
//  {1920, 1080, 1920, 1080, 16, cfA1R5G5B5}, // 4050KiB

  // VGA
//  { 640,  480,  640,  480, 16, cfA1R5G5B5}, //  600KiB
//  { 640,  480,  640,  480, 24, cfB8G8R8},   //  900KiB
//  { 640,  480,  640,  480, 32, cfA8B8G8R8}, // 1200KiB
  // SVGA
//  { 800,  600,  800,  600, 16, cfA1R5G5B5}, //  938KiB
//  { 800,  600,  800,  600, 24, cfB8G8R8},   // 1406KiB
//  { 800,  600,  800,  600, 32, cfA8B8G8R8}, // 1875KiB
  // XGA
//  {1024,  768, 1024,  768, 16, cfA1R5G5B5}, // 1536KiB
//  {1024,  768, 1024,  768, 24, cfB8G8R8},   // 2304KiB
//  {1024,  768, 1024,  768, 32, cfA8B8G8R8}, // 3072KiB
  // SXGA
//  {1280, 1024, 1280, 1024, 16, cfA1R5G5B5}, // 2560KiB
};
static const int videoModeCount(sizeof(videoModes) / sizeof(SVideoMode));

DECLARE_GS_PACKET(GsCmdBuffer,50);


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
 : bDataWaiting_(false)
{
  INIT_GS_PACKET(GsCmdBuffer,50);

  // Create new packet
  BEGIN_GS_PACKET(GsCmdBuffer);
  GIF_TAG_AD(GsCmdBuffer, 1, 0, 0, 0);

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
    // Set active frame
    GIF_DATA_AD(GsCmdBuffer, frame_1, GS_FRAME((uint32_t)pSurface_->p >> 13, pSurface_->mode.width >> 6, pSurface_->psm_, 0));
    bDataWaiting_ = true;
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
    // FIXME: shouldn't be here
    REG_GS_CSR = REG_GS_CSR & 8;
    while(!(REG_GS_CSR & 8));

    // Send packet to GS
    SEND_GS_PACKET(GsCmdBuffer);

    // Create new packet
    BEGIN_GS_PACKET(GsCmdBuffer);
    GIF_TAG_AD(GsCmdBuffer, 1, 0, 0, 0);
  }
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

  GIF_DATA_AD(GsCmdBuffer, prim, GS_PRIM(PRIM_POINT, 0, 0, 0, 0, 0, 0, 0, 0));
  GIF_DATA_AD(GsCmdBuffer, rgbaq, GS_RGBAQ(color_.r, color_.g, color_.b, 0x80, 0));
  GIF_DATA_AD(GsCmdBuffer, xyz2, GS_XYZ2(x<<4, y<<4, 0));
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

  GIF_DATA_AD(GsCmdBuffer, prim, GS_PRIM(PRIM_SPRITE, 0, 0, 0, 0, 0, 0, 0, 0));
  GIF_DATA_AD(GsCmdBuffer, rgbaq, GS_RGBAQ(color_.r, color_.g, color_.b, 0x80, 0));
  GIF_DATA_AD(GsCmdBuffer, xyz2, GS_XYZ2(x<<4, y<<4, 0));
  GIF_DATA_AD(GsCmdBuffer, xyz2, GS_XYZ2((x+width+1)<<4, (y+height+1)<<4, 0));
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

  GIF_DATA_AD(GsCmdBuffer, prim, GS_PRIM(PRIM_LINE, 0, 0, 0, 0, 0, 0, 0, 0));
  GIF_DATA_AD(GsCmdBuffer, rgbaq, GS_RGBAQ(color_.r, color_.g, color_.b, 0x80, 0));
  GIF_DATA_AD(GsCmdBuffer, xyz2, GS_XYZ2(x1<<4, y1<<4, 0));
  GIF_DATA_AD(GsCmdBuffer, xyz2, GS_XYZ2(x2<<4, y2<<4, 0));
  bDataWaiting_ = true;
}

//---------------------------------------------------------------------------
void
CPS22DRenderer::drawRect(int x, int y, unsigned int width, unsigned int height)
{
  x += GS_X_BASE;
  y += GS_Y_BASE;

  GIF_DATA_AD(GsCmdBuffer, prim, GS_PRIM(PRIM_LINE_STRIP, 0, 0, 0, 0, 0, 0, 0, 0));
  GIF_DATA_AD(GsCmdBuffer, rgbaq, GS_RGBAQ(color_.r, color_.g, color_.b, 0x80, 0));
  GIF_DATA_AD(GsCmdBuffer, xyz2, GS_XYZ2(x<<4, y<<4, 0));
  GIF_DATA_AD(GsCmdBuffer, xyz2, GS_XYZ2(x+width<<4, y<<4, 0));
  GIF_DATA_AD(GsCmdBuffer, xyz2, GS_XYZ2(x+width<<4, y+height<<4, 0));
  GIF_DATA_AD(GsCmdBuffer, xyz2, GS_XYZ2(x<<4, y+height<<4, 0));
  GIF_DATA_AD(GsCmdBuffer, xyz2, GS_XYZ2(x<<4, y<<4, 0));
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
  INIT_GS_PACKET(GsCmdBuffer,50);
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
CPS2VideoDevice::getMode(SVideoMode ** mode)
{
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

  BEGIN_GS_PACKET(GsCmdBuffer);
  GIF_TAG_AD(GsCmdBuffer, 1, 0, 0, 0);

  // Use drawing parameters from PRIM register
  GIF_DATA_AD(GsCmdBuffer, prmodecont, 1);
  // Setup frame buffers. Point to 0 initially.
  GIF_DATA_AD(GsCmdBuffer, frame_1, GS_FRAME(0, pCurrentPS2Mode_->width >> 6, pCurrentPS2Mode_->psm, 0));
  // Displacement between Primitive and Window coordinate systems.
  GIF_DATA_AD(GsCmdBuffer, xyoffset_1, GS_XYOFFSET(GS_X_BASE<<4, GS_Y_BASE<<4));
  // Clip to frame buffer.
  GIF_DATA_AD(GsCmdBuffer, scissor_1, GS_SCISSOR(0, pCurrentPS2Mode_->width, 0, pCurrentPS2Mode_->height));

  SEND_GS_PACKET(GsCmdBuffer);
}

//---------------------------------------------------------------------------
void
CPS2VideoDevice::getSurface(CSurface ** surface, ESurfaceType type)
{
  CPS2Surface * pSurface = 0;

  switch(type)
  {
    case stSCREEN:
    {
      pSurface = new CPS2Surface;
      pSurface->mode = *pCurrentMode_;
      pSurface->psm_ = pCurrentPS2Mode_->psm;
      pSurface->p    = 0x00000000;
      break;
    }
    default:
      ;
  };

  *surface = pSurface;
}

//---------------------------------------------------------------------------
void
CPS2VideoDevice::getRenderer(I2DRenderer ** renderer)
{
  *renderer = new CPS22DRenderer;
}

//---------------------------------------------------------------------------
void
CPS2VideoDevice::waitVSync()
{
  REG_GS_CSR = REG_GS_CSR & 8;
  while(!(REG_GS_CSR & 8));
}

//---------------------------------------------------------------------------
void
CPS2VideoDevice::displaySurface(CSurface * surface)
{
  CPS2Surface * pNewSurface = (CPS2Surface *)surface;

  // Always VSync, even if the frame is not new.
  if(vSync_ == true)
  {
    REG_GS_CSR = REG_GS_CSR & 8;
    while(!(REG_GS_CSR & 8));
  }

  // Set new surface
  if(pNewSurface != NULL)
  {
    pSurface_ = pNewSurface;

    // Set visible frame
    REG_GS_DISPFB1 = GS_SET_DISPFB((uint32_t)pSurface_->p >> 13, pSurface_->mode.width >> 6, pSurface_->psm_, 0, 0);
  }
}
