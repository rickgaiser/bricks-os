#include "videoDevice.h"
#include <pspdebug.h>
#include <pspdisplay.h>
#include <pspkernel.h>
#include <pspgu.h>


static const SVideoMode videoModes[] =
{
//  {512, 272, 480, 272, 16, cfB5G6R5},
//  {512, 272, 480, 272, 16, cfA1B5G5R5},
//  {512, 272, 480, 272, 16, cfA4B4G4R4},
  {512, 272, 480, 272, 32, cfA8B8G8R8},
};
static const int videoModeCount(sizeof(videoModes) / sizeof(SVideoMode));


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CPSPVideoDevice::CPSPVideoDevice()
 : CAVideoDevice()
 , pSurface_(NULL)
 , pCurrentMode_(NULL)
{
}

//---------------------------------------------------------------------------
CPSPVideoDevice::~CPSPVideoDevice()
{
}

//---------------------------------------------------------------------------
void
CPSPVideoDevice::listModes(const SVideoMode ** modes, int * modeCount)
{
  *modes = videoModes;
  *modeCount = videoModeCount;
}

//---------------------------------------------------------------------------
void
CPSPVideoDevice::getMode(SVideoMode ** mode)
{
}

//---------------------------------------------------------------------------
void
CPSPVideoDevice::setMode(const SVideoMode * mode)
{
  pCurrentMode_ = mode;

  switch(mode->format)
  {
    case cfB5G6R5:
      sceDisplaySetMode(PSP_DISPLAY_PIXEL_FORMAT_565, mode->width, mode->height);
      sceDisplaySetFrameBuf((void *)0x04000000, mode->xpitch, PSP_DISPLAY_PIXEL_FORMAT_565, PSP_DISPLAY_SETBUF_NEXTFRAME);
      break;
    case cfA1B5G5R5:
      sceDisplaySetMode(PSP_DISPLAY_PIXEL_FORMAT_5551, mode->width, mode->height);
      sceDisplaySetFrameBuf((void *)0x04000000, mode->xpitch, PSP_DISPLAY_PIXEL_FORMAT_5551, PSP_DISPLAY_SETBUF_NEXTFRAME);
      break;
    case cfA4B4G4R4:
      sceDisplaySetMode(PSP_DISPLAY_PIXEL_FORMAT_4444, mode->width, mode->height);
      sceDisplaySetFrameBuf((void *)0x04000000, mode->xpitch, PSP_DISPLAY_PIXEL_FORMAT_4444, PSP_DISPLAY_SETBUF_NEXTFRAME);
      break;
    case cfA8B8G8R8:
      sceDisplaySetMode(PSP_DISPLAY_PIXEL_FORMAT_8888, mode->width, mode->height);
      sceDisplaySetFrameBuf((void *)0x04000000, mode->xpitch, PSP_DISPLAY_PIXEL_FORMAT_8888, PSP_DISPLAY_SETBUF_NEXTFRAME);
      break;
    default:
      ;
  };
}

//---------------------------------------------------------------------------
void
CPSPVideoDevice::getSurface(CSurface ** surface, ESurfaceType type)
{
  CSurface * pSurface = 0;

  switch(type)
  {
    case stSCREEN:
    {
      pSurface = new CSurface;
      pSurface->mode = *pCurrentMode_;
      pSurface->p = (uint16_t *)0x04000000;
      *surface = pSurface;
      break;
    }
    default:
      ;
  };

  *surface = pSurface;
}

//---------------------------------------------------------------------------
void
CPSPVideoDevice::getRenderer(I2DRenderer ** renderer)
{
  *renderer = new C2DRenderer;
}

//---------------------------------------------------------------------------
void
CPSPVideoDevice::waitVSync()
{
  sceKernelDcacheWritebackAll();
  sceDisplayWaitVblankStart();
}

//---------------------------------------------------------------------------
void
CPSPVideoDevice::displaySurface(CSurface * surface)
{
  if(vSync_ == true)
    waitVSync();

  // Set new surface
  if(surface != NULL)
  {
    pSurface_ = surface;
  }
}
