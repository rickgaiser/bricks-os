#ifndef GBA_GLESCONTEXT_H
#define GBA_GLESCONTEXT_H


#include "../../../../gl/softGLFx.h"


//-----------------------------------------------------------------------------
class CGBAGLESContext
 : public CSoftGLESFixed
{
public:
  CGBAGLESContext();
  virtual ~CGBAGLESContext();

  virtual void glClear(GLbitfield mask);
};


#endif // GBA_GLESCONTEXT_H
