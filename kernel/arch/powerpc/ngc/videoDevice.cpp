#include "videoDevice.h"
#include "asm/arch/registers.h"
#include <gccore.h>


// Video registers for different modes
const uint32_t vid640480ntsc[] = {
0x0F060001, 0x476901AD, 0x02EA5140, 0x00030018,
0x00020019, 0x410C410C, 0x40ED40ED, 0x00435A4E,
0x00000000, 0x00435A4E, 0x00000000, 0x00000000,
0x110701AE, 0x10010001, 0x00010001, 0x00010001,
0x00000000, 0x00000000, 0x28500100, 0x1AE771F0,
0x0DB4A574, 0x00C1188E, 0xC4C0CBE2, 0xFCECDECF,
0x13130F08, 0x00080C0F, 0x00FF0000, 0x00000000,
0x02800000, 0x000000FF, 0x00FF00FF, 0x00FF00FF};

const uint32_t vid640480pal60[] = {
0x0F060001, 0x476901AD, 0x02EA5140, 0x00030018,
0x00020019, 0x410C410C, 0x40ED40ED, 0x00435A4E,
0x00000000, 0x00435A4E, 0x00000000, 0x00050176,
0x110701AE, 0x10010001, 0x00010001, 0x00010001,
0x00000000, 0x00000000, 0x28500100, 0x1AE771F0,
0x0DB4A574, 0x00C1188E, 0xC4C0CBE2, 0xFCECDECF,
0x13130F08, 0x00080C0F, 0x00FF0000, 0x00000000,
0x02800000, 0x000000FF, 0x00FF00FF, 0x00FF00FF};

const uint32_t vid640480pal50[] = {
0x11F50101, 0x4B6A01B0, 0x02F85640, 0x00010023,
0x00000024, 0x4D2B4D6D, 0x4D8A4D4C, 0x00435A4E,
0x00000000, 0x00435A4E, 0x00000000, 0x013C0144,
0x113901B1, 0x10010001, 0x00010001, 0x00010001,
0x00000000, 0x00000000, 0x28500100, 0x1AE771F0,
0x0DB4A574, 0x00C1188E, 0xC4C0CBE2, 0xFCECDECF,
0x13130F08, 0x00080C0F, 0x00FF0000, 0x00000000,
0x02800000, 0x000000FF, 0x00FF00FF, 0x00FF00FF};


static const SVideoMode videoModes[] =
{
  {320, 480, 320, 480, 32, cfR8G8B8},
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
  const uint32_t * videoRegs;

  videoRegs = vid640480pal50;
  //videoRegs = vid640480pal60;
  //videoRegs = vid640480ntsc;

  for(int i(0); i < 0x20; i++)
    REG_VI_BASE[i] = videoRegs[i];

  rmode_ = &TVMpal480IntDf;
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
  // Busy waiting for vblank
  while(REG_VI_HLINE != 200); //0x20A);
  while(REG_VI_HLINE <= 200); //0x20A);
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

    REG_VI_XFB1 = (uint32_t)pSurface_->p;
    REG_VI_XFB2 = (uint32_t)pSurface_->p;
  }
}
