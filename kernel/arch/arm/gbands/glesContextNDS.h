#ifndef NDS_GLESCONTEXTNDS_H
#define NDS_GLESCONTEXTNDS_H


#include "../../../../gl/softGLFx.h"


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
};


IGLESContext * getGLESContext();


#endif // GBA_GLESCONTEXT_H
