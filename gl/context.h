#ifndef GL_CONTEXT_H
#define GL_CONTEXT_H


#include "GLES/gl.h"
#include "GLES/gl_extra.h"

#include "matrix.h"
#include "fixedPoint.h"
#include "context.h"


#define FP_PRESICION_ZBUFFER    8 //  8.8
#define FP_PRESICION_COLOR      8 // 24.8

typedef short zbuf_t;


//-----------------------------------------------------------------------------
struct SColor
{
  GLfixed r;
  GLfixed g;
  GLfixed b;
  GLfixed a;
};

//-----------------------------------------------------------------------------
struct SVertex
{
  // 3D Point (in space) x/y/z
  GLfixed v[3];
  // 2D Point (on screen) x/y
  GLint     sx;
  GLint     sy;
  // Color
  SColor    c;
};

//-----------------------------------------------------------------------------
// Triangle/Polygon
struct SPolygon
{
  SVertex   v[3];
  GLfixed n[3];
};

//-----------------------------------------------------------------------------
class CEdge
{
public:
  CEdge(uint32_t height);
  ~CEdge();

  void add(SVertex * vfrom, SVertex * vto);

public:
  // Edge x
  GLint   * x_;
  // Edge depth (fp: 8.8)
  zbuf_t  * z_;
  // Edge color (fp: 24.8)
  SColor  * c_;
};

//-----------------------------------------------------------------------------
struct SBufferPointer
{
  GLint size;
  GLenum type;
  GLsizei stride;
  const GLvoid * pointer;
};

//-----------------------------------------------------------------------------
class CContext
{
public:
  CContext();
  virtual ~CContext();

  void glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
  void glMatrixMode(GLenum mode);
  void glLoadIdentity();
  void glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
  void glScalef(GLfloat x, GLfloat y, GLfloat z);
  void glTranslatef(GLfloat x, GLfloat y, GLfloat z);
  void glClear(GLbitfield mask);
  void glShadeModel(GLenum mode);
  void glEnable(GLenum cap);
  void glDisable(GLenum cap);
  void glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);

  void glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer);
  void glDrawArrays(GLenum mode, GLint first, GLsizei count);
  void glFlush(void);
  void glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer);
  void glViewport(GLint x, GLint y, GLsizei width, GLsizei height);

private:
  void hline(GLint x1, GLint x2, GLint y, SColor c);
  void hline_d(GLint x1, GLint x2, GLint y, zbuf_t z1, zbuf_t z2, SColor c);
  void hline_s(CEdge & from, CEdge & to, GLint & y);
  void hline_sd(CEdge & from, CEdge & to, GLint & y);
  void plotPoly(SPolygon & poly);

public: // FIXME: should be private
  CSurface * renderSurface;
  zbuf_t  * zbuffer;

  GLenum    beginMode_;
  GLenum    shadingModel_;
  GLenum    enableCapabilities_;

  // Matrix
  GLenum    matrixMode_;
  CMatrix   matrixModelView;
  CMatrix   matrixProjection;

  // Buffers
  SBufferPointer bufColor_;
  SBufferPointer bufVertex_;

  // Colors/Lights
  SColor    clCurrent;
  SColor    clClear;

  SPolygon  globalPolygon;
  GLint     iGlobalPolyVCount;
  CEdge   * edge1;
  CEdge   * edge2;

  // Viewport
  GLint     viewportXOffset;
  GLint     viewportYOffset;
  GLsizei   viewportWidth;
  GLsizei   viewportHeight;
  GLsizei   viewportPixelCount;
  GLsizei   viewportByteCount;
  GLfixed   fpFieldofviewXScalar;
  GLfixed   fpFieldofviewYScalar;
};


#endif // GL_CONTEXT_H
