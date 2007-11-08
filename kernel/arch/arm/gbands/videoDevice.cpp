#include "videoDevice.h"
#include "asm/arch/registers.h"
#include "asm/arch/macros.h"
#include "kernel/debug.h"


volatile bool bSwap(false);

static const SVideoMode videoModes[] =
{
#ifdef GBA
//  {240, 160, 16, cfX1R5G5B5}, // 3:2
//  {240, 160,  8, cf????????}, // 3:2
  {160, 128, 16, cfX1R5G5B5}, // 5:4
//  {120,  80, 16, cfX1R5G5B5}, // 3:2
#endif // GBA
#ifdef NDS9
  {256, 192, 16, cfA1R5G5B5}, // 4:3
#endif // NDS9
};
static const int videoModeCount(sizeof(videoModes) / sizeof(SVideoMode));


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CGBASurface::CGBASurface()
 : CSurface()
{
}

//---------------------------------------------------------------------------
CGBASurface::~CGBASurface()
{
}

//---------------------------------------------------------------------------
void
CGBASurface::fill()
{
  dmaFill16(fmtFillColor_ | 0x8000, p, width_ * height_);
}

//---------------------------------------------------------------------------
void
CGBASurface::fillRect_i(int x, int y, unsigned int width, unsigned int height)
{
  for(unsigned int iY(y); iY < (y + height); iY++)
  {
    dmaFill16(fmtFillColor_ | 0x8000, &((uint16_t *)p)[iY * width_ + x], width);
  }
}

//---------------------------------------------------------------------------
void
CGBASurface::swap(bool sync)
{
  if(pBack != 0)
  {
    if(sync == true)
      waitVSync();

#ifdef GBA
    if((width_ == 240)/* && (bpp_ == 16)*/)
    {
      // Copy
      dmaCopy(pBack, pFront, (width_*height_) << 1);
    }
    else
    {
      // Swap
      REG_DISPCNT ^= 0x0010;
#endif // GBA
#ifdef NDS9
      REG_BG3CNT ^= 0x0800;
#endif // NDS9
      p = pFront;
      pFront = pBack;
      pBack = p;
#ifdef GBA
    }
#endif // GBA
  }
}

//---------------------------------------------------------------------------
void
CGBASurface::waitVSync()
{
  // Wait for VSync interrupt
  // FIXME: Busy waiting!
  bSwap = true;
  while(bSwap == true){}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CGBAVideoDevice::CGBAVideoDevice()
 : CAVideoDevice()
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
  if(bSwap == true)
    bSwap = false;

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
/*
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
*/
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
CGBAVideoDevice::getSurface(CSurface ** surface, ESurfaceType type, bool bDouble)
{
  switch(type)
  {
    case stSCREEN:
    {
      CSurface * pSurface = new CGBASurface;
      pSurface->width_ = pCurrentMode_->width;
      pSurface->height_= pCurrentMode_->height;
      pSurface->bpp_   = pCurrentMode_->bpp;
      pSurface->format_= pCurrentMode_->format;
      pSurface->pFront = (uint16_t *)0x6000000;
      if(bDouble == true)
      {
        // Allocate back buffer
#ifdef GBA
        if(pCurrentMode_->width == 240)
          pSurface->pBack  = new uint16_t[240*160];
        else
          pSurface->pBack  = (uint16_t *)0x600A000;
#endif // GBA
#ifdef NDS9
        pSurface->pBack  = (uint16_t *)0x06020000;
#endif // NDS9
        // Draw to back buffer
        pSurface->p      = pSurface->pBack;
      }
      else
      {
        pSurface->pBack  = 0;
        pSurface->p      = pSurface->pFront;
      }
      *surface = pSurface;
      break;
    }
    case stOFFSCREEN:
    {
      CSurface * pSurface = new CGBASurface;
      pSurface->width_ = pCurrentMode_->width;
      pSurface->height_= pCurrentMode_->height;
      pSurface->bpp_   = pCurrentMode_->bpp;
      pSurface->format_= pCurrentMode_->format;
      pSurface->p      = 0;
      *surface = pSurface;
      break;
    }
    default:
    {
      *surface = 0;
    }
  };
}
