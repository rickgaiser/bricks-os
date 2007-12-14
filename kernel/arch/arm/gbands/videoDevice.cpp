#include "videoDevice.h"
#include "asm/arch/registers.h"
#include "asm/arch/macros.h"
#include "kernel/debug.h"
#include "stddef.h"


static const SVideoMode videoModes[] =
{
#ifdef GBA
//  {240, 160, 240, 160, 16, cfX1B5G5R5}, // 3:2
//  {240, 160, 240, 160,  8, cfP8}, // 3:2
  {160, 128, 160, 128, 16, cfX1B5G5R5}, // 5:4
//  {160, 128, 120,  80, 16, cfX1B5G5R5}, // 3:2
#endif // GBA
#ifdef NDS9
  {256, 192, 256, 192, 16, cfA1B5G5R5}, // 4:3
#endif // NDS9
};
static const int videoModeCount(sizeof(videoModes) / sizeof(SVideoMode));


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
    dmaFill16(fmtColor_ | 0x8000, pSurface_->p, pSurface_->mode.width * pSurface_->mode.height);
  }
  else
  {
    unsigned int iBase(0);
    for(unsigned int iY(0); iY < pSurface_->mode.height; iY++)
    {
      dmaFill16(fmtColor_ | 0x8000, &((uint16_t *)pSurface_->p)[iBase], pSurface_->mode.width);
      iBase += pSurface_->mode.xpitch;
    }
  }
}

//---------------------------------------------------------------------------
void
CGBA2DRenderer::fillRect_i(int x, int y, unsigned int width, unsigned int height)
{
  unsigned int iBase(y * pSurface_->mode.xpitch + x);

  for(unsigned int iY(y); iY < (y + height); iY++)
  {
    dmaFill16(fmtColor_ | 0x8000, &((uint16_t *)pSurface_->p)[iBase], width);
    iBase += pSurface_->mode.xpitch;
  }
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CGBAVideoDevice::CGBAVideoDevice()
 : CAVideoDevice()
 , pSurface_(NULL)
 , bSwap_(false)
 , pCurrentMode_(NULL)
{
  CInterruptManager::attach(0, this);
}

//---------------------------------------------------------------------------
CGBAVideoDevice::~CGBAVideoDevice()
{
}

//---------------------------------------------------------------------------
int
CGBAVideoDevice::isr(int irq)
{
  // Notify swap function that we have vertical sync
  if(bSwap_ == true)
    bSwap_ = false;

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
CGBAVideoDevice::getMode(SVideoMode ** mode)
{
}

//---------------------------------------------------------------------------
void
CGBAVideoDevice::setMode(const SVideoMode * mode)
{
//  printk("Mode set to: %dx%dx%d\n", mode->xres, mode->yres, mode->bitsPerPixel);

  // Setup BG2
  REG_BG3CNT  = BG_BMP16_256x256;
  REG_BG2_XDX = 1 << 8;
  REG_BG2_XDY = 0;
  REG_BG2_YDX = 0;
  REG_BG2_YDY = 1 << 8;
  REG_BG2_CX  = 0;
  REG_BG2_CY  = 0;

  // Setup BG3
  REG_BG3CNT  = BG_BMP16_256x256;
  REG_BG3_XDX = 1 << 8;
  REG_BG3_XDY = 0;
  REG_BG3_YDX = 0;
  REG_BG3_YDY = 1 << 8;
  REG_BG3_CX  = 0;
  REG_BG3_CY  = 0;

#ifdef GBA
  if((mode->width == 240) && (mode->height == 160) && (mode->bpp == 16))
  {
    // Mode 3:
    //  - 240x160x16
    //  - BG2 Only
    //  - 1 frame fits in VRAM! Double buffering needs DMA copy from RAM to VRAM
    REG_DISPCNT = MODE_3 | BG2_ENABLE;
  }
  else if((mode->width == 240) && (mode->height == 160) && (mode->bpp == 8))
  {
    // Mode 4:
    //  - 240x160x8
    //  - BG2 Only
    //  - 2 frames fit in VRAM
    //  - paletted
    REG_DISPCNT = MODE_4 | BG2_ENABLE;
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
  }
#endif // GBA
#ifdef NDS9
  REG_DISPCNT   = MODE_5 | BG3_ENABLE;

  // Setup VRAM banks
  REG_VRAM_A_CR = VRAM_ENABLE | VRAM_OFFSET(0) | VRAM_TYPE_MAIN_BG; // 128KiB
  REG_VRAM_B_CR = VRAM_ENABLE | VRAM_OFFSET(1) | VRAM_TYPE_MAIN_BG; // 128KiB
  REG_VRAM_C_CR = VRAM_ENABLE | VRAM_OFFSET(2) | VRAM_TYPE_MAIN_BG; // 128KiB
  REG_VRAM_D_CR = VRAM_ENABLE | VRAM_OFFSET(3) | VRAM_TYPE_MAIN_BG; // 128KiB
#endif // NDS9
  // Enable VBlank interrupt
  REG_DISPSTAT |= IRQ_VBLANK_ENABLE;

  pCurrentMode_ = mode;
}

//---------------------------------------------------------------------------
void
CGBAVideoDevice::getSurface(CSurface ** surface, ESurfaceType type)
{
  switch(type)
  {
    case stSCREEN:
    {
      CSurface * pSurface = new CSurface;
      pSurface->mode = *pCurrentMode_;
      pSurface->p = (uint16_t *)0x6000000;
      *surface = pSurface;
      break;
    }
    case stOFFSCREEN:
    {
      CSurface * pSurface = new CSurface;
      pSurface->mode = *pCurrentMode_;
      // Allocate back buffer
#ifdef GBA
      if(pCurrentMode_->width == 240)
        pSurface->p = new uint16_t[240*160];
      else
        pSurface->p = (uint16_t *)0x600A000;
#endif // GBA
#ifdef NDS9
      pSurface->p = (uint16_t *)0x06020000;
#endif // NDS9
      *surface = pSurface;
      break;
    }
    default:
    {
      *surface = 0;
    }
  };
}

//---------------------------------------------------------------------------
void
CGBAVideoDevice::getRenderer(I2DRenderer ** renderer)
{
  *renderer = new CGBA2DRenderer;
}

//---------------------------------------------------------------------------
void
CGBAVideoDevice::waitVSync()
{
  bSwap_ = true;
  while(bSwap_ == true){}
}

//---------------------------------------------------------------------------
void
CGBAVideoDevice::displaySurface(CSurface * surface)
{
  // Always VSync, even if the frame is not new.
  if(vSync_ == true)
  {
    bSwap_ = true;
    while(bSwap_ == true){}
  }

  // Set new surface if it changed
  if((surface != NULL) && (surface != pSurface_))
  {
    pSurface_ = surface;

#ifdef GBA
    if(pSurface_->p == (uint16_t *)0x6000000)
    {
      REG_DISPCNT &= ~0x0010; // Clear bit
    }
    else if (pSurface_->p == (uint16_t *)0x600A000)
    {
      REG_DISPCNT |= 0x0010; // Set bit
    }
    else
    {
      // Copy?
    }
#endif // GBA

#ifdef NDS9
    if(pSurface_->p == (uint16_t *)0x6000000)
    {
      REG_BG3CNT &= ~0x0800; // Clear bit
    }
    else if (pSurface_->p == (uint16_t *)0x06020000)
    {
      REG_BG3CNT |= 0x0800; // Set bit
    }
    else
    {
      // Copy?
    }
#endif // NDS9
  }
}
