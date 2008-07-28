#include "debugScreen.h"
#include <gccore.h>
#include <stdio.h>
#include <ogcsys.h>


static void *xfb = NULL;
GXRModeObj *rmode;


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

  switch(VIDEO_GetCurrentTvMode())
  {
      case VI_NTSC:
          rmode = &TVNtsc480IntDf;
          break;
      case VI_PAL:
          rmode = &TVPal528IntDf;
          break;
      case VI_MPAL:
          rmode = &TVMpal480IntDf;
          break;
      default:
          rmode = &TVNtsc480IntDf;
          break;
  }

  xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

  VIDEO_Configure(rmode);
  VIDEO_SetNextFramebuffer(xfb);
  VIDEO_SetBlack(FALSE);
  VIDEO_Flush();
  VIDEO_WaitVSync();
  if(rmode->viTVMode&VI_NON_INTERLACE)
    VIDEO_WaitVSync();

  console_init(xfb,20,64,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*2);

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
