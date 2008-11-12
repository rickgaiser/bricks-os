/*
 * Bricks-OS, Operating System for Game Consoles
 * Copyright (C) 2008 Maximus32 <Maximus32@bricks-os.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA
 */


#include "vesa.h"
#include "mmap.h"
#include "asm/hal.h"
#include "kernel/debug.h"
#include "stddef.h"
#include "../../../../gl/softGLF.h"


const char * sVBEFunction[] =
{
  "Return VBE Controller Information",
  "Return VBE Mode Information",
  "Set VBE Mode",
  "Return Current VBE Mode",
  "Save/Restore state",
  "Display window control",
  "Set/Get logical scanline control",
  "Set/Get display start",
  "Set/Get DAC palette format",
  "Set/Get palette data",
  "Return VBE protected mode interface",
  "Get/Set pixel clock"
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
const uint32_t vmode1_count(sizeof(vesaVideoModes1) / sizeof(SVideoMode));

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
const uint32_t vmode2_count(sizeof(vesaVideoModes2) / sizeof(SVideoMode));

//---------------------------------------------------------------------------
CVesaVideoDevice::CVesaVideoDevice()
 : CAVideoDevice()
 , pSurface_(NULL)
 , iFrameCount_(0)
 , pCurrentMode_(NULL)
 , pDefaultMode_(NULL)
#ifdef CONFIG_MMU
 , v86thr_(0) // FIXME! What addres space do we use?
#else
 , v86thr_()
#endif
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
    printk("Video Memory %dKiB\n", pInfo_->vid_mem_size * 64);

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
  uint32_t vesaMode(0);

  if(mode->bpp >= 16)
  {
    // Look for 16/24 bit mode
    for(uint32_t i(0); i < vmode2_count; i++)
    {
      if((mode->width  == vesaVideoModes2[i].width) &&
         (mode->height == vesaVideoModes2[i].height) &&
         (mode->format == vesaVideoModes2[i].format))
      {
        vesaMode = 0x110 + i;
        break;
      }
    }
  }
  else
  {
    // Look for 4/8 bit mode
  }

  if(vesaMode == 0)
  {
    printk("VESA: Invalid mode: %dx%dx%d\n", mode->width, mode->height, mode->bpp);
    return;
  }

  printk("VESA: Setting mode to: %dx%dx%d\n", mode->width, mode->height, mode->bpp);

  // Get mode information
  v86thr_.pTSS_->ecx = vesaMode;
  v86thr_.pTSS_->es  = (uint32_t)pCurrentVBEMode_ >> 4;
  v86thr_.pTSS_->edi = (uint32_t)pCurrentVBEMode_ &  0xf;
  if(vbeCall(0x01) == true)
  {
    // Set mode
    v86thr_.pTSS_->ebx = (1 << 14) | vesaMode;
    if(vbeCall(0x02) == true)
    {
      printk("VESA: Mode set to: %dx%dx%d\n", mode->width, mode->height, mode->bpp);
      pCurrentMode_ = mode;
    }
    else
      printk("VESA: Unable to set mode\n");
  }
  else
    printk("VESA: Unable to get mode information\n");
}

//---------------------------------------------------------------------------
static uint32_t bytesUsed(0);
void
CVesaVideoDevice::getSurface(CSurface ** surface, int width, int height)
{
  CSurface * pSurface = new CSurface;

  pSurface->mode = *pCurrentMode_;

  if(((uint32_t)width == pCurrentMode_->width) && ((uint32_t)height == pCurrentMode_->height))
  {
    pSurface->p           = (void *)(pCurrentVBEMode_->physBasePtr + bytesUsed);

    // Add the bytes we just used
    bytesUsed += pCurrentMode_->xpitch * pCurrentMode_->ypitch * (pCurrentMode_->bpp / 8);
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
  // FIXME
  iFrameCount_++;

  // Set new surface if it changed
  if((surface != NULL) && (surface != pSurface_))
  {
    pSurface_ = surface;

/*
    // Subfunction 0
    v86thr_.pTSS_->ebx = 0x00000000;
    // First pixel in scanline
    v86thr_.pTSS_->ecx = 0;
    // First scanline
    v86thr_.pTSS_->edx = 0;

    // Subfunction 2
    v86thr_.pTSS_->ebx = 0x00000002;
    // Byte offset into video memory
    v86thr_.pTSS_->ecx = ((uint32_t)surface->p) - pCurrentVBEMode_->physBasePtr;

    if(vbeCall(0x07) == false)
      panic("swap failed");
*/
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
//    printk("VBE \"%s\" function failed (%d)\n", sVBEFunction[function], (v86thr_.pTSS_->eax & 0xff00) >> 8);
    bRetVal = false;
  }

  return bRetVal;
}
