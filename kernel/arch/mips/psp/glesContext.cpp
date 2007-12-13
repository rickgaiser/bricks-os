#include "glesContext.h"


//-----------------------------------------------------------------------------
IGLESContext *
getGLESContext()
{
  return new CSoftGLESFloat;
}
