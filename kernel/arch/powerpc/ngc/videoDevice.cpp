#include "videoDevice.h"
#include "asm/arch/registers.h"
#include <gccore.h>


// Video registers for different modes
static const uint32_t videoRegs[6][32] =
{
  { // NTSC 480i60
    0x0f060001, 0x476901ad, 0x02ea5140, 0x00030018,
    0x00020019, 0x410c410c, 0x40ed40ed, 0x00435a4e,
    0x00000000, 0x00435a4e, 0x00000000, 0x00000000,
    0x110701ae, 0x10010001, 0x00010001, 0x00010001,
    0x00000000, 0x00000000, 0x28500100, 0x1ae771f0,
    0x0db4a574, 0x00c1188e, 0xc4c0cbe2, 0xfcecdecf,
    0x13130f08, 0x00080c0f, 0x00ff0000, 0x00000000,
    0x02800000, 0x000000ff, 0x00ff00ff, 0x00ff00ff
  },
  { // PAL 576i50
    0x11f50101, 0x4b6a01b0, 0x02f85640, 0x00010023,
    0x00000024, 0x4d2b4d6d, 0x4d8a4d4c, 0x00435a4e,
    0x00000000, 0x00435a4e, 0x00000000, 0x013c0144,
    0x113901b1, 0x10010001, 0x00010001, 0x00010001,
    0x00000000, 0x00000000, 0x28500100, 0x1ae771f0,
    0x0db4a574, 0x00c1188e, 0xc4c0cbe2, 0xfcecdecf,
    0x13130f08, 0x00080c0f, 0x00ff0000, 0x00000000,
    0x02800000, 0x000000ff, 0x00ff00ff, 0x00ff00ff
  },
  { // DEBUG
    0
  },
  { // DEBUG PAL
    0
  },
  { // MPAL 480i60
    0
  },
  { // PAL60 480i60
    0x0f060001, 0x476901ad, 0x02ea5140, 0x00030018,
    0x00020019, 0x410c410c, 0x40ed40ed, 0x00435a4e,
    0x00000000, 0x00435a4e, 0x00000000, 0x00050176,
    0x110701ae, 0x10010001, 0x00010001, 0x00010001,
    0x00000000, 0x00000000, 0x28500100, 0x1ae771f0,
    0x0db4a574, 0x00c1188e, 0xc4c0cbe2, 0xfcecdecf,
    0x13130f08, 0x00080c0f, 0x00ff0000, 0x00000000,
    0x02800000, 0x000000ff, 0x00ff00ff, 0x00ff00ff
  },
};

#define DEFAULT_VIDEO_MODE_PAL  videoModes[1] // PAL:  640x576x32
#define DEFAULT_VIDEO_MODE_NTSC videoModes[0] // NTSC: 640x480x32
static const SVideoMode videoModes[] =
{
  {320, 480, 320, 480, 32, cfR8G8B8}, // NTSC 480i60
  {320, 576, 320, 576, 32, cfR8G8B8}, // PAL  576i50
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
CNGCVideoDevice::getCurrentMode(const SVideoMode ** mode)
{
  *mode = pCurrentMode_;
}

//---------------------------------------------------------------------------
void
CNGCVideoDevice::getDefaultMode(const SVideoMode ** mode)
{
  if(BIOS_TV_MODE == BTM_PAL)
    *mode = &DEFAULT_VIDEO_MODE_PAL;
  else
    *mode = &DEFAULT_VIDEO_MODE_NTSC;
}

//---------------------------------------------------------------------------
void
CNGCVideoDevice::setMode(const SVideoMode * mode)
{
  const uint32_t * pRegs;

  if(mode->height == 480)
  {
    // NTSC
    pRegs  = videoRegs[BTM_NTSC];
  }
  else
  {
    // PAL
    pRegs = videoRegs[BTM_PAL];
  }

  for(int i(0); i < 0x20; i++)
    REG_VI_BASE[i] = pRegs[i];

  pCurrentMode_ = mode;
}

//---------------------------------------------------------------------------
void
CNGCVideoDevice::getSurface(CSurface ** surface, ESurfaceType type)
{
  switch(type)
  {
    case stSCREEN:
    case stOFFSCREEN:
    {
      CSurface * pSurface = new CSurface;
      pSurface->mode = *pCurrentMode_;
      pSurface->p = MEM_K0_TO_K1(SYS_AllocateFramebuffer(&TVPal574IntDfScale)); // FIXME
      *surface = pSurface;
      break;
    }
    default:
    {
      *surface = 0;
    }
  };
}

//---------------------------------------------------------------------------
void
CNGCVideoDevice::get2DRenderer(I2DRenderer ** renderer)
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
