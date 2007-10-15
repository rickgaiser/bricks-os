#include "glesContext.h"
#include "asm/arch/macros.h"


  // NOTE: We are off by 1 here, but it's much faster this way
#define fpRGB(r,g,b) (0x8000 | \
                      (((b-1)>> 1) & 0x7c00) | \
                      (((g-1)>> 6) & 0x03e0) | \
                      (((r-1)>>11) & 0x001f))


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CGBAGLESContext::CGBAGLESContext()
 : CSoftGLESFixed()
{
}

//-----------------------------------------------------------------------------
CGBAGLESContext::~CGBAGLESContext()
{
}

//-----------------------------------------------------------------------------
void
CGBAGLESContext::glClear(GLbitfield mask)
{
  long iCount(viewportByteCount >> 1);

  if(mask & GL_COLOR_BUFFER_BIT)
  {
    unsigned short color(fpRGB(clClear.r, clClear.g, clClear.b));
    dmaFill16(color, renderSurface->p, iCount);
  }
  if(mask & GL_DEPTH_BUFFER_BIT)
    dmaFill32(depthClear_ * 100, zbuffer, iCount);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
IGLESContext *
getGLESContext()
{
  return new CGBAGLESContext;
}
