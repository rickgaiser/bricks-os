#include "glesContext.h"


//-----------------------------------------------------------------------------
IGLESRenderer *
getGLESContext()
{
  return new CSoftGLESFloat;
}
