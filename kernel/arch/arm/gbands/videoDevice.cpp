#include "videoDevice.h"
#include "asm/arch/registers.h"
#include "asm/arch/macros.h"
#include "kernel/debug.h"


volatile bool bSwap(false);


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
  dmaFill16(fillColor_ | 0x8000, p, width_ * height_);
}

//---------------------------------------------------------------------------
void
CGBASurface::fillRect(int x, int y, int width, int height)
{
  for(int iY(y); iY < (y + height); iY++)
  {
    dmaFill16(fillColor_ | 0x8000, &((uint16_t *)p)[iY * width_ + x], width);
  }
}

//---------------------------------------------------------------------------
void
CGBASurface::swap(bool bForceCopy)
{
  if(pBack != 0)
  {
    if(bForceCopy == true)
    {
      dmaCopy(pBack, pFront, (width_*height_) << 1);
    }
    else
    {
#ifdef GBA
      if(width_ == 240)
      {
        dmaCopy(pBack, pFront, (width_*height_) << 1);
      }
      else if(width_ == 160)
      {
#endif // GBA
        bSwap = true;
        while(bSwap == true){}
        p = pFront;
        pFront = pBack;
        pBack = p;
#ifdef GBA
      }
#endif // GBA
    }
  }
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
  if(bSwap == true)
  {
#ifdef GBA
    REG_DISPCNT ^= 0x0010;
#endif // GBA
#ifdef NDS9
    REG_BG3CNT ^= ((128 / 16) << 8);
#endif // NDS9
    bSwap = false;
  }

  return 0;
}

//---------------------------------------------------------------------------
void
CGBAVideoDevice::listModes(const SVideoMode ** modes, int * modeCount)
{
  static const SVideoMode videoModes[] =
  {
#ifdef GBA
      {160, 128, 16, 2, cfX1R5G5B5}
    , {240, 160, 16, 2, cfX1R5G5B5}
#endif // GBA
#ifdef NDS9
      {256, 192, 16, 2, cfA1R5G5B5}
#endif // NDS9
  };
  static const int videoModeCount(sizeof(videoModes) / sizeof(SVideoMode));

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
#ifdef GBA
  if(mode->xres == 240)
  {
    REG_DISPCNT   = MODE_3 | BG2_ENABLE;
  }
  else if(mode->xres == 160)
  {
    REG_DISPCNT   = MODE_5 | BG2_ENABLE;
  }
  REG_DISPSTAT |= IRQ_VBLANK_ENABLE;
#endif // GBA
#ifdef NDS9
  REG_DISPCNT   = MODE_5 | BG3_ENABLE;
  REG_DISPSTAT |= IRQ_VBLANK_ENABLE;
  REG_BG3CNT    = BG_BMP16_256x256;
  REG_BG3_XDX = 1 << 8;
  REG_BG3_XDY = 0;
  REG_BG3_YDX = 0;
  REG_BG3_YDY = 1 << 8;
  REG_BG3_CX  = 0;
  REG_BG3_CY  = 0;
  REG_VRAM_A_CR = VRAM_ENABLE | (0<<3) | VRAM_TYPE_MAIN_BG;
  REG_VRAM_B_CR = VRAM_ENABLE | (1<<3) | VRAM_TYPE_MAIN_BG;
#endif // NDS9

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
      CSurface * pSurface = new CGBASurface;
      pSurface->width_ = pCurrentMode_->xres;
      pSurface->height_= pCurrentMode_->yres;
      pSurface->format_= pCurrentMode_->format;
      pSurface->pFront = (uint16_t *)0x6000000;
#ifdef GBA
//      if(pCurrentMode_->xres == 240)
//      {
//        pSurface->pBack  = new uint16_t[240*160];
//      }
//      else if(pCurrentMode_->xres == 160)
//      {
//        pSurface->pBack  = (uint16_t *)0x600A000;
//      }
#endif // GBA
#ifdef NDS9
      pSurface->pBack  = (uint16_t *)(0x06000000 + 256 * 256 * 2);
#endif // NDS9
//      pSurface->p      = pSurface->pBack;
      pSurface->p      = pSurface->pFront;
      *surface = pSurface;
      break;
    }
    case stOFFSCREEN:
    {
      CSurface * pSurface = new CGBASurface;
      pSurface->width_ = 0;
      pSurface->height_= 0;
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
