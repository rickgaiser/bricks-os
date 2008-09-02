#include "videoDevice.h"
#include "asm/arch/registers.h"
#include "asm/arch/macros.h"
#include "kernel/debug.h"
#include "stddef.h"

#ifdef GBA
#include "glesContext.h"
#endif // GBA
#ifdef NDS9
#include "glesContextNDS.h"
#endif // NDS9


#ifdef GBA
  #define DEFAULT_VIDEO_MODE  videoModes[2] // 240x160x16
#endif // GBA
#ifdef NDS9
  #define DEFAULT_VIDEO_MODE  videoModes[0] // 256x192x16
#endif // NDS9
static const SVideoMode videoModes[] =
{
#ifdef GBA
  {240, 160, 240, 160, 16, cfX1B5G5R5}, // 3:2
  {240, 160, 240, 160,  8, cfR3G3B2},   // 3:2
  {160, 128, 160, 128, 16, cfX1B5G5R5}, // 5:4
  {160, 128, 120,  80, 16, cfX1B5G5R5}, // 3:2
#endif // GBA
#ifdef NDS9
  {256, 192, 256, 192, 16, cfA1B5G5R5}, // 4:3
#endif // NDS9
};
static const int videoModeCount(sizeof(videoModes) / sizeof(SVideoMode));


// NDS Configuration options:
//
// Top Display (engine a):
//   2D:
//     3x 128KiB (3 full frame buffers)
//   3D (option 1):
//     Direct rendering to display
//     3x 128KiB
//   3D (option 2):
//     1x 128KiB Render buffer
//     1x 128KiB Display buffer
//     1x 128KiB
//
// Bottom Display (engine b):
//   2D:
//     1x 128KiB (1 full frame buffer)


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CGBA2DRenderer::CGBA2DRenderer()
 : C2DRenderer()
{
}

//---------------------------------------------------------------------------
CGBA2DRenderer::~CGBA2DRenderer()
{
}

//---------------------------------------------------------------------------
void
CGBA2DRenderer::fill_i()
{
  if(pSurface_->mode.xpitch == pSurface_->mode.width)
  {
    if(pSurface_->mode.bpp == 8)
    {
      dmaFill16((fmtColor_ << 8) | fmtColor_, pSurface_->p, (pSurface_->mode.width * pSurface_->mode.height) >> 1);
    }
    else
    {
      dmaFill16(fmtColor_ | 0x8000, pSurface_->p, pSurface_->mode.width * pSurface_->mode.height);
    }
  }
  else
  {
    fillRect_i(0, 0, pSurface_->mode.width, pSurface_->mode.height);
  }
}

//---------------------------------------------------------------------------
void
CGBA2DRenderer::fillRect_i(int x, int y, unsigned int width, unsigned int height)
{
  unsigned int iBase(y * pSurface_->mode.xpitch + x);

  if(pSurface_->mode.bpp == 8)
  {
    for(unsigned int iY(y); iY < (y + height); iY++)
    {
      dmaFill16((fmtColor_ << 8) | fmtColor_, &((uint8_t *)pSurface_->p)[iBase], width >> 1);
      iBase += pSurface_->mode.xpitch;
    }
  }
  else
  {
    for(unsigned int iY(y); iY < (y + height); iY++)
    {
      dmaFill16(fmtColor_ | 0x8000, &((uint16_t *)pSurface_->p)[iBase], width);
      iBase += pSurface_->mode.xpitch;
    }
  }
}

//---------------------------------------------------------------------------
void
CGBA2DRenderer::drawHLine_i(int x, int y, unsigned int width)
{
  unsigned int iBase(y * pSurface_->mode.xpitch + x);

  if(pSurface_->mode.bpp == 8)
  {
    dmaFill16((fmtColor_ << 8) | fmtColor_, &((uint8_t *)pSurface_->p)[iBase], width >> 1);
  }
  else
  {
    dmaFill16(fmtColor_ | 0x8000, &((uint16_t *)pSurface_->p)[iBase], width);
  }
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CGBAVideoDevice::CGBAVideoDevice()
 : CAVideoDevice()
 , pSurface_(NULL)
 , iSurfacesFree_(0)
 , iFrameCount_(0)
 , pCurrentMode_(NULL)
{
  // Initialize mutex to a locked state so we can use it to wait for VSync
  k_pthread_mutex_init(&mutex_, NULL);
  k_pthread_cond_init(&condVSync_, NULL);

  CInterruptManager::attach(0, this);
  // Enable VBlank interrupt
  REG_DISPSTAT |= IRQ_VBLANK_ENABLE;
}

//---------------------------------------------------------------------------
CGBAVideoDevice::~CGBAVideoDevice()
{
}

//---------------------------------------------------------------------------
int
CGBAVideoDevice::isr(int irq)
{
  iFrameCount_++;

  // Notify swap function that we have vertical sync
  k_pthread_cond_broadcast(&condVSync_);

  return 0;
}

//---------------------------------------------------------------------------
void
CGBAVideoDevice::listModes(const SVideoMode ** modes, int * modeCount)
{
  *modes = videoModes;
  *modeCount = videoModeCount;
}

//---------------------------------------------------------------------------
void
CGBAVideoDevice::getCurrentMode(const SVideoMode ** mode)
{
  *mode = pCurrentMode_;
}

//---------------------------------------------------------------------------
void
CGBAVideoDevice::getDefaultMode(const SVideoMode ** mode)
{
  *mode = &DEFAULT_VIDEO_MODE;
}

//---------------------------------------------------------------------------
void
CGBAVideoDevice::setMode(const SVideoMode * mode)
{
//  printk("Mode set to: %dx%dx%d\n", mode->xres, mode->yres, mode->bitsPerPixel);

#ifdef GBA
  // Setup BG2
  REG_BG2CNT  = BG_BMP16_256x256;
  REG_BG2_XDX = 1 << 8;
  REG_BG2_XDY = 0;
  REG_BG2_YDX = 0;
  REG_BG2_YDY = 1 << 8;
  REG_BG2_CX  = 0;
  REG_BG2_CY  = 0;

  if((mode->width == 240) && (mode->height == 160) && (mode->bpp == 16))
  {
    // Mode 3:
    //  - 240x160x16
    //  - BG2 Only
    //  - 1 frame fits in VRAM! Double buffering needs DMA copy from RAM to VRAM
    REG_DISPCNT = MODE_3 | BG2_ENABLE;
    iSurfacesFree_ = 0x01;
  }
  else if((mode->width == 240) && (mode->height == 160) && (mode->bpp == 8))
  {
    // Mode 4:
    //  - 240x160x8
    //  - BG2 Only
    //  - 2 frames fit in VRAM
    //  - paletted
    REG_DISPCNT = MODE_4 | BG2_ENABLE;
    iSurfacesFree_ = 0x03;

    // Set default palette (cfR3G3B2)
    for(uint32_t i(0); i < 256; i++)
    {
      BG_PALETTE[i] = BxColorFormat_FromRGB(
        cfX1B5G5R5,
        BxColorFormat_GetR(cfR3G3B2, i),
        BxColorFormat_GetG(cfR3G3B2, i),
        BxColorFormat_GetB(cfR3G3B2, i));
    }
  }
  else if((mode->width == 160) && (mode->height == 128) && (mode->bpp == 16))
  {
    // Mode 6:
    //  - 160x128x16
    //  - BG2 Only
    //  - 2 frames fit in VRAM
    //  - NOTE: The frames are not full screen! They can be stretched though.
    // 1:1 pixel ratio (not full screen)
    REG_BG2_XDX = 1<<8; // 1.00x
    REG_BG2_YDY = 1<<8; // 1.00x
    // Full screen (aspect ratio changed, looks bad)
    //REG_BG2_XDX = (160<<8)/240; // 1.50x
    //REG_BG2_YDY = (128<<8)/160; // 1.25x
    // 3:2 pixel ratio (looks bad, pixels lost)
    //REG_BG2_XDX = (2<<8)/3; // 1.50x
    //REG_BG2_YDY = (2<<8)/3; // 1.50x
    REG_DISPCNT = MODE_5 | BG2_ENABLE;
    iSurfacesFree_ = 0x03;
  }
  else if((mode->width == 120) && (mode->height == 80) && (mode->bpp == 16))
  {
    // Mode 6:
    //  - 160x128x16, stretched 2x (visible: 120x80x16)
    //  - BG2 Only
    //  - 2 frames fit in VRAM
    REG_BG2_XDX = (1<<8)/2; // 2.00x
    REG_BG2_YDY = (1<<8)/2; // 2.00x
    REG_DISPCNT = MODE_5 | BG2_ENABLE;
    iSurfacesFree_ = 0x03;
  }
#endif // GBA
#ifdef NDS9
  // Setup BG3
  REG_BG3_XDX = 1 << 8;
  REG_BG3_XDY = 0;
  REG_BG3_YDX = 0;
  REG_BG3_YDY = 1 << 8;
  REG_BG3_CX  = 0;
  REG_BG3_CY  = 0;

  // Top display
  //REG_DISPCNT     = DISP_SOURCE_VRAM | DISP_VRAMA;
  REG_DISPCNT     = MODE_5 | BG3_ENABLE | DISP_SOURCE_ENGINE;
  REG_BG3CNT      = BG_BMP16_256x256; // 0x06000000 // | 0x0800 for 0x06020000

  // Bottom display
  REG_SUB_DISPCNT = MODE_5 | BG3_ENABLE | DISP_SOURCE_ENGINE;
  REG_SUB_BG3CNT  = BG_BMP16_256x256; // 0x06200000

  // Setup VRAM banks
  REG_VRAM_A_CR = VRAM_ENABLE | VRAM_OFFSET(0) | VRAM_TYPE_MAIN_BG; // 128KiB @ 0x06000000 (EngineA)
  REG_VRAM_B_CR = VRAM_ENABLE | VRAM_OFFSET(1) | VRAM_TYPE_MAIN_BG; // 128KiB @ 0x06020000 (EngineA)
  REG_VRAM_C_CR = VRAM_ENABLE | 4;                                  // 128KiB @ 0x06200000 (EngineB)
  REG_VRAM_D_CR = VRAM_ENABLE | VRAM_OFFSET(2) | VRAM_TYPE_MAIN_BG; // 128KiB @ 0x06040000 (EngineA)

  iSurfacesFree_ = 0x07;
#endif // NDS9
  // Enable VBlank interrupt
  REG_DISPSTAT |= IRQ_VBLANK_ENABLE;

  pCurrentMode_ = mode;
}

//---------------------------------------------------------------------------
void
CGBAVideoDevice::getSurface(CSurface ** surface, int width, int height)
{
  *surface = 0;

  if(((uint32_t)width == pCurrentMode_->width) && ((uint32_t)height == pCurrentMode_->height))
  {
    if(iSurfacesFree_ != 0)
    {
      *surface = new CSurface;
      (*surface)->mode = *pCurrentMode_;

      // Allocate one of the available surfaces in VRAM
      if(iSurfacesFree_ & (1<<0))
      {
        iSurfacesFree_ &= ~(1<<0);
        (*surface)->p = (uint16_t *)0x06000000;
      }
      else if(iSurfacesFree_ & (1<<1))
      {
        iSurfacesFree_ &= ~(1<<1);
#ifdef GBA
        (*surface)->p = (uint16_t *)0x0600A000;
#endif // GBA
#ifdef NDS9
        (*surface)->p = (uint16_t *)0x06020000;
#endif // NDS9
      }
#ifdef NDS9
      else if(iSurfacesFree_ & (1<<2))
      {
        iSurfacesFree_ &= ~(1<<2);
        (*surface)->p = (uint16_t *)0x06040000;
      }
      else if(iSurfacesFree_ & (1<<3))
      {
        iSurfacesFree_ &= ~(1<<3);
        (*surface)->p = (uint16_t *)0x06060000;
      }
#endif // NDS9
    }
  }
  else
  {
    // Allocate a surface in EWRAM/IWRAM
    CSurface * pSurface = new CSurface;
    pSurface->mode        = *pCurrentMode_;
    pSurface->mode.xpitch = width;
    pSurface->mode.ypitch = height;
    pSurface->mode.width  = width;
    pSurface->mode.height = height;
    pSurface->p = new uint8_t[width * height * (pCurrentMode_->bpp / 8)];
    *surface = pSurface;
  }
}

//---------------------------------------------------------------------------
void
CGBAVideoDevice::get2DRenderer(I2DRenderer ** renderer)
{
  *renderer = new CGBA2DRenderer;
}

//---------------------------------------------------------------------------
void
CGBAVideoDevice::get3DRenderer(I3DRenderer ** renderer)
{
#ifdef GBA
  *renderer = new CGBAGLESContext;
//  *renderer = new CSoftGLESFixed;
#endif // GBA
#ifdef NDS9
  *renderer = new CNDSGLESContext;
#endif // NDS9
}

//---------------------------------------------------------------------------
uint32_t
CGBAVideoDevice::getFrameNr()
{
  return iFrameCount_;
}

//---------------------------------------------------------------------------
uint32_t
CGBAVideoDevice::waitVSync()
{
  k_pthread_cond_wait(&condVSync_, &mutex_);

  return iFrameCount_;
}

//---------------------------------------------------------------------------
void
CGBAVideoDevice::displaySurface(CSurface * surface)
{
  // Always VSync, even if the frame is not new.
  if(vSync_ == true)
    waitVSync();

  // Set new surface if it changed
  if((surface != NULL) && (surface != pSurface_))
  {
    pSurface_ = surface;

#ifdef GBA
    if(pSurface_->p == (uint16_t *)0x6000000)
      REG_DISPCNT &= ~0x0010; // Clear bit
    else if (pSurface_->p == (uint16_t *)0x600A000)
      REG_DISPCNT |= 0x0010; // Set bit
#endif // GBA
#ifdef NDS9
    if(pSurface_->p == (uint16_t *)0x06000000)
      REG_DISPCNT = DISP_SOURCE_VRAM | DISP_VRAMA;
    else if (pSurface_->p == (uint16_t *)0x06020000)
      REG_DISPCNT = DISP_SOURCE_VRAM | DISP_VRAMB;
    else if (pSurface_->p == (uint16_t *)0x06040000)
      REG_DISPCNT = DISP_SOURCE_VRAM | DISP_VRAMC;
    else if (pSurface_->p == (uint16_t *)0x06060000)
      REG_DISPCNT = DISP_SOURCE_VRAM | DISP_VRAMD;
#endif // NDS9
  }
}

//---------------------------------------------------------------------------
void
CGBAVideoDevice::bitBlt(CSurface * dest, int dx, int dy, int w, int h, CSurface * source, int sx, int sy)
{
  unsigned int iSrcBase(sy * source->mode.xpitch + sx);
  unsigned int iDstBase(dy * dest->mode.xpitch   + dx);

  if(dest->mode.bpp == 8)
  {
    for(int iY(0); iY < h; iY++)
    {
      dmaCopy(&((uint8_t *)dest->p)[iDstBase], &((uint8_t *)source->p)[iSrcBase], w);
      iSrcBase += source->mode.xpitch;
      iDstBase += dest->mode.xpitch;
    }
  }
  else
  {
    for(int iY(0); iY < h; iY++)
    {
      dmaCopy(&((uint16_t *)source->p)[iSrcBase], &((uint16_t *)dest->p)[iDstBase], w << 1);
      iSrcBase += source->mode.xpitch;
      iDstBase += dest->mode.xpitch;
    }
  }
}
