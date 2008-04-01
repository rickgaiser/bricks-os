#ifndef NDS_MATRIXNDS_H
#define NDS_MATRIXNDS_H


#include "../../../../gl/context.h"
#include "../../../../gl/fixedPoint.h"


#define NDS_MATRIX_PRECISION   12
#define nds_fpfromi(i)   fpfromi(NDS_MATRIX_PRECISION,i)
#define nds_fptoi(i)     fptoi(NDS_MATRIX_PRECISION,i)
#define nds_fpfromf(i)   fpfromf(NDS_MATRIX_PRECISION,i)
#define nds_fptof(i)     fptof(NDS_MATRIX_PRECISION,i)
#define nds_fpmul(i1,i2) fpmul32(NDS_MATRIX_PRECISION,i1,i2)
#define nds_fpdiv(i1,i2) fpdiv32(NDS_MATRIX_PRECISION,i1,i2)

#define nds_to_gl(i)     (i>>4)
#define gl_to_ndsv(i)    (i>>4)  // Vertex format 4.12
#define gl_to_ndst(i)    (i>>12) // Texture format 12.4
#define gl_to_ndsz(i)    (i>>13) // Depth format 12.3


typedef GLfixed NDSfixed;


//-----------------------------------------------------------------------------
class CAGLESMatrixNDSFx
 : public virtual I3DRenderer
{
public:
  CAGLESMatrixNDSFx();
  virtual ~CAGLESMatrixNDSFx();

  virtual void glFrustumx(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar);
  virtual void glLoadMatrixx(const GLfixed *m);
  virtual void glMultMatrixx(const GLfixed *m);
  virtual void glOrthox(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar);
  virtual void glRotatex(GLfixed angle, GLfixed x, GLfixed y, GLfixed z);
  virtual void glScalex(GLfixed x, GLfixed y, GLfixed z);
  virtual void glTranslatex(GLfixed x, GLfixed y, GLfixed z);

  virtual void glLoadIdentity(void);
  virtual void glMatrixMode(GLenum mode);

protected:
  GLenum      matrixMode_;
  uint32_t    ndsCurrentMatrixId_;

  static bool        bInitialized_;
  static NDSfixed    fpSin_[];
  static NDSfixed    fpCos_[];
};


#endif
