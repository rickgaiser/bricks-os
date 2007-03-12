#include "videoDevice.h"
#include "asm/arch/registers.h"
#include "asm/arch/macros.h"
#include "iostream"


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
CGBASurface::fill(color_t color)
{
//  dmaFill16(color | 0x8000, p, width * height);
  fillRect(0, 0, this->width, this->height, color | 0x8000);
}

//---------------------------------------------------------------------------
void
CGBASurface::fillRect(int x, int y, int width, int height, color_t color)
{
//  for(int iY(y); iY < (y + height); iY++)
//  {
//    dmaFill16(color | 0x8000, &p[iY * this->width + x], width);
//  }
  CSurface::fillRect(x, y, width, height, color | 0x8000);
}

//---------------------------------------------------------------------------
void
CGBASurface::swap(bool bForceCopy)
{
  if(pBack != 0)
  {
#ifdef GBA
    if(bForceCopy == true)
    {
      dmaCopy(pBack, pFront, (width*height) << 1);
    }
    else
    {
      if(width == 240)
      {
        dmaCopy(pBack, pFront, (width*height) << 1);
      }
      else if(width == 160)
      {
        bSwap = true;
        while(bSwap == true){}
        p = pFront;
        pFront = pBack;
        pBack = p;
      }
    }
#endif // GBA
#ifdef NDS9
    if(bForceCopy == true)
    {
      dmaCopy(pBack, pFront, (width*height) << 1);
    }
    else
    {
      bSwap = true;
      while(bSwap == true){}
      p = pFront;
      pFront = pBack;
      pBack = p;
    }
#endif // NDS9
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
      {160, 128, 16, 2, pfR5G5B5}
    , {240, 160, 16, 2, pfR5G5B5}
#endif // GBA
#ifdef NDS9
      {256, 192, 16, 2, pfA1R5G5B5}
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
//  std::cout<<"Mode set to: "<<mode->xres<<"x"<<mode->yres<<"x"<<mode->bitsPerPixel<<std::endl;
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
  CSurface * pSurface = new CGBASurface;

  switch(type)
  {
    case stSCREEN:
    {
#ifdef GBA
      if(pCurrentMode_->xres == 240)
      {
        pSurface->pBack  = new pixel_t[240*160];
      }
      else if(pCurrentMode_->xres == 160)
      {
        pSurface->pBack  = (pixel_t *)0x600A000;
      }
      pSurface->width  = pCurrentMode_->xres;
      pSurface->height = pCurrentMode_->yres;
      pSurface->format = pfR5G5B5;
      pSurface->pFront = (pixel_t *)0x6000000;
      pSurface->p      = pSurface->pBack;
#endif // GBA
#ifdef NDS9
      pSurface->width  = 256;
      pSurface->height = 192;
      pSurface->format = pfA1R5G5B5;
      pSurface->pFront = (pixel_t *)0x6000000;
      pSurface->pBack  = (pixel_t *)(0x06000000 + 256 * 256 * 2);
      pSurface->p      = pSurface->pBack;
#endif // NDS9
      pSurface->key    = false;
      pSurface->clKey  = 0;
      break;
    }
    case stOFFSCREEN:
    {
      pSurface->width  = 0;
      pSurface->height = 0;
#ifdef GBA
      pSurface->format = pfR5G5B5;
#endif // GBA
#ifdef NDS9
      pSurface->format = pfA1R5G5B5;
#endif // NDS9
      pSurface->p      = 0;
      pSurface->key    = false;
      pSurface->clKey  = 0;
      break;
    }
    default:
    {
      delete pSurface;
      pSurface = 0;
    }
  };

  *surface = pSurface;
}
