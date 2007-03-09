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
CGBASurface::fillRect(int x, int y, int width, int height, color_t color)
{
#ifdef GBA
  for(int iY(y); iY < (y + height); iY++)
  {
    dmaFill16(color, &p[iY * this->width], width);
  }
#endif // GBA
#ifdef NDS9
  CSurface::fillRect(x, y, width, height, color);
#endif // NDS9
}

//---------------------------------------------------------------------------
void
CGBASurface::swap(bool bForceCopy)
{
  if(pBack != 0)
  {
#ifdef GBA
    // "bForceCopy" can be ignored, we always copy
    dmaCopy(pBack, pFront, (width*height) << 1);
#endif // GBA
#ifdef NDS9
    if(bForceCopy == true)
    {
      dmaCopy(pBack, pFront, (width*height) << 1);
    }
    else
    {
      REG_BG3CNT ^= ((128 / 16) << 8);
//      bSwap = true;
//      while(bSwap == true){}
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
#ifdef NDS9
  if(bSwap == true)
  {
    REG_BG3CNT ^= ((128 / 16) << 8);
    bSwap = false;
  }
#endif // NDS9

  return 0;
}

//---------------------------------------------------------------------------
void
CGBAVideoDevice::listModes(const SVideoMode ** modes, int * modeCount)
{
  static const SVideoMode videoModes[] =
  {
#ifdef GBA
    {240, 160, 16, 2, pfR5G5B5}
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
  std::cout<<"Mode set to: "<<mode->xres<<"x"<<mode->yres<<"x"<<mode->bitsPerPixel<<std::endl;
#ifdef GBA
  REG_DISPCNT   = MODE_3 | BG2_ENABLE;
  REG_DISPSTAT |= IRQ_VBLANK_ENABLE;
#endif // GBA
#ifdef NDS9
  REG_DISPCNT   = MODE_5 | BG3_ENABLE;
  REG_DISPSTAT |= IRQ_VBLANK_ENABLE;
  REG_BG3CNT    = BG_BMP16_256x256;
  REG_VRAM_A_CR = VRAM_ENABLE | (0<<3) | VRAM_TYPE_MAIN_BG;
  REG_VRAM_B_CR = VRAM_ENABLE | (1<<3) | VRAM_TYPE_MAIN_BG;
#endif // NDS9
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
      pSurface->width  = 240;
      pSurface->height = 160;
      pSurface->format = pfR5G5B5;
      pSurface->pFront = (pixel_t *)0x6000000;
      pSurface->pBack  = new pixel_t[240*160];
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
