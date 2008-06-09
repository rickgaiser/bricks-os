#include "vesa.h"
#include "mmap.h"
#include "hal.h"
#include "kernel/debug.h"
#include "stddef.h"
#include "../../../../gl/softGLF.h"


const char * sVBEFunction[] =
{
  "Return VBE Controller Information",
  "Return VBE Mode Information",
  "Set VBE Mode",
  "Return Current VBE Mode",
};

// 4/8 bit modes
static const SVideoMode vesaVideoModes1[] =
{
  { 640,  400,  640,  400,  8, cfP8}, // 0x100
  { 640,  480,  640,  480,  8, cfP8}, // 0x101
  { 800,  600,  800,  600,  4, cfP4}, // 0x102
  { 800,  600,  800,  600,  8, cfP8}, // 0x103
  {1024,  768, 1024,  768,  4, cfP4}, // 0x104
  {1024,  768, 1024,  768,  8, cfP8}, // 0x105
  {1280, 1024, 1280, 1024,  4, cfP4}, // 0x106
  {1280, 1024, 1280, 1024,  8, cfP8}, // 0x107
};

// 16/24 bit modes
static const SVideoMode vesaVideoModes2[] =
{
  { 640,  480,  640,  480, 16, cfX1R5G5B5}, // 0x110
  { 640,  480,  640,  480, 16, cfR5G6B5},   // 0x111
  { 640,  480,  640,  480, 24, cfR8G8B8},   // 0x112
  { 800,  600,  800,  600, 16, cfX1R5G5B5}, // 0x113
  { 800,  600,  800,  600, 16, cfR5G6B5},   // 0x114
  { 800,  600,  800,  600, 24, cfR8G8B8},   // 0x115
  {1024,  768, 1024,  768, 16, cfX1R5G5B5}, // 0x116
  {1024,  768, 1024,  768, 16, cfR5G6B5},   // 0x117
  {1024,  768, 1024,  768, 24, cfR8G8B8},   // 0x118
  {1280, 1024, 1280, 1024, 16, cfX1R5G5B5}, // 0x119
  {1280, 1024, 1280, 1024, 16, cfR5G6B5},   // 0x11a
  {1280, 1024, 1280, 1024, 24, cfR8G8B8},   // 0x11b
};

//---------------------------------------------------------------------------
CVesaVideoDevice::CVesaVideoDevice()
 : CAVideoDevice()
 , pSurface_(NULL)
 , iFrameCount_(0)
 , pCurrentMode_(NULL)
 , pDefaultMode_(NULL)
 , v86thr_()
 , pMode_(NULL)
 , iModeCount_(0)
{
  pInfo_ = (SVBEInfo *)physAllocPageLow();
  pCurrentVBEMode_ = (SVBEMode *)(&((uint8_t *)pInfo_)[512]);
  pInfo_->sig[0] = 'V';
  pInfo_->sig[1] = 'B';
  pInfo_->sig[2] = 'E';
  pInfo_->sig[3] = '2';

  v86thr_.pTSS_->es  = (uint32_t)pInfo_ >> 4;
  v86thr_.pTSS_->edi = (uint32_t)pInfo_ &  0xf;

  if(vbeCall(0x00) == true)
  {
    printk("VBE version %d.%d\n", pInfo_->ver_major, pInfo_->ver_minor);

    // Allocate mode buffer
    // FIXME: How big?
    pMode_ = new SVideoMode[40];
    printk("Supported video modes:\n");
    pModeNrs_ = (int16_t *)from_v86_addr(pInfo_->mode_list_seg, pInfo_->mode_list_off);
    for(int i(0); pModeNrs_[i] != -1 ; i++)
    {
      if((pModeNrs_[i] >= 0x100) && (pModeNrs_[i] <= 0x107))
      {
        const SVideoMode & mode = vesaVideoModes1[pModeNrs_[i] - 0x100];
        printk(" - 0x%x: %dx%dx%d\n", pModeNrs_[i], mode.width, mode.height, mode.bpp);
        pMode_[iModeCount_] = mode;
        iModeCount_++;
      }
      else if((pModeNrs_[i] >= 0x110) && (pModeNrs_[i] <= 0x11b))
      {
        const SVideoMode & mode = vesaVideoModes2[pModeNrs_[i] - 0x110];
        printk(" - 0x%x: %dx%dx%d\n", pModeNrs_[i], mode.width, mode.height, mode.bpp);
        pMode_[iModeCount_] = mode;
        pDefaultMode_ = &pMode_[iModeCount_];
        iModeCount_++;
      }
//      else
//      {
//        printk(" - 0x%x: unknown\n");
//      }
    }
  }
}

//---------------------------------------------------------------------------
CVesaVideoDevice::~CVesaVideoDevice()
{
}

//---------------------------------------------------------------------------
void
CVesaVideoDevice::listModes(const SVideoMode ** modes, int * modeCount)
{
  *modes = pMode_;
  *modeCount = iModeCount_;
}

//---------------------------------------------------------------------------
void
CVesaVideoDevice::getCurrentMode(const SVideoMode ** mode)
{
  *mode = pCurrentMode_;
}

//---------------------------------------------------------------------------
void
CVesaVideoDevice::getDefaultMode(const SVideoMode ** mode)
{
  *mode = pDefaultMode_;
}

//---------------------------------------------------------------------------
void
CVesaVideoDevice::setMode(const SVideoMode * mode)
{
  printk("Setting mode set to: %dx%dx%d\n", mode->width, mode->height, mode->bpp);

  // Get mode information
  v86thr_.pTSS_->ecx = 0x11a; // FIXME
  v86thr_.pTSS_->es  = (uint32_t)pCurrentVBEMode_ >> 4;
  v86thr_.pTSS_->edi = (uint32_t)pCurrentVBEMode_ &  0xf;
  if(vbeCall(0x01) == true)
  {
    // Set mode
    v86thr_.pTSS_->ebx = (1 << 14) | 0x11a; // FIXME
    if(vbeCall(0x02) == true)
    {
      pCurrentMode_ = mode;
    }
  }
}

//---------------------------------------------------------------------------
void
CVesaVideoDevice::getSurface(CSurface ** surface, int width, int height)
{
  CSurface * pSurface = new CSurface;

  pSurface->mode = *pCurrentMode_;

  if(((uint32_t)width == pCurrentMode_->width) && ((uint32_t)height == pCurrentMode_->height))
  {
    pSurface->p           = (void *)pCurrentVBEMode_->physBasePtr;
  }
  else
  {
    pSurface->mode.xpitch = width;
    pSurface->mode.ypitch = height;
    pSurface->mode.width  = width;
    pSurface->mode.height = height;
    pSurface->p           = new uint8_t[width * height * (pCurrentMode_->bpp / 8)];
  }

  *surface = pSurface;
}

//---------------------------------------------------------------------------
void
CVesaVideoDevice::get2DRenderer(I2DRenderer ** renderer)
{
  *renderer = new C2DRenderer;
}

//---------------------------------------------------------------------------
void
CVesaVideoDevice::get3DRenderer(I3DRenderer ** renderer)
{
  *renderer = new CSoftGLESFloat;
}

//---------------------------------------------------------------------------
uint32_t
CVesaVideoDevice::getFrameNr()
{
  return iFrameCount_;
}

//---------------------------------------------------------------------------
uint32_t
CVesaVideoDevice::waitVSync()
{
  return iFrameCount_;
}

//---------------------------------------------------------------------------
void
CVesaVideoDevice::displaySurface(CSurface * surface)
{
  // Always VSync, even if the frame is not new.
  if(vSync_ == true)
    waitVSync();

  // FIXME
  iFrameCount_++;

  // Set new surface if it changed
  if((surface != NULL) && (surface != pSurface_))
  {
    pSurface_ = surface;
  }
}

//---------------------------------------------------------------------------
bool
CVesaVideoDevice::vbeCall(uint8_t function)
{
  bool bRetVal(true);

  v86thr_.pTSS_->eax = 0x4f00 + function;
  v86thr_.interrupt(0x10);

  if((v86thr_.pTSS_->eax & 0xff) != 0x4f)
  {
//    printk("VBE \"%s\" function not supported\n", sVBEFunction[function]);
    bRetVal = false;
  }

  if((v86thr_.pTSS_->eax & 0xff00) != 0)
  {
//    printk("VBE \"%s\" function failed\n", sVBEFunction[function]);
    bRetVal = false;
  }

  return bRetVal;
}
