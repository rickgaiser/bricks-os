#include "videoDevice.h"
#include "kernel/2dRenderer.h"
#include "../../../../gl/softGLF.h"
#include <pspdebug.h>
#include <pspdisplay.h>
#include <pspkernel.h>
#include <pspgu.h>


#define DEFAULT_VIDEO_MODE  videoModes[3] // 480x272x32
static const SVideoMode videoModes[] =
{
  {512, 272, 480, 272, 16, cfB5G6R5},
  {512, 272, 480, 272, 16, cfA1B5G5R5},
  {512, 272, 480, 272, 16, cfA4B4G4R4},
  {512, 272, 480, 272, 32, cfA8B8G8R8},
};
static const int videoModeCount(sizeof(videoModes) / sizeof(SVideoMode));


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CPSPVideoDevice::CPSPVideoDevice()
 : CAVideoDevice()
 , pSurface_(NULL)
 , pCurrentMode_(NULL)
 , iFrameCount_(0)
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
CPSPVideoDevice::getCurrentMode(const SVideoMode ** mode)
{
  *mode = pCurrentMode_;
}

//---------------------------------------------------------------------------
void
CPSPVideoDevice::getDefaultMode(const SVideoMode ** mode)
{
  *mode = &DEFAULT_VIDEO_MODE;
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
CPSPVideoDevice::getSurface(CSurface ** surface, int width, int height)
{
  CSurface * pSurface = new CSurface;

  pSurface->mode = *pCurrentMode_;
  pSurface->p = (uint16_t *)0x04000000;

  *surface = pSurface;
}

//---------------------------------------------------------------------------
void
CPSPVideoDevice::get2DRenderer(I2DRenderer ** renderer)
{
  *renderer = new C2DRenderer;
}

//---------------------------------------------------------------------------
void
CPSPVideoDevice::get3DRenderer(I3DRenderer ** renderer)
{
  *renderer = new CSoftGLESFloat;
}

//---------------------------------------------------------------------------
uint32_t
CPSPVideoDevice::getFrameNr()
{
  return iFrameCount_;
}

//---------------------------------------------------------------------------
uint32_t
CPSPVideoDevice::waitVSync()
{
  sceKernelDcacheWritebackAll();
  sceDisplayWaitVblankStart();

  return iFrameCount_;
}

//---------------------------------------------------------------------------
void
CPSPVideoDevice::displaySurface(CSurface * surface)
{
  if(vSync_ == true)
    waitVSync();

  // FIXME: isr should update this, but we don't have interrupts
  iFrameCount_++;

  // Set new surface
  if(surface != NULL)
  {
    pSurface_ = surface;
  }
}
