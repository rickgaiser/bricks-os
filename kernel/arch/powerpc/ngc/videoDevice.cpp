#include "videoDevice.h"
#include <gccore.h>


static const SVideoMode videoModes[] =
{
  {320, 528, 320, 528, 32, cfR8G8B8},
};
static const int videoModeCount(sizeof(videoModes) / sizeof(SVideoMode));


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
#define RGB2YUV_SHIFT   16
#define RGB2YUV_LUMA    16
#define RGB2YUV_CHROMA 128

#define RGB2YUV_MUL_Yr ((int)( 0.299*(1<<RGB2YUV_SHIFT)))
#define RGB2YUV_MUL_Yg ((int)( 0.587*(1<<RGB2YUV_SHIFT)))
#define RGB2YUV_MUL_Yb ((int)( 0.114*(1<<RGB2YUV_SHIFT)))

#define RGB2YUV_MUL_Ur ((int)(-0.169*(1<<RGB2YUV_SHIFT)))
#define RGB2YUV_MUL_Ug ((int)(-0.331*(1<<RGB2YUV_SHIFT)))
#define RGB2YUV_MUL_Ub ((int)( 0.500*(1<<RGB2YUV_SHIFT)))

#define RGB2YUV_MUL_Vr ((int)( 0.500*(1<<RGB2YUV_SHIFT)))
#define RGB2YUV_MUL_Vg ((int)(-0.419*(1<<RGB2YUV_SHIFT)))
#define RGB2YUV_MUL_Vb ((int)(-0.081*(1<<RGB2YUV_SHIFT)))

#define clamp(x, y, z) ((z < x) ? x : ((z > y) ? y : z))

#define RGB2YUV_Y(r,g,b)  clamp(16, 235, ((RGB2YUV_MUL_Yr * r + RGB2YUV_MUL_Yg * g + RGB2YUV_MUL_Yb * b) >> RGB2YUV_SHIFT) + RGB2YUV_LUMA);
#define RGB2YUV_Cb(r,g,b) clamp(16, 240, ((RGB2YUV_MUL_Ur * r + RGB2YUV_MUL_Ug * g + RGB2YUV_MUL_Ub * b) >> RGB2YUV_SHIFT) + RGB2YUV_CHROMA);
#define RGB2YUV_Cr(r,g,b) clamp(16, 240, ((RGB2YUV_MUL_Vr * r + RGB2YUV_MUL_Vg * g + RGB2YUV_MUL_Vb * b) >> RGB2YUV_SHIFT) + RGB2YUV_CHROMA);

// -----------------------------------------------------------------------------
// 2 rgb pixels --> 2 yuv pixels (yuyv)
inline uint32_t
rgbrgb2yuyv(uint8_t r1, uint8_t g1, uint8_t b1, uint8_t r2, uint8_t g2, uint8_t b2)
{
  uint8_t Y1, Cb, Y2, Cr;

  Y1 = RGB2YUV_Y(r1, g1, b1);
  Y2 = RGB2YUV_Y(r2, g2, b2);

  r1 = (r1 + r2) >> 1;
  g1 = (g1 + g2) >> 1;
  b1 = (b1 + b2) >> 1;

  Cb = RGB2YUV_Cb(r1, g1, b1);
  Cr = RGB2YUV_Cr(r1, g1, b1);

  return ((((uint8_t)Y1) << 24) |
          (((uint8_t)Cb) << 16) |
          (((uint8_t)Y2) <<  8) |
          (((uint8_t)Cr)      ));
}

// -----------------------------------------------------------------------------
// 1 rgb pixel --> 2 yuv pixels (yuyv)
inline uint32_t
rgb2yuyv(uint8_t r, uint8_t g, uint8_t b)
{
  uint8_t Y, Cb, Cr;

  Y  = RGB2YUV_Y(r, g, b);
  Cb = RGB2YUV_Cb(r, g, b);
  Cr = RGB2YUV_Cr(r, g, b);

  return ((((uint8_t)Y ) << 24) |
          (((uint8_t)Cb) << 16) |
          (((uint8_t)Y ) <<  8) |
          (((uint8_t)Cr)      ));
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CNGC2DRenderer::CNGC2DRenderer()
 : C2DRenderer()
{
}

//---------------------------------------------------------------------------
CNGC2DRenderer::~CNGC2DRenderer()
{
}

//---------------------------------------------------------------------------
void
CNGC2DRenderer::setColor(uint8_t r, uint8_t g, uint8_t b)
{
  color_.r = r;
  color_.g = g;
  color_.b = b;
  color_.a = 255;

  fmtColor_ = rgb2yuyv(r, g, b);
}

//---------------------------------------------------------------------------
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
  *renderer = new CNGC2DRenderer;
}

//---------------------------------------------------------------------------
void
CNGCVideoDevice::waitVSync()
{
  VIDEO_Flush();
  VIDEO_WaitVSync();
  if(rmode_->viTVMode & VI_NON_INTERLACE)
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
