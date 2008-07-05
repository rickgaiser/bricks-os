#ifndef NDS_GLESCONTEXTNDS_H
#define NDS_GLESCONTEXTNDS_H


#include "../../../../gl/softGLFx.h"
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

#define MAX_TEXTURE_COUNT 32


//-----------------------------------------------------------------------------
typedef GLfixed NDSfixed;

//-----------------------------------------------------------------------------
struct STextureNDS
{
  bool used;

  GLsizei width;
  GLsizei height;

  uint32_t format;

  const void * data;
};

//-----------------------------------------------------------------------------
class CNDSGLESContext
 : public CASoftGLESFixed
{
public:
  CNDSGLESContext();
  virtual ~CNDSGLESContext();

  // Surfaces
  virtual void       setSurface(CSurface * surface){IRenderer::setSurface(surface);}
  virtual CSurface * getSurface()                  {return IRenderer::getSurface();}

  // Flush operations to surface
  virtual void       flush()                       {IRenderer::flush();}

  virtual void glBindTexture(GLenum target, GLuint texture);
  virtual void glClear(GLbitfield mask);
  virtual void glClearColorx(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha);
  virtual void glClearDepthx(GLclampx depth);
  virtual void glColor4ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);
  virtual void glColor4x(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha);
  virtual void glCullFace(GLenum mode);
  virtual void glDeleteTextures(GLsizei n, const GLuint *textures);
  virtual void glDisable(GLenum cap);
  virtual void glEnable(GLenum cap);
  virtual void glFlush(void);
  virtual void glFogx(GLenum pname, GLfixed param);
  virtual void glFogxv(GLenum pname, const GLfixed *params);
  virtual void glFrustumx(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar);
  virtual void glGenTextures(GLsizei n, GLuint *textures);
  virtual void glLoadIdentity(void);
  virtual void glLoadMatrixx(const GLfixed *m);
  virtual void glMatrixMode(GLenum mode);
  virtual void glMultMatrixx(const GLfixed *m);
  virtual void glNormal3x(GLfixed nx, GLfixed ny, GLfixed nz);
  virtual void glOrthox(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar);
  virtual void glRotatex(GLfixed angle, GLfixed x, GLfixed y, GLfixed z);
  virtual void glScalex(GLfixed x, GLfixed y, GLfixed z);
  virtual void glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
  virtual void glTexParameterf(GLenum target, GLenum pname, GLfloat param);
  virtual void glTexParameterx(GLenum target, GLenum pname, GLfixed param);
  virtual void glTranslatex(GLfixed x, GLfixed y, GLfixed z);
  virtual void glViewport(GLint x, GLint y, GLsizei width, GLsizei height);

protected:
  virtual void begin(GLenum mode);
  virtual void end();
  virtual void rasterTriangle(STriangleFx & tri);

private:
  void plotPoly(SVertexFx * vtx[3]);
  void updateLights();
  void updateFog();

  uint32_t    iNDSGFXControl_;
  uint32_t    iNDSPolyFormat_;

  GLenum      matrixMode_;
  uint32_t    ndsCurrentMatrixId_;

  static bool        bInitialized_;
  static NDSfixed    fpSin_[];
  static NDSfixed    fpCos_[];

  bool        texturesEnabled_;
  STextureNDS * pCurrentTex_;

  STextureNDS textures_[MAX_TEXTURE_COUNT];
};


#endif // NDS_GLESCONTEXTNDS_H
