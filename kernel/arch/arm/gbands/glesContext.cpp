#include "glesContext.h"
#include "asm/arch/macros.h"


#define fpRGB(r,g,b) (0x8000 | \
                      (((b*255) >>  9) & 0x7c00) | \
                      (((g*255) >> 14) & 0x03e0) | \
                      (((r*255) >> 19) & 0x001f))


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
    dmaFill32(0xffffffff, zbuffer, iCount);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
IGLESContext *
getGLESContext()
{
  return new CGBAGLESContext;
}
