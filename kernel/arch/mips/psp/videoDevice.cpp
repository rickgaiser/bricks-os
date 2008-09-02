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
    case cfB5G6R5:   sceDisplaySetMode(PSP_DISPLAY_PIXEL_FORMAT_565,  mode->width, mode->height); break;
    case cfA1B5G5R5: sceDisplaySetMode(PSP_DISPLAY_PIXEL_FORMAT_5551, mode->width, mode->height); break;
    case cfA4B4G4R4: sceDisplaySetMode(PSP_DISPLAY_PIXEL_FORMAT_4444, mode->width, mode->height); break;
    case cfA8B8G8R8: sceDisplaySetMode(PSP_DISPLAY_PIXEL_FORMAT_8888, mode->width, mode->height); break;
    default:
      ;
  };
}

//---------------------------------------------------------------------------
// Align to 128bit == 16byte == 0xf
#define mallocSurface(size) \
  (void *)(((uint32_t)new uint8_t[size + 0xf]) & (~0xf))
//---------------------------------------------------------------------------
//static uint32_t bytesUsed(0);
void
CPSPVideoDevice::getSurface(CSurface ** surface, int width, int height)
{
  CSurface * pSurface = new CSurface;

  uint32_t size = pCurrentMode_->xpitch * pCurrentMode_->ypitch * (pCurrentMode_->bpp / 8);

  pSurface->mode        = *pCurrentMode_;
  //pSurface->p           = (void *)(0x04000000 + bytesUsed);
  pSurface->p           = mallocSurface(size);

  // Add the bytes we just used
  //bytesUsed += size;

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
  sceDisplayWaitVblankStart();

  return iFrameCount_;
}

//---------------------------------------------------------------------------
void
CPSPVideoDevice::displaySurface(CSurface * surface)
{
  PspDisplayPixelFormats format;
  switch(pCurrentMode_->format)
  {
    case cfB5G6R5:   format = PSP_DISPLAY_PIXEL_FORMAT_565;  break;
    case cfA1B5G5R5: format = PSP_DISPLAY_PIXEL_FORMAT_5551; break;
    case cfA4B4G4R4: format = PSP_DISPLAY_PIXEL_FORMAT_4444; break;
    default:
    case cfA8B8G8R8: format = PSP_DISPLAY_PIXEL_FORMAT_8888; break;
  };

  // Always VSync, even if the frame is not new.
  if(vSync_ == true)
    waitVSync();

  // FIXME: isr should update this, but we don't have interrupts
  iFrameCount_++;

  // Set new surface
  if(surface != NULL)
  {
    pSurface_ = surface;

    sceKernelDcacheWritebackAll();
    sceDisplaySetFrameBuf((void *)(pSurface_->p), pCurrentMode_->xpitch, format, PSP_DISPLAY_SETBUF_IMMEDIATE);
  }
}
