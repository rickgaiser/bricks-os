#ifndef NDS_GLESCONTEXTNDS_H
#define NDS_GLESCONTEXTNDS_H


#include "../../../../gl/softGLFx.h"
#include "../../../../gl/fixedPoint.h"


// Macro's for NDS (4.12)
#define FP_PRESICION_NDS 12
#define nds_fpfromi(i)   fpfromi(FP_PRESICION_NDS,i)
#define nds_fptoi(i)     fptoi(FP_PRESICION_NDS,i)
#define nds_fpfromf(i)   fpfromf(FP_PRESICION_NDS,i)
#define nds_fptof(i)     fptof(FP_PRESICION_NDS,i)
#define nds_fpmul(i1,i2) fpmul32(FP_PRESICION_NDS,i1,i2)
#define nds_fpdiv(i1,i2) fpdiv32(FP_PRESICION_NDS,i1,i2)

#define nds_to_gl(i)     (i>>4)
#define gl_to_nds(i)     (i>>4)


typedef GLfixed NDSfixed;


//-----------------------------------------------------------------------------
class CNDSGLESContext
 : public CSoftGLESFixed
{
public:
  CNDSGLESContext();
  virtual ~CNDSGLESContext();

  virtual void glClear(GLbitfield mask);
  virtual void glClearColorx(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha);
  virtual void glFlush(void);
  virtual void glFrustumx(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar);
  virtual void glLoadIdentity(void);
  virtual void glMatrixMode(GLenum mode);
  virtual void glRotatex(GLfixed angle, GLfixed x, GLfixed y, GLfixed z);
  virtual void glScalex(GLfixed x, GLfixed y, GLfixed z);
  virtual void glTranslatex(GLfixed x, GLfixed y, GLfixed z);
  virtual void glViewport(GLint x, GLint y, GLsizei width, GLsizei height);

protected:
  virtual void rasterPoly(SPolygonFx & poly);

  uint32_t ndsCurrentMatrixId_;
};


IGLESContext * getGLESContext();


#endif // GBA_GLESCONTEXT_H
