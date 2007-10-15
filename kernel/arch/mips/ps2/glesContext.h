#ifndef PS2_GLESCONTEXT_H
#define PS2_GLESCONTEXT_H


#include "../../../../gl/softGLF.h"


//-----------------------------------------------------------------------------
class CPS2GLESContext
 : public CSoftGLESFloat
{
public:
  CPS2GLESContext();
  virtual ~CPS2GLESContext();

  virtual void glClear(GLbitfield mask);
  virtual void glFlush(void);

protected:
  virtual void rasterPoly(SPolygonF & poly);
};


IGLESContext * getGLESContext();


#endif // GBA_GLESCONTEXT_H
