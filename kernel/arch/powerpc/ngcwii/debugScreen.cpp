#include "debugScreen.h"
#include <gccore.h>
#include <stdio.h>
#include <ogcsys.h>


static void *xfb = NULL;
GXRModeObj *rmode;


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// NOTE: There are compatibility issues when:
//  1 - Using libogc-20080602
//  2 - Loading with HBC beta9
// I can't figure out why, but I reverted back to libogc-20080228.
extern "C" GXRModeObj *
VIDEO_GetPreferredMode(GXRModeObj * mode)
{
  switch(VIDEO_GetCurrentTvMode())
  {
    case VI_NTSC: return &TVNtsc480IntDf;
    case VI_PAL:  return &TVPal528IntDf;
    case VI_MPAL: return &TVMpal480IntDf;
    default:      return &TVNtsc480IntDf;
  }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
CNGCDebugScreen::CNGCDebugScreen()
{
  // Don't use constructor, use init function instead
}

// -----------------------------------------------------------------------------
CNGCDebugScreen::~CNGCDebugScreen()
{
}

// -----------------------------------------------------------------------------
int
CNGCDebugScreen::init()
{
  VIDEO_Init();

  rmode = VIDEO_GetPreferredMode(NULL);

  xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
  console_init(xfb,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);

  VIDEO_Configure(rmode);
  VIDEO_SetNextFramebuffer(xfb);
  VIDEO_SetBlack(FALSE);
  VIDEO_Flush();
  VIDEO_WaitVSync();
  if(rmode->viTVMode&VI_NON_INTERLACE)
    VIDEO_WaitVSync();

  return 0;
}

// -----------------------------------------------------------------------------
ssize_t
CNGCDebugScreen::write(const void * buffer, size_t size, bool block)
{
  for(size_t i(0); i < size; i++)
    printf("%c", ((const char *)buffer)[i]);

  return 0;
}
