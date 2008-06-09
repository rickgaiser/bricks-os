#include "videoDevice.h"
#include "asm/arch/registers.h"
#include <gccore.h>


// NOTE:
//   Embedded FrameBuffer (EFB) is limited to 640x528!
//   eXternal FrameBuffer (XFB) needs to have the same width as EFB
//   Video Interface is limited to 480 lines for NTSC and 574 lines for PAL
// Resulting Maximum resolutions:
//   NTSC: (EFB) 640x480 --> (XFB) 640x480 --> (VI) 720x480
//   PAL:  (EFB) 640x528 --> (XFB) 640x574 --> (VI) 720x574


#define SCREEN_WIDTH           640
#define SCREEN_XPOS            (((720 - SCREEN_WIDTH) / 2) + 0)


// Video registers for different modes
#define VIDEO_REGISTERS_NTSC   0
#define VIDEO_REGISTERS_PAL    1
#define VIDEO_REGISTERS_MPAL   0
#define VIDEO_REGISTERS_PAL60  0
static const uint32_t videoRegs[6][32] =
{
  { // NTSC 480i60
    ((VI_VTR(480>>1, 6) << 16) |                             // Vertical Timing
      VI_DCR(VI_DCR_NTSC, VI_DCR_ENABLE)),                   // Display Configuration
    VI_HTR0(71, 105, 429),                                   // Horizontal Timing 0
    VI_HTR1_CREATE(413, 122, 64, SCREEN_XPOS, SCREEN_WIDTH), // Horizontal Timing 1  // 0x02EA5140
    VI_VT(3, 24),                                            // Odd Field Vertical Timing
    VI_VT(2, 25),                                            // Even Field Vertical Timing
    VI_BBI(520, 12, 520, 12),
    VI_BBI(519, 13, 519, 13),
    0x00435a4e,
    0x00000000, 0x00435a4e, 0x00000000, 0x00000000,
    0x110701ae, 0x10010001, 0x00010001, 0x00010001,
    0x00000000, 0x00000000,
    ((0x2850) << 16) | VI_HSR_ENABLE | VI_HSR_SCALE(640, SCREEN_WIDTH),
    0x1ae771f0,
    0x0db4a574, 0x00c1188e, 0xc4c0cbe2, 0xfcecdecf,
    0x13130f08, 0x00080c0f, 0x00ff0000, 0x00000000,
    0x02800000, 0x000000ff, 0x00ff00ff, 0x00ff00ff
  },
  { // PAL 576i50
    ((VI_VTR(574>>1, 5) << 16) |                             // Vertical Timing
      VI_DCR(VI_DCR_PAL, VI_DCR_ENABLE)),                    // Display Configuration
    VI_HTR0(75, 106, 432),                                   // Horizontal Timing 0
    VI_HTR1_CREATE(420, 132, 64, SCREEN_XPOS, SCREEN_WIDTH), // Horizontal Timing 1
    VI_VT(1, 35),                                            // Odd Field Vertical Timing
    VI_VT(0, 36),                                            // Even Field Vertical Timing
    VI_BBI(617, 11, 619, 13),
    VI_BBI(620, 10, 618, 12),
    0x00435a4e,
    0x00000000, 0x00435a4e, 0x00000000, 0x013c0144,
    0x113901b1, 0x10010001, 0x00010001, 0x00010001,
    0x00000000, 0x00000000,
    ((0x2850) << 16) | VI_HSR_ENABLE | VI_HSR_SCALE(640, SCREEN_WIDTH),
    0x1ae771f0,
    0x0db4a574, 0x00c1188e, 0xc4c0cbe2, 0xfcecdecf,
    0x13130f08, 0x00080c0f, 0x00ff0000, 0x00000000,
    0x02800000, 0x000000ff, 0x00ff00ff, 0x00ff00ff
  },
};

// NOTE: The sizes used here are for XFB
#define DEFAULT_VIDEO_MODE_NTSC   videoModes[0]
#define DEFAULT_VIDEO_MODE_PAL    videoModes[2]
#define DEFAULT_VIDEO_MODE_MPAL   videoModes[0]
#define DEFAULT_VIDEO_MODE_PAL60  videoModes[0]
static const SVideoMode videoModes[] =
{
  // NTSC
  {640, 480, 640, 480, 32, cfA8R8G8B8}, // RGB Surface
  {320, 480, 320, 480, 32, cfR8G8B8},   // Native Surface
  // PAL (Max. XFB sizes)
  {640, 574, 640, 574, 32, cfA8R8G8B8}, // RGB Surface
  {320, 574, 320, 574, 32, cfR8G8B8},   // Native Surface
  // PAL (Max. EFB sizes)
  {640, 528, 640, 528, 32, cfA8R8G8B8}, // RGB Surface
  {320, 528, 320, 528, 32, cfR8G8B8},   // Native Surface
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
CNGC2DRenderer::CNGC2DRenderer(CNGCVideoDevice * dev)
 : C2DRenderer()
 , pDev_(dev)
{
}

//---------------------------------------------------------------------------
CNGC2DRenderer::~CNGC2DRenderer()
{
}

//---------------------------------------------------------------------------
void
CNGC2DRenderer::flush()
{
  if(pSurface_->mode.width == 640)
  {
    // BitBlt RGB Surface to Native Surface
    pDev_->flush(pSurface_);
  }
}

//---------------------------------------------------------------------------
void
CNGC2DRenderer::setColor(color_t rgb)
{
  C2DRenderer::setColor(rgb);

  // Override color if mode is YCbYCr (hidden as a cfR8G8B8 surface)
  if(pSurface_->mode.width == 320)
    fmtColor_ = rgb2yuyv(color_.r, color_.g, color_.b);
}

//---------------------------------------------------------------------------
void
CNGC2DRenderer::setColor(uint8_t r, uint8_t g, uint8_t b)
{
  C2DRenderer::setColor(r, g, b);

  // Override color if mode is YCbYCr (hidden as a cfR8G8B8 surface)
  if(pSurface_->mode.width == 320)
    fmtColor_ = rgb2yuyv(color_.r, color_.g, color_.b);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CNGC3DRenderer::CNGC3DRenderer(CNGCVideoDevice * dev)
 : CSoftGLESFloat()
 , pDev_(dev)
{
}

//---------------------------------------------------------------------------
CNGC3DRenderer::~CNGC3DRenderer()
{
}

//---------------------------------------------------------------------------
void
CNGC3DRenderer::flush()
{
  if(renderSurface->mode.width == 640)
  {
    // BitBlt RGB Surface to Native Surface
    pDev_->flush(renderSurface);
  }
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CNGCVideoDevice::CNGCVideoDevice()
 : CAVideoDevice()
 , pSurface_(NULL)
 , pNativeSurface_(NULL)
 , pCurrentMode_(NULL)
 , iFrameCount_(0)
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
  switch(BIOS_TV_MODE)
  {
    case BTM_NTSC:  *mode = &DEFAULT_VIDEO_MODE_NTSC;  break;
    case BTM_MPAL:  *mode = &DEFAULT_VIDEO_MODE_MPAL;  break;
    case BTM_PAL60: *mode = &DEFAULT_VIDEO_MODE_PAL60; break;
    case BTM_PAL:
    default:        *mode = &DEFAULT_VIDEO_MODE_PAL;
  };
}

//---------------------------------------------------------------------------
void
CNGCVideoDevice::setMode(const SVideoMode * mode)
{
  pCurrentMode_ = mode;

  if(mode->height == 480) // NTSC
  {
    // Copy to registers
    for(int i(0); i < 0x20; i++)
      REG_VI_BASE[i] = videoRegs[VIDEO_REGISTERS_NTSC][i];

    // Set horizontal registers (centered, width of 640)
    setHorizontal((720 - 640) / 2, 640);
    // Set vertical registers (centered, height of 480)
    setVertical(0, 480);
    // Set interlacing
    setInterlaced(true);
  }
  else // PAL
  {
    // Copy to registers
    for(int i(0); i < 0x20; i++)
      REG_VI_BASE[i] = videoRegs[VIDEO_REGISTERS_PAL][i];

    // Set horizontal registers (centered, width of 640)
    setHorizontal((720 - 640) / 2, 640);
    // Set vertical registers (centered, height of 574)
    setVertical(1, 574);
    // Set interlacing
    setInterlaced(true);
  }
}

//---------------------------------------------------------------------------
//  32 byte aligned?
// 512 byte aligned?
#define mallocSurface(size) \
  MEM_K0_TO_K1(((uint32_t)new uint8_t[size + 0x1f]) & (~0x1f))
//  MEM_K0_TO_K1(((uint32_t)new uint8_t[size + 0x1ff]) & (~0x1ff))
//---------------------------------------------------------------------------
void
CNGCVideoDevice::getSurface(CSurface ** surface, int width, int height)
{
  if(width == 320) // Native Surface
  {
    CSurface * pSurface = new CSurface;
    pSurface->mode  = *pCurrentMode_;
    pSurface->p     = mallocSurface(width * height * 4);
    *surface = pSurface;
  }
  else if(width == 640) // RGB Surface
  {
    CSurface * pSurface = new CSurface;
    pSurface->mode  = *pCurrentMode_;
    pSurface->p     = mallocSurface(width * height * 4);
    if(pNativeSurface_ == NULL)
      pNativeSurface_ = mallocSurface(width * height * 2);
    *surface = pSurface;
  }
  else // Texture?
  {
  }
}

//---------------------------------------------------------------------------
void
CNGCVideoDevice::get2DRenderer(I2DRenderer ** renderer)
{
  *renderer = new CNGC2DRenderer(this);
}

//---------------------------------------------------------------------------
void
CNGCVideoDevice::get3DRenderer(I3DRenderer ** renderer)
{
  *renderer = new CNGC3DRenderer(this);
}

//---------------------------------------------------------------------------
uint32_t
CNGCVideoDevice::getFrameNr()
{
  return iFrameCount_;
}

//---------------------------------------------------------------------------
uint32_t
CNGCVideoDevice::waitVSync()
{
  // Busy waiting for vblank
  while(REG_VI_HLINE != 200); //0x20A);
  while(REG_VI_HLINE <= 200); //0x20A);

  return iFrameCount_;
}

//---------------------------------------------------------------------------
void
CNGCVideoDevice::displaySurface(CSurface * surface)
{
  // Always VSync, even if the frame is not new.
  if(vSync_ == true)
    waitVSync();

  // FIXME: isr should update this, but we don't have interrupts
  iFrameCount_++;

  // Set new surface if it changed
  if((surface != NULL) && (surface != pSurface_))
  {
    pSurface_ = surface;

    if(pSurface_->mode.width == 320)
    {
      REG_VI_XFB1 = (uint32_t)pSurface_->p;
      REG_VI_XFB2 = ((uint32_t)pSurface_->p) + (pSurface_->mode.width * 4);
    }
    else
    {
      REG_VI_XFB1 = (uint32_t)pNativeSurface_;
      REG_VI_XFB2 = ((uint32_t)pNativeSurface_) + (pSurface_->mode.width * 2);
    }
  }
}

//---------------------------------------------------------------------------
void
CNGCVideoDevice::flush(CSurface * surface)
{
  if((surface == pSurface_) && (surface->mode.width == 640))
  {
    uint32_t * source = (uint32_t *)surface->p;
    uint32_t * dest   = (uint32_t *)pNativeSurface_;
    SColor     pixel1;
    SColor     pixel2;

    uint32_t height = surface->mode.height;
    while(height--)
    {
      uint32_t width = surface->mode.width >> 1;
      while(width--)
      {
        pixel1.color = *source++;
        pixel2.color = *source++;

        *dest++ = rgbrgb2yuyv(pixel1.r,
                              pixel1.g,
                              pixel1.b,
                              pixel2.r,
                              pixel2.g,
                              pixel2.b);
      }
    }
  }
}

//---------------------------------------------------------------------------
void
CNGCVideoDevice::setHorizontal(uint16_t xoffset, uint16_t width)
{
  // The maximum width is 720
  if(width > 720)
    width = 720;
  // The minimum width is 640
  if(width < 640)
    width = 640;

  if((xoffset + width) > 720)
    xoffset = 720 - width;

  // Set the horizontal register
  if(pCurrentMode_->height == 480) // NTSC
    REG_VI_HTR1 = VI_HTR1_CREATE(413, 122, 64, xoffset, width);
  else // PAL
    REG_VI_HTR1 = VI_HTR1_CREATE(420, 132, 64, xoffset, width);

  // The maximum framebuffer width is 640, so scale if it has to be larger
  if(width > 640)
    REG_VI_HSR = VI_HSR_ENABLE | VI_HSR_SCALE(640, width);
}

//---------------------------------------------------------------------------
void
CNGCVideoDevice::setVertical(uint16_t yoffset, uint16_t height)
{
  // FIXME
  bool interlaced = true;
  uint8_t  equ;
  uint16_t maxHeight;
  uint8_t  oprb, eprb, opob, epob;
  if(pCurrentMode_->height == 480) // NTSC
  {
    maxHeight = 480;
    equ       =  12;
    oprb      =  24;
    eprb      =  25;
    opob      =   3;
    epob      =   2;
  }
  else // PAL
  {
    maxHeight = 574;
    equ       =  10;
    oprb      =  35;
    eprb      =  36;
    opob      =   1;
    epob      =   0;
  }

  // The maximum height is 480
  if(height > maxHeight)
    height = maxHeight;

  if((yoffset + height) > maxHeight)
    yoffset = maxHeight - height;

  // Calculate Pre-Blanking
  uint16_t oddPreBlanking   = oprb + (yoffset);
  uint16_t evenPreBlanking  = eprb + (yoffset);
  // Calculate Post-Blanking
  uint16_t oddPostBlanking  = opob + (maxHeight - height - yoffset);
  uint16_t evenPostBlanking = epob + (maxHeight - height - yoffset);

  if(interlaced == true)
  {
    REG_VI_VTR = VI_VTR(height/2, equ/2);
    REG_VI_VTO = (yoffset % 2) ? VI_VT(evenPostBlanking, evenPreBlanking) : VI_VT(oddPostBlanking,  oddPreBlanking);
    REG_VI_VTE = (yoffset % 2) ? VI_VT(oddPostBlanking,  oddPreBlanking)  : VI_VT(evenPostBlanking, evenPreBlanking);
  }
  else
  {
    REG_VI_VTR = VI_VTR(height, equ);
    REG_VI_VTO = (yoffset % 2) ? VI_VT(2 * evenPostBlanking, 2 * evenPreBlanking) : VI_VT(2 * oddPostBlanking,  2 * oddPreBlanking);
    REG_VI_VTE = (yoffset % 2) ? VI_VT(2 * oddPostBlanking,  2 * oddPreBlanking)  : VI_VT(2 * evenPostBlanking, 2 * evenPreBlanking);
  }
}

//---------------------------------------------------------------------------
void
CNGCVideoDevice::setInterlaced(bool interlaced)
{
  if(pCurrentMode_->height == 480) // NTSC
  {
    if(interlaced == true)
    {
      REG_VI_VTR = VI_VTR(pCurrentMode_->height/2, 12/2);
      REG_VI_DCR = VI_DCR(VI_DCR_NTSC, VI_DCR_ENABLE);
    }
    else
    {
      REG_VI_VTR = VI_VTR(pCurrentMode_->height, 12);
      REG_VI_DCR = VI_DCR(VI_DCR_NTSC, VI_DCR_ENABLE | VI_DCR_PROGRESSIVE);
    }
  }
  else // PAL
  {
    if(interlaced == true)
    {
      REG_VI_VTR = VI_VTR(pCurrentMode_->height/2, 10/2);
      REG_VI_DCR = VI_DCR(VI_DCR_PAL, VI_DCR_ENABLE);
    }
    else
    {
      REG_VI_VTR = VI_VTR(pCurrentMode_->height, 10);
      REG_VI_DCR = VI_DCR(VI_DCR_PAL, VI_DCR_ENABLE | VI_DCR_PROGRESSIVE);
    }
  }

  if(interlaced == true)
    REG_VI_CLK = VI_CLK_27MHZ;
  else
    REG_VI_CLK = VI_CLK_54MHZ;
}
