#include "videoDevice.h"
#include <gccore.h>


static const SVideoMode videoModes[] =
{
  {320, 528, 320, 528, 32, cfX8B8G8R8},
};
static const int videoModeCount(sizeof(videoModes) / sizeof(SVideoMode));


//---------------------------------------------------------------------------
CNGCVideoDevice::CNGCVideoDevice()
 : CAVideoDevice()
 , pSurface_(NULL)
 , rmode_(NULL)
 , pCurrentMode_(NULL)
{
}

//---------------------------------------------------------------------------
CNGCVideoDevice::~CNGCVideoDevice()
{
}

//---------------------------------------------------------------------------
void
CNGCVideoDevice::listModes(const SVideoMode ** modes, int * modeCount)
{
  *modes = videoModes;
  *modeCount = videoModeCount;
}

//---------------------------------------------------------------------------
void
CNGCVideoDevice::getMode(SVideoMode ** mode)
{
//  *mode = pCurrentMode_;
}

//---------------------------------------------------------------------------
void
CNGCVideoDevice::setMode(const SVideoMode * mode)
{
  VIDEO_Init();

//  switch(VIDEO_GetCurrentTvMode())
//  {
//      case VI_NTSC:
//          rmode = &TVNtsc480IntDf;
//          break;
//      case VI_PAL:
          rmode_ = &TVPal528IntDf;
//          break;
//      case VI_MPAL:
//          rmode = &TVMpal480IntDf;
//          break;
//      default:
//          rmode = &TVNtsc480IntDf;
//          break;
//  }

  VIDEO_Configure(rmode_);

  pCurrentMode_ = mode;
}

//---------------------------------------------------------------------------
void
CNGCVideoDevice::getSurface(CSurface ** surface, ESurfaceType type)
{
  switch(type)
  {
    case stSCREEN:
    {
      CSurface * pSurface = new CSurface;
      pSurface->mode = *pCurrentMode_;
      pSurface->p = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode_));
      *surface = pSurface;
      break;
    }
/*
    case stOFFSCREEN:
    {
      CSurface * pSurface = new CSurface;
      pSurface->mode = *pCurrentMode_;
      *surface = pSurface;
      break;
    }
*/
    default:
    {
      *surface = 0;
    }
  };
}

//---------------------------------------------------------------------------
void
CNGCVideoDevice::getRenderer(I2DRenderer ** renderer)
{
  *renderer = new C2DRenderer;
}

//---------------------------------------------------------------------------
void
CNGCVideoDevice::waitVSync()
{
  VIDEO_Flush();
  VIDEO_WaitVSync();
  if(rmode_->viTVMode&VI_NON_INTERLACE)
    VIDEO_WaitVSync();
}

//---------------------------------------------------------------------------
void
CNGCVideoDevice::displaySurface(CSurface * surface)
{
  // Always VSync, even if the frame is not new.
  if(vSync_ == true)
    waitVSync();

  // Set new surface if it changed
  if((surface != NULL) && (surface != pSurface_))
  {
    pSurface_ = surface;

    VIDEO_SetNextFramebuffer(pSurface_->p);
    VIDEO_SetBlack(FALSE);
    VIDEO_Flush();
    VIDEO_WaitVSync();
    if(rmode_->viTVMode&VI_NON_INTERLACE)
      VIDEO_WaitVSync();
  }
}
