#include "videoDevice.h"
#include "bios.h"
#include "dma.h"
#include "gs.h"
#include "gif.h"
#include "string.h"


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
static const SVideoMode videoModes[] =
{
  // PAL
//  { 640,  256, 16, cfA1R5G5B5}, //  320Kb
//  { 640,  256, 24, cfB8G8R8},   //  480Kb
  { 640,  256, 32, cfA8B8G8R8}, //  640Kb
//  { 640,  512, 16, cfA1R5G5B5}, //  640Kb
//  { 640,  512, 24, cfB8G8R8},   //  960Kb
//  { 640,  512, 32, cfA8B8G8R8}, // 1280Kb
  // NTSC
//  { 640,  224, 16, cfA1R5G5B5}, //  280Kb
//  { 640,  224, 24, cfB8G8R8},   //  420Kb
  { 640,  224, 32, cfA8B8G8R8}, //  560Kb
//  { 640,  448, 16, cfA1R5G5B5}, //  560Kb
//  { 640,  448, 24, cfB8G8R8},   //  840Kb
//  { 640,  448, 32, cfA8B8G8R8}, // 1120Kb

  // EDTV
//  { 720,  480, 16, cfA1R5G5B5}, //  675Kb
//  { 720,  480, 24, cfB8G8R8},   // 1012Kb
//  { 720,  480, 32, cfA8B8G8R8}, // 1350Kb
  // HDTV
//  {1280,  720, 16, cfA1R5G5B5}, // 1800Kb
//  {1280,  720, 24, cfB8G8R8},   // 2700Kb
//  {1280,  720, 32, cfA8B8G8R8}, // 3600Kb
//  {1920,  540, 16, cfA1R5G5B5}, // 2025Kb
//  {1920,  540, 24, cfB8G8R8},   // 3038Kb
//  {1920,  540, 32, cfA8B8G8R8}, // 4050Kb
//  {1920, 1080, 16, cfA1R5G5B5}, // 4050Kb

  // VGA
//  { 640,  480, 16, cfA1R5G5B5},
//  { 640,  480, 24, cfB8G8R8},
//  { 640,  480, 32, cfA8B8G8R8},
  // SVGA
//  { 800,  600, 16, cfA1R5G5B5},
//  { 800,  600, 24, cfB8G8R8},
//  { 800,  600, 32, cfA8B8G8R8},
  // XGA
//  {1024,  768, 16, cfA1R5G5B5},
//  {1024,  768, 24, cfB8G8R8},
//  {1024,  768, 32, cfA8B8G8R8},
  // SXGA
//  {1280, 1024, 16, cfA1R5G5B5},
};
static const int videoModeCount(sizeof(videoModes) / sizeof(SVideoMode));

uint32_t   gs_mem_current;
uint16_t   gs_origin_x;
uint16_t   gs_origin_y;
DECLARE_GS_PACKET(gs_dma_buf,50);


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
CPS22DRenderer::CPS22DRenderer(CSurface * surf)
{
  pSurface_ = dynamic_cast<CPS2Surface *>(surf);

  if(pSurface_ != NULL)
  {
    // Set visible frame
    REG_GS_DISPFB1  = GS_SET_DISPFB((uint32_t)pSurface_->p >> 13, pSurface_->width_ >> 6, pSurface_->psm_, 0, 0);

    // Set active frame
    BEGIN_GS_PACKET(gs_dma_buf);
    GIF_TAG_AD(gs_dma_buf, 1, 0, 0, 0);
    GIF_DATA_AD(gs_dma_buf, frame_1, GS_FRAME((uint32_t)pSurface_->p >> 13, pSurface_->width_ >> 6, pSurface_->psm_, 0));
    SEND_GS_PACKET(gs_dma_buf);
  }
}

//---------------------------------------------------------------------------
CPS22DRenderer::~CPS22DRenderer()
{
}

//---------------------------------------------------------------------------
void
CPS22DRenderer::setSurface(CSurface * surf)
{
  pSurface_ = dynamic_cast<CPS2Surface *>(surf);

  if(pSurface_ != NULL)
  {
    // Set visible frame
    REG_GS_DISPFB1  = GS_SET_DISPFB((uint32_t)pSurface_->p >> 13, pSurface_->width_ >> 6, pSurface_->psm_, 0, 0);

    // Set active frame
    BEGIN_GS_PACKET(gs_dma_buf);
    GIF_TAG_AD(gs_dma_buf, 1, 0, 0, 0);
    GIF_DATA_AD(gs_dma_buf, frame_1, GS_FRAME((uint32_t)pSurface_->p >> 13, pSurface_->width_ >> 6, pSurface_->psm_, 0));
    SEND_GS_PACKET(gs_dma_buf);
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
  x += gs_origin_x;
  y += gs_origin_y;

  BEGIN_GS_PACKET(gs_dma_buf);
  GIF_TAG_AD(gs_dma_buf, 1, 0, 0, 0);

  GIF_DATA_AD(gs_dma_buf, prim, GS_PRIM(PRIM_POINT, 0, 0, 0, 0, 0, 0, 0, 0));
  GIF_DATA_AD(gs_dma_buf, rgbaq, GS_RGBAQ(color_.r, color_.g, color_.b, 0x80, 0));
  GIF_DATA_AD(gs_dma_buf, xyz2, GS_XYZ2(x<<4, y<<4, 0));

  SEND_GS_PACKET(gs_dma_buf);
}

//---------------------------------------------------------------------------
void
CPS22DRenderer::fill()
{
  fillRect(0, 0, pSurface_->width_, pSurface_->height_);
}

//---------------------------------------------------------------------------
void
CPS22DRenderer::fillRect(int x, int y, unsigned int width, unsigned int height)
{
  x += gs_origin_x;
  y += gs_origin_y;

  BEGIN_GS_PACKET(gs_dma_buf);
  GIF_TAG_AD(gs_dma_buf, 1, 0, 0, 0);

  GIF_DATA_AD(gs_dma_buf, prim, GS_PRIM(PRIM_SPRITE, 0, 0, 0, 0, 0, 0, 0, 0));
  GIF_DATA_AD(gs_dma_buf, rgbaq, GS_RGBAQ(color_.r, color_.g, color_.b, 0x80, 0));
  GIF_DATA_AD(gs_dma_buf, xyz2, GS_XYZ2(x<<4, y<<4, 0));
  GIF_DATA_AD(gs_dma_buf, xyz2, GS_XYZ2((x+width+1)<<4, (y+height+1)<<4, 0));

  SEND_GS_PACKET(gs_dma_buf);
}

//---------------------------------------------------------------------------
void
CPS22DRenderer::drawLine(int x1, int y1, int x2, int y2)
{
  x1 += gs_origin_x;
  y1 += gs_origin_y;
  x2 += gs_origin_x;
  y2 += gs_origin_y;

  BEGIN_GS_PACKET(gs_dma_buf);
  GIF_TAG_AD(gs_dma_buf, 1, 0, 0, 0);

  GIF_DATA_AD(gs_dma_buf, prim, GS_PRIM(PRIM_LINE, 0, 0, 0, 0, 0, 0, 0, 0));
  GIF_DATA_AD(gs_dma_buf, rgbaq, GS_RGBAQ(color_.r, color_.g, color_.b, 0x80, 0));
  GIF_DATA_AD(gs_dma_buf, xyz2, GS_XYZ2(x1<<4, y1<<4, 0));
  GIF_DATA_AD(gs_dma_buf, xyz2, GS_XYZ2(x2<<4, y2<<4, 0));

  SEND_GS_PACKET(gs_dma_buf);
}

//---------------------------------------------------------------------------
void
CPS22DRenderer::drawRect(int x, int y, unsigned int width, unsigned int height)
{
  x += gs_origin_x;
  y += gs_origin_y;

  BEGIN_GS_PACKET(gs_dma_buf);
  GIF_TAG_AD(gs_dma_buf, 1, 0, 0, 0);

  GIF_DATA_AD(gs_dma_buf, prim, GS_PRIM(PRIM_LINE_STRIP, 0, 0, 0, 0, 0, 0, 0, 0));
  GIF_DATA_AD(gs_dma_buf, rgbaq, GS_RGBAQ(color_.r, color_.g, color_.b, 0x80, 0));
  GIF_DATA_AD(gs_dma_buf, xyz2, GS_XYZ2(x<<4, y<<4, 0));
  GIF_DATA_AD(gs_dma_buf, xyz2, GS_XYZ2(x+width<<4, y<<4, 0));
  GIF_DATA_AD(gs_dma_buf, xyz2, GS_XYZ2(x+width<<4, y+height<<4, 0));
  GIF_DATA_AD(gs_dma_buf, xyz2, GS_XYZ2(x<<4, y+height<<4, 0));
  GIF_DATA_AD(gs_dma_buf, xyz2, GS_XYZ2(x<<4, y<<4, 0));

  SEND_GS_PACKET(gs_dma_buf);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CPS2VideoDevice::CPS2VideoDevice()
 : CAVideoDevice()
 , pCurrentMode_(NULL)
 , pCurrentPS2Mode_(NULL)
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
CPS2VideoDevice::getMode(SVideoMode ** mode)
{
}

//---------------------------------------------------------------------------
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

  gs_origin_x = 1024;
  gs_origin_y = 1024;

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

  BEGIN_GS_PACKET(gs_dma_buf);
  GIF_TAG_AD(gs_dma_buf, 1, 0, 0, 0);
  // Use drawing parameters from PRIM register
  GIF_DATA_AD(gs_dma_buf, prmodecont, 1);
  // Setup frame buffers. Point to 0 initially.
  GIF_DATA_AD(gs_dma_buf, frame_1, GS_FRAME(0, pCurrentPS2Mode_->width >> 6, pCurrentPS2Mode_->psm, 0));
  // Displacement between Primitive and Window coordinate systems.
  GIF_DATA_AD(gs_dma_buf, xyoffset_1, GS_XYOFFSET(gs_origin_x<<4, gs_origin_y<<4));
  // Clip to frame buffer.
  GIF_DATA_AD(gs_dma_buf, scissor_1, GS_SCISSOR(0, pCurrentPS2Mode_->width, 0, pCurrentPS2Mode_->height));

  SEND_GS_PACKET(gs_dma_buf);
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
      pSurface->width_ = pCurrentPS2Mode_->width;
      pSurface->height_= pCurrentPS2Mode_->height;
      pSurface->bpp_   = pCurrentPS2Mode_->bpp;
      pSurface->format_= pCurrentMode_->format;
      pSurface->psm_   = pCurrentPS2Mode_->psm;
      pSurface->p      = 0x00000000;
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
