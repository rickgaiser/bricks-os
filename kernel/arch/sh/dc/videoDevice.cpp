/*
 * Bricks-OS, Operating System for Game Consoles
 * Copyright (C) 2009 Maximus32 <Maximus32@bricks-os.org>
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


#include "videoDevice.h"
#include "kernel/2dRenderer.h"
#include "../../../../gl/softGLF.h"


#define DC_VIDEO_CABLE_VGA          0
#define DC_VIDEO_CABLE_RGB          2
#define DC_VIDEO_CABLE_COMPOSITE    3

#define DC_VIDEO_MODE_RGB555        0
#define DC_VIDEO_MODE_RGB565        1
#define DC_VIDEO_MODE_ARGB4444      2
#define DC_VIDEO_MODE_RGB888        3
#define DC_VIDEO_MODE_ARGB8888      4

#define DC_VIDEO_RESOLUTION_320_240 0
#define DC_VIDEO_RESOLUTION_640_240 1
#define DC_VIDEO_RESOLUTION_640_480 2


#define DEFAULT_VIDEO_MODE  videoModes[1] // 640x480x16
static const SVideoMode videoModes[] =
{
  {640, 480, 640, 480, 16, cfX1R5G5B5},
  {640, 480, 640, 480, 16, cfR5G6B5},
  {640, 480, 640, 480, 16, cfA4R4G4B4},
  {640, 480, 640, 480, 32, cfR8G8B8},
  {640, 480, 640, 480, 32, cfA8R8G8B8},
};
static const int videoModeCount(sizeof(videoModes) / sizeof(SVideoMode));


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Setup video registers to the desired video mode
//   cabletype  (0=VGA, 2=RGB, 3=Composite)
//   pixel mode (0=RGB555, 1=RGB565, 3=RGB888)
//   res        (0 = 320 x 240, 1 = 640 x 240, 2 = 640 x 480)
void
init_video(int cabletype, int mode, int res)
{
  volatile unsigned int *videobase=(volatile unsigned int *)(void*)0xa05f8000;
  static int bppshifttab[]= { 1,1,0,2 };
  int shift, lines, modulo, words_per_line, vpos;
  int laceoffset=0, voffset=0;
  unsigned int videoflags, attribs;
  unsigned int hvcounter = (res<2? 0x01060359 : 0x020c0359);

  mode &= 3;
  shift = bppshifttab[mode];

  videobase[8/4]=0;
  videobase[0x40/4]=0;

  // Select pixel clock and colour mode
  mode = (mode<<2)|1;
  lines = 240;
  if(!(cabletype&2))
  {
    // VGA mode
    if(res < 2)
      mode |= 2; // doublescan

    hvcounter = 0x020c0359;

    lines <<= 1;
    mode |= 0x800000; // fast pixel clock
  }
  videobase[0x44/4]=mode;

  // Set video base address.  Short fields will be offset by
  // 640 pixels, regardless of horizontal resolution.
  videobase[0x50/4]=0;
  videobase[0x54/4]=640<<shift;

  // Set screen size, modulo, and interlace flag
  videoflags = 1<<8;
  if(res==0)
    words_per_line=(320/4)<<shift;
  else
    words_per_line=(640/4)<<shift;
  modulo = 1;

  if(!(cabletype&2))
  {
    if(res==0)
      //VGA lores -> skip 320 pixels to keep modulo at 640 pixels
      modulo += words_per_line;
  }
  else
  {
    if(res!=1)
      // NTSC lores -> skip 320 pixels to keep modulo at 640 pixels
      // _or_ NTSC hires -> skip every other line due to interlace
      modulo += words_per_line;

    if(res==2)
      // interlace mode
      videoflags |= 1<<4;

    // enable NTSC
    videoflags |= 1<<6;
  }

  // Write screen size and modulo
  videobase[0x5c/4]=(((modulo<<10)+lines-1)<<10)+words_per_line-1;

  // Enable video (lace, NTSC)
  videobase[0xd0/4]=videoflags;

  // Screen and border position

  if(!(cabletype&2))
    // VGA
    voffset += 36;
  else
    voffset += 18;

  vpos=(voffset<<16)|(voffset+laceoffset);

  videobase[0xf0/4]=vpos;       // V start
  videobase[0xdc/4]=vpos+lines; // start and end border
  videobase[0xec/4]=0xa4;       // Horizontal pos
  videobase[0xd8/4]=hvcounter;  // HV counter
  videobase[0xd4/4]=0x007e0345; // Horizontal border

  // Select horizontal pixel doubling for lowres
  if(res==0)
    attribs=((22<<8)+1)<<8;
  else
    attribs=22<<16;
  videobase[0xe8/4]=attribs;

  // Set up vertical blank event
  vpos = 260;
  if(!(cabletype&2))
    vpos = 510;
  videobase[0xcc/4]=(0x21<<16)|vpos;

  // Select RGB/CVBS
  if(cabletype&1)
    mode = 3;
  else
    mode = 0;
  *(volatile unsigned int *)(void*)0xa0702c00 = mode << 8;

  return;
}

//---------------------------------------------------------------------------
// Check type of video cable connected
int
check_cable()
{
  volatile unsigned int *porta = (unsigned int *)0xff80002c;

  // PORT8 and PORT9 is input
  *porta = (*porta & ~0xf0000) | 0xa0000;

  // Return PORT8 and PORT9
  return ((*(volatile unsigned short *)(porta+1))>>8)&3;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CDCVideoDevice::CDCVideoDevice()
 : CAVideoDevice()
 , pSurface_(NULL)
 , pCurrentMode_(NULL)
 , bFirstFrameBufferFree_(true)
 , iFrameCount_(0)
{
}

//---------------------------------------------------------------------------
CDCVideoDevice::~CDCVideoDevice()
{
}

//---------------------------------------------------------------------------
void
CDCVideoDevice::listModes(const SVideoMode ** modes, int * modeCount)
{
  *modes = videoModes;
  *modeCount = videoModeCount;
}

//---------------------------------------------------------------------------
void
CDCVideoDevice::getCurrentMode(const SVideoMode ** mode)
{
  *mode = pCurrentMode_;
}

//---------------------------------------------------------------------------
void
CDCVideoDevice::getDefaultMode(const SVideoMode ** mode)
{
  *mode = &DEFAULT_VIDEO_MODE;
}

//---------------------------------------------------------------------------
void
CDCVideoDevice::setMode(const SVideoMode * mode)
{
  pCurrentMode_ = mode;

  switch(mode->format)
  {
    case cfX1R5G5B5: init_video(check_cable(), DC_VIDEO_MODE_RGB555,   DC_VIDEO_RESOLUTION_640_480); break;
    case cfR5G6B5:   init_video(check_cable(), DC_VIDEO_MODE_RGB565,   DC_VIDEO_RESOLUTION_640_480); break;
    case cfA4R4G4B4: init_video(check_cable(), DC_VIDEO_MODE_ARGB4444, DC_VIDEO_RESOLUTION_640_480); break;
    case cfR8G8B8:   init_video(check_cable(), DC_VIDEO_MODE_RGB888,   DC_VIDEO_RESOLUTION_640_480); break;
    case cfA8R8G8B8: init_video(check_cable(), DC_VIDEO_MODE_ARGB8888, DC_VIDEO_RESOLUTION_640_480); break;
    default:
      ;
  };
}

//---------------------------------------------------------------------------
void
CDCVideoDevice::getSurface(CSurface ** surface, int width, int height)
{
  CSurface * pSurface = new CSurface;

  //uint32_t size = pCurrentMode_->xpitch * pCurrentMode_->ypitch * (pCurrentMode_->bpp / 8);

  pSurface->mode = *pCurrentMode_;
  if(bFirstFrameBufferFree_ == true)
  {
    bFirstFrameBufferFree_ = false;
    pSurface->p    = (void *)(0xa5000000);
  }
  else
  {
    pSurface->p    = (void *)(0xa5400000);
  }

  *surface = pSurface;
}

//---------------------------------------------------------------------------
void
CDCVideoDevice::get2DRenderer(I2DRenderer ** renderer)
{
  *renderer = new C2DRenderer;
}

//---------------------------------------------------------------------------
void
CDCVideoDevice::get3DRenderer(I3DRenderer ** renderer)
{
#ifdef CONFIG_GL
  *renderer = new CSoftGLESFloat;
#else
  *renderer = NULL;
#endif
}

//---------------------------------------------------------------------------
uint32_t
CDCVideoDevice::getFrameNr()
{
  return iFrameCount_;
}

//---------------------------------------------------------------------------
uint32_t
CDCVideoDevice::waitVSync()
{
  return iFrameCount_;
}

//---------------------------------------------------------------------------
void
set_display_addr(void *ptr)
{
  volatile unsigned int *reg = (volatile unsigned int *)(void*)0xa05f8050;
  unsigned int addr = ((unsigned int)ptr)&0x007fffff;
  reg[0] = addr;
  reg[1] = addr+640*2;
}

//---------------------------------------------------------------------------
void
CDCVideoDevice::displaySurface(CSurface * surface)
{
  // FIXME: isr should update this, but we don't have interrupts
  iFrameCount_++;

  // Set new surface
  if(surface != NULL)
  {
    pSurface_ = surface;

    if((uint32_t)pSurface_->p == 0xa5000000)
    {
      set_display_addr(pSurface_->p);
    }
    else if((uint32_t)pSurface_->p == 0xa5400000)
    {
      set_display_addr(pSurface_->p);
    }
  }
}
