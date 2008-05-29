#include "vesa.h"
#include "mmap.h"
#include "kernel/debug.h"
#include "stddef.h"


const char * sVBEFunction[] =
{
  "Return VBE Controller Information",
  "Return VBE Mode Information",
  "Set VBE Mode",
  "Return Current VBE Mode",
};


//---------------------------------------------------------------------------
CVesaVideoDevice::CVesaVideoDevice()
 : CAVideoDevice()
 , pSurface_(NULL)
 , iFrameCount_(0)
 , pCurrentMode_(NULL)
 , v86thr_()
{
  pInfo_ = (SVBEInfo *)physAllocPageLow();
  pInfo_->sig[0] = 'V';
  pInfo_->sig[1] = 'B';
  pInfo_->sig[2] = 'E';
  pInfo_->sig[3] = '2';

  v86thr_.pTSS_->es  = (uint32_t)pInfo_ >> 4;
  v86thr_.pTSS_->edi = (uint32_t)pInfo_ &  0xf;

  if(vbeCall(0x00) == true)
    printk("VBE version %d.%d\n", pInfo_->ver_major, pInfo_->ver_minor);
}

//---------------------------------------------------------------------------
CVesaVideoDevice::~CVesaVideoDevice()
{
}

//---------------------------------------------------------------------------
void
CVesaVideoDevice::listModes(const SVideoMode ** modes, int * modeCount)
{
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
}

//---------------------------------------------------------------------------
void
CVesaVideoDevice::setMode(const SVideoMode * mode)
{
  pCurrentMode_ = mode;
}

//---------------------------------------------------------------------------
void
CVesaVideoDevice::getSurface(CSurface ** surface, int width, int height)
{
  *surface = NULL;
}

//---------------------------------------------------------------------------
void
CVesaVideoDevice::get2DRenderer(I2DRenderer ** renderer)
{
  *renderer = NULL;
}

//---------------------------------------------------------------------------
void
CVesaVideoDevice::get3DRenderer(I3DRenderer ** renderer)
{
  *renderer = NULL;
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
void
CVesaVideoDevice::bitBlt(CSurface * dest, int dx, int dy, int w, int h, CSurface * source, int sx, int sy)
{
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
