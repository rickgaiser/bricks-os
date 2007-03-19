#include "GL/gl.h"
#include "GL/CMatrix.h"
#include "GL/fixedPoint.h"

#include "asm/arch/registers.h"
#include "asm/arch/macros.h"

typedef unsigned int wint_t;
#include <math.h>


#define FP_PRESICION_ZBUFFER    8 //  8.8
#define FP_PRESICION_COLOR      8 // 24.8
#define ZBUFFER_MAX_DEPTH       fpfromi(FP_PRESICION_ZBUFFER, (1 << (FP_PRESICION_ZBUFFER - 1)) - 1) // 2^7-1
typedef short zbuf_t;


//-----------------------------------------------------------------------------
struct SColor
{
  fxpoint_t r;
  fxpoint_t g;
  fxpoint_t b;
  fxpoint_t a;
};

//-----------------------------------------------------------------------------
struct SVertex
{
  // 3D Point (in space) x/y/z
  fxpoint_t v[3];
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
  fxpoint_t n[3];
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
  GLint     x_[160];
//  GLint   * x_;
  // Edge depth (fp: 8.8)
  zbuf_t    z_[160];
//  zbuf_t  * z_;
  // Edge color (fp: 24.8)
  SColor    c_[160];
//  SColor  * c_;
};

//-----------------------------------------------------------------------------
class CGLContext
{
public:
  CGLContext();

public:
  CSurface * renderSurface;
  zbuf_t  * zbuffer;

  GLenum    currentMode;
  GLenum    iShadeModel;
  GLenum    capabilities;
  bool      bCullBack;

  // Matrix
  GLenum    matrixMode;
  CMatrix   matrixModelView;
  CMatrix   matrixProjection;

  // Colors/Lights
  SColor    colorCurrent;
  SColor    globalColor;
  SColor    clearColor;
  SColor    globalLight[3];
  SColor    lights[8][3];

  SPolygon  globalPolygon;
  GLint     iGlobalPolyVCount;

  // Viewport
  GLint     viewportXOffset;
  GLint     viewportYOffset;
  GLsizei   viewportWidth;
  GLsizei   viewportHeight;
  GLsizei   viewportPixelCount;
  GLsizei   viewportByteCount;
  fxpoint_t fpFieldofviewXScalar;  // fp: matrix precision
  fxpoint_t fpFieldofviewYScalar;  // fp: matrix precision
};


CGLContext context;


//-----------------------------------------------------------------------------
#define SCREENX(v) (m_fptoi(m_fpdiv(m_fpmul(v[0], context.fpFieldofviewXScalar), -v[2])) + (context.viewportWidth  >> 1))
#define SCREENY(v) (m_fptoi(m_fpdiv(m_fpmul(v[1], context.fpFieldofviewYScalar), -v[2])) + (context.viewportHeight >> 1))
#define RGB(r,g,b) (((b << 7) & 0xfc00) | ((g << 2) & 0x03e0) | ((r >> 3) & 0x001f))


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CGLContext::CGLContext()
 : renderSurface(0)
 , zbuffer(0)
 , currentMode(GL_POINTS)
 , iShadeModel(GL_FLAT)
 , capabilities(0)
 , bCullBack(true)
 , matrixMode(GL_MODELVIEW)
 , iGlobalPolyVCount(0)
 , viewportXOffset(0)
 , viewportYOffset(0)
 , viewportWidth(0)
 , viewportHeight(0)
 , viewportPixelCount(0)
 , viewportByteCount(0)
 , fpFieldofviewXScalar(m_fpfromf(1.0f))
 , fpFieldofviewYScalar(m_fpfromf(1.0f))
{
  globalColor.r = fpfromi(FP_PRESICION_COLOR, 0);
  globalColor.g = fpfromi(FP_PRESICION_COLOR, 0);
  globalColor.b = fpfromi(FP_PRESICION_COLOR, 0);
  globalColor.a = fpfromi(FP_PRESICION_COLOR, 0);

  clearColor.r = fpfromi(FP_PRESICION_COLOR, 0);
  clearColor.g = fpfromi(FP_PRESICION_COLOR, 0);
  clearColor.b = fpfromi(FP_PRESICION_COLOR, 0);
  clearColor.a = fpfromi(FP_PRESICION_COLOR, 0);

  for(int i(0); i < 3; i++)
  {
    globalLight[i].r = fpfromi(FP_PRESICION_COLOR, 255);
    globalLight[i].g = fpfromi(FP_PRESICION_COLOR, 255);
    globalLight[i].b = fpfromi(FP_PRESICION_COLOR, 255);
    globalLight[i].a = fpfromi(FP_PRESICION_COLOR, 255);
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CEdge::CEdge(uint32_t height)
{
/*
  x_ = new GLint[height];
  z_ = new zbuf_t[height];
  c_ = new SColor[height];
*/
}

//-----------------------------------------------------------------------------
CEdge::~CEdge()
{
/*
  delete x_;
  delete z_;
  delete c_;
*/
}

//-----------------------------------------------------------------------------
void
CEdge::add(SVertex * vfrom, SVertex * vto)
{
  if(vto->sy != vfrom->sy)
  {
    GLint dy(vto->sy - vfrom->sy);
    GLint x(fpfromi(16, vfrom->sx));
    GLint z(vfrom->v[2]);
    GLint mx((fpfromi(16, vto->sx  - vfrom->sx )) / dy);
    GLint mz((vto->v[2] - vfrom->v[2]) / dy);

    switch(context.iShadeModel)
    {
      case GL_FLAT:
      {
        int yfrom = (vfrom->sy < 0) ? 0 : (vfrom->sy);
        int yto   = (vto->sy >= context.viewportHeight) ? (context.viewportHeight - 1) : (vto->sy);
        for(int y(yfrom); y < yto; y++)
        {
          x_[y] = fptoi(16, x);
          z_[y] = z;

          x += mx;
          z += mz;
        }
        break;
      }
      case GL_SMOOTH:
      {
        GLint x(fpfromi(16, vfrom->sx ));
        GLint z(vfrom->v[2]);
        GLint r(vfrom->c.r);
        GLint g(vfrom->c.g);
        GLint b(vfrom->c.b);
        GLint mx((fpfromi(16, vto->sx  - vfrom->sx )) / dy);
        GLint mz((vto->v[2] - vfrom->v[2]) / dy);
        GLint mr((vto->c.r - vfrom->c.r) / dy);
        GLint mg((vto->c.g - vfrom->c.g) / dy);
        GLint mb((vto->c.b - vfrom->c.b) / dy);

        for(int y(vfrom->sy); y < vto->sy; y++)
        {
          if(y >= context.viewportHeight)
            break;

          if(y >= 0)
          {
            x_[y]   = fptoi(16, x);
            z_[y]   = z;
            c_[y].r = r;
            c_[y].g = g;
            c_[y].b = b;
          }

          x += mx;
          z += mz;
          r += mr;
          g += mg;
          b += mb;
        }
      }
      break;
    }
  }
}

//-----------------------------------------------------------------------------
void
glSetSurface(CSurface * surface)
{
  context.renderSurface = surface;
}

//-----------------------------------------------------------------------------
// Horizontal Line Fill
inline void
hline(GLint x1, GLint x2, GLint y, SColor c)
{
  // Check for invalid
  if(x1 == x2)
    return;
  else if(x1 > x2)
  {
    // Swap
    GLint tmp(x1);
    x1 = x2;
    x2 = tmp;
  }

  // Range limit
  if(x1 < 0)
    x1 = 0;
  if(x2 >= context.viewportWidth)
    x2 = context.viewportWidth - 1;

  short color = RGB(fptoi(FP_PRESICION_COLOR, c.r),
                    fptoi(FP_PRESICION_COLOR, c.g),
                    fptoi(FP_PRESICION_COLOR, c.b));

  dmaFill16(color, &context.renderSurface->p[(y * context.viewportWidth) + x1], x2 - x1);
}

//-----------------------------------------------------------------------------
// Horizontal Line Fill, with depth test
inline void
hline_d(GLint x1, GLint x2, GLint y, zbuf_t z1, zbuf_t z2, SColor c)
{
  // Check for invalid
  if(x1 == x2)
    return;
  else if(x1 > x2)
  {
    // Swap
    GLint  xtmp(x1);
    zbuf_t ztmp(z1);

    x1 = x2;
    x2 = xtmp;

    z1 = z2;
    z2 = ztmp;
  }

  zbuf_t z(z1);
  zbuf_t mz((z2 - z1) / (x2 - x1));

  // Range check before loop, so the loop can be optimized for speed
  if(x1 < 0)
  {
    z += mz * (-x1);
    x1 = 0;
  }
  if(x2 >= context.viewportWidth)
  {
    x2 = context.viewportWidth - 1;
  }

  short color = RGB(fptoi(FP_PRESICION_COLOR, c.r), fptoi(FP_PRESICION_COLOR, c.g), fptoi(FP_PRESICION_COLOR, c.b));
  unsigned long index((y * context.viewportWidth) + x1);
  for(GLint x(x1); x < x2; x++)
  {
    if(context.zbuffer[index] > z)
    {
      context.zbuffer[index] = z;
      context.renderSurface->p[index] = color;
    }
    z += mz;
    index++;
  }
}

//-----------------------------------------------------------------------------
// Horizontal Line Fill, smooth colors
inline void
hline_s(CEdge & from, CEdge & to, GLint & y)
{
  if(from.x_[y] < to.x_[y])
  {
    GLint dx(to.x_[y] - from.x_[y]);
    GLint mr((to.c_[y].r - from.c_[y].r) / dx);
    GLint mg((to.c_[y].g - from.c_[y].g) / dx);
    GLint mb((to.c_[y].b - from.c_[y].b) / dx);
    GLint r(from.c_[y].r);
    GLint g(from.c_[y].g);
    GLint b(from.c_[y].b);

    unsigned long index((y * context.viewportWidth) + from.x_[y]);
    for(GLint x(from.x_[y]); x < to.x_[y]; x++)
    {
      if(x >= context.viewportWidth)
        break;

      if(x >= 0)
        context.renderSurface->p[index] = RGB(fptoi(FP_PRESICION_COLOR, r), fptoi(FP_PRESICION_COLOR, g), fptoi(FP_PRESICION_COLOR, b));

      r += mr;
      g += mg;
      b += mb;
      index++;
    }
  }
}

//-----------------------------------------------------------------------------
// Horizontal Line Fill, smooth colors, depth test
inline void
hline_sd(CEdge & from, CEdge & to, GLint & y)
{
  if(from.x_[y] < to.x_[y])
  {
    GLint dx(to.x_[y] - from.x_[y]);
    GLint mz((to.z_[y] - from.z_[y]) / dx);
    GLint z(from.z_[y]);

    GLint mr((to.c_[y].r - from.c_[y].r) / dx);
    GLint mg((to.c_[y].g - from.c_[y].g) / dx);
    GLint mb((to.c_[y].b - from.c_[y].b) / dx);
    GLint r(from.c_[y].r);
    GLint g(from.c_[y].g);
    GLint b(from.c_[y].b);

    unsigned long index((y * context.viewportWidth) + from.x_[y]);
    for(GLint x(from.x_[y]); x < to.x_[y]; x++)
    {
      if(x >= context.viewportWidth)
        break;

      if(x >= 0)
      {
        if(context.zbuffer[index] > z)
        {
          context.zbuffer[index] = z;
          context.renderSurface->p[index] = RGB(fptoi(FP_PRESICION_COLOR, r), fptoi(FP_PRESICION_COLOR, g), fptoi(FP_PRESICION_COLOR, b));
        }
      }
      r += mr;
      g += mg;
      b += mb;
      z += mz;
      index++;
    }
  }
}

//-----------------------------------------------------------------------------
void
plotPoly(SPolygon & poly)
{
  // ModelView Transformation
  context.matrixModelView.transform(poly.v[0].v, poly.v[0].v);
  context.matrixModelView.transform(poly.v[1].v, poly.v[1].v);
  context.matrixModelView.transform(poly.v[2].v, poly.v[2].v);

  // Projection Transformation
  context.matrixProjection.transform(poly.v[0].v, poly.v[0].v);
  context.matrixProjection.transform(poly.v[1].v, poly.v[1].v);
  context.matrixProjection.transform(poly.v[2].v, poly.v[2].v);

  // Perspective division, viewport transformation
  poly.v[0].sx = SCREENX(poly.v[0].v);
  poly.v[0].sy = SCREENY(poly.v[0].v);
  poly.v[1].sx = SCREENX(poly.v[1].v);
  poly.v[1].sy = SCREENY(poly.v[1].v);
  poly.v[2].sx = SCREENX(poly.v[2].v);
  poly.v[2].sy = SCREENY(poly.v[2].v);

  // ----------------
  // Backface culling
  // ----------------
  if(context.capabilities & GL_CULL_FACE)
  {
    bool bBackFace;
    if((poly.v[1].sx != poly.v[0].sx) && (poly.v[2].sx != poly.v[0].sx))
    {
      bBackFace = ((((fpfromi(16, poly.v[1].sy - poly.v[0].sy) / (poly.v[1].sx - poly.v[0].sx)) -
                     (fpfromi(16, poly.v[2].sy - poly.v[0].sy) / (poly.v[2].sx - poly.v[0].sx))) < 0) ^
                     ((poly.v[0].sx <= poly.v[1].sx) == (poly.v[0].sx > poly.v[2].sx)));
    }
    else if((poly.v[2].sx != poly.v[1].sx) && (poly.v[0].sx != poly.v[1].sx))
    {
      bBackFace = ((((fpfromi(16, poly.v[2].sy - poly.v[1].sy) / (poly.v[2].sx - poly.v[1].sx)) -
                     (fpfromi(16, poly.v[0].sy - poly.v[1].sy) / (poly.v[0].sx - poly.v[1].sx))) < 0) ^
                     ((poly.v[1].sx <= poly.v[2].sx) == (poly.v[1].sx > poly.v[0].sx)));
    }
    else if((poly.v[0].sx != poly.v[2].sx) && (poly.v[1].sx != poly.v[2].sx))
    {
      bBackFace = ((((fpfromi(16, poly.v[0].sy - poly.v[2].sy) / (poly.v[0].sx - poly.v[2].sx)) -
                     (fpfromi(16, poly.v[1].sy - poly.v[2].sy) / (poly.v[1].sx - poly.v[2].sx))) < 0) ^
                     ((poly.v[2].sx <= poly.v[0].sx) == (poly.v[2].sx > poly.v[1].sx)));
    }
    else
    {
      // Triangle invisible
      return;
    }

    if(bBackFace == context.bCullBack)
      return;
  }

  // Bubble sort the 3 vertexes
  SVertex * vtemp;
  SVertex * vhi(&(poly.v[0]));
  SVertex * vmi(&(poly.v[1]));
  SVertex * vlo(&(poly.v[2]));

  // Swap bottom with middle?
  if(vlo->sy > vmi->sy)
  {
    vtemp = vmi;
    vmi   = vlo;
    vlo   = vtemp;
  }
  // Swap middle with top?
  if(vmi->sy > vhi->sy)
  {
    vtemp = vhi;
    vhi   = vmi;
    vmi   = vtemp;
    // Swap bottom with middle again?
    if(vlo->sy > vmi->sy)
    {
      vtemp = vmi;
      vmi   = vlo;
      vlo   = vtemp;
    }
  }

  // Create edge lists
  CEdge edge1(context.viewportHeight);
  CEdge edge2(context.viewportHeight);

  edge1.add(vlo, vhi);
  edge2.add(vlo, vmi);
  edge2.add(vmi, vhi);

  CEdge * pEdgeLeft  = &edge1;
  CEdge * pEdgeRight = &edge2;
  GLint my(vlo->sy+((vhi->sy-vlo->sy)/2));
  if(edge1.x_[my] > edge2.x_[my])
  {
    // Swap
    pEdgeLeft  = &edge2;
    pEdgeRight = &edge1;
  }

  // Display triangle (horizontal lines forming the triangle)
  switch(context.iShadeModel)
  {
    case GL_FLAT:
    {
      if(context.capabilities & GL_DEPTH_TEST)
      {
        // Flat shading with depth test
        for(GLint y(vlo->sy); y < vhi->sy; y++)
          hline_d(pEdgeLeft->x_[y], pEdgeRight->x_[y], y, pEdgeLeft->z_[y], pEdgeRight->z_[y], poly.v[2].c);
        break;
      }
      else
      {
        // Flat shading without depth test
        for(GLint y(vlo->sy); y < vhi->sy; y++)
          hline(pEdgeLeft->x_[y], pEdgeRight->x_[y], y, poly.v[2].c);
        break;
      }
    }
    case GL_SMOOTH:
    {
      if(context.capabilities & GL_DEPTH_TEST)
      {
        for(GLint y(vlo->sy); y < vhi->sy; y++)
          hline_sd(*pEdgeLeft, *pEdgeRight, y);
        break;
      }
      else
      {
        for(GLint y(vlo->sy); y < vhi->sy; y++)
          hline_s(*pEdgeLeft, *pEdgeRight, y);
        break;
      }
    }
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// GL API
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void
glBegin(GLenum mode)
{
  context.currentMode = mode;
}

//-----------------------------------------------------------------------------
void
glEnd()
{
  context.currentMode = 0;
}

//-----------------------------------------------------------------------------
void
glVertex3fp(GLint x, GLint y, GLint z)
{
  GLint vtx[3] = {x, y, z};
  glVertex3fpv(vtx);
}

//-----------------------------------------------------------------------------
void
glVertex3i(GLint x, GLint y, GLint z)
{
  GLint vtx[3] = {m_fpfromi(x), m_fpfromi(y), m_fpfromi(z)};
  glVertex3fpv(vtx);
}

//-----------------------------------------------------------------------------
void
glVertex3f(GLfloat x, GLfloat y, GLfloat z)
{
  GLint vtx[3] = {m_fpfromf(x), m_fpfromf(y), m_fpfromf(z)};
  glVertex3fpv(vtx);
}

//-----------------------------------------------------------------------------
void
glVertex3fpv(const GLint * v)
{
  switch(context.currentMode)
  {
    case GL_TRIANGLES:
    {
      // Store values
      context.globalPolygon.v[context.iGlobalPolyVCount].v[0] = v[0];
      context.globalPolygon.v[context.iGlobalPolyVCount].v[1] = v[1];
      context.globalPolygon.v[context.iGlobalPolyVCount].v[2] = v[2];
      context.globalPolygon.v[context.iGlobalPolyVCount].c  = context.colorCurrent;

      // Check for finished triangle
      if(context.iGlobalPolyVCount == 2)
      {
        plotPoly(context.globalPolygon);
        context.iGlobalPolyVCount = 0;
      }
      else
      {
        context.iGlobalPolyVCount++;
      }
      break;
    }
  }
}

//-----------------------------------------------------------------------------
void
glVertex3iv(const GLint * v)
{
  GLint vtx[3] = {m_fpfromi(v[0]), m_fpfromi(v[1]), m_fpfromi(v[2])};
  glVertex3fpv(vtx);
}

//-----------------------------------------------------------------------------
void
glVertex3fv(const GLfloat * v)
{
  GLint vtx[3] = {m_fpfromf(v[0]), m_fpfromf(v[1]), m_fpfromf(v[2])};
  glVertex3fpv(vtx);
}

//-----------------------------------------------------------------------------
void
glColor3ub(GLubyte red, GLubyte green, GLubyte blue)
{
  context.globalColor.r = fpfromi(FP_PRESICION_COLOR, red);
  context.globalColor.g = fpfromi(FP_PRESICION_COLOR, green);
  context.globalColor.b = fpfromi(FP_PRESICION_COLOR, blue);

  if(context.capabilities & GL_LIGHTING)
  {
//    context.colorCurrent.r = fpmul(FP_PRESICION_COLOR, context.globalColor.r, context.globalLight[0].r) / 255;
//    context.colorCurrent.g = fpmul(FP_PRESICION_COLOR, context.globalColor.g, context.globalLight[0].g) / 255;
//    context.colorCurrent.b = fpmul(FP_PRESICION_COLOR, context.globalColor.b, context.globalLight[0].b) / 255;
    context.colorCurrent.r = context.globalColor.r * fptoi(FP_PRESICION_COLOR, context.globalLight[0].r) / 255;
    context.colorCurrent.g = context.globalColor.g * fptoi(FP_PRESICION_COLOR, context.globalLight[0].g) / 255;
    context.colorCurrent.b = context.globalColor.b * fptoi(FP_PRESICION_COLOR, context.globalLight[0].b) / 255;
  }
  else
  {
    context.colorCurrent   = context.globalColor;
  }
}

//-----------------------------------------------------------------------------
void
glColor3f(GLfloat red, GLfloat green, GLfloat blue)
{
  glColor3ub((GLubyte)(red * 255), (GLubyte)(green * 255), (GLubyte)(blue * 255));
}

//-----------------------------------------------------------------------------
void
glNormal3i(GLint nx, GLint ny, GLint nz)
{
  GLint normal[3] = {m_fpfromi(nx), m_fpfromi(ny), m_fpfromi(nz)};
  glNormal3iv(normal);
}

//-----------------------------------------------------------------------------
void
glNormal3iv(const GLint * v)
{
  // FIXME
}

//-----------------------------------------------------------------------------
// Matrix Operations
//-----------------------------------------------------------------------------
void glMatrixMode(GLenum mode)
{
  context.matrixMode = mode;
}

//-----------------------------------------------------------------------------
void
glLoadIdentity()
{
  switch(context.matrixMode)
  {
    case GL_MODELVIEW:
      context.matrixModelView.loadIdentity();
      break;
    case GL_PROJECTION:
      context.matrixProjection.loadIdentity();
      break;
  };
}

//-----------------------------------------------------------------------------
void
glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
  switch(context.matrixMode)
  {
    case GL_MODELVIEW:
      context.matrixModelView.rotate(m_fpfromf(angle), m_fpfromf(x), m_fpfromf(y), m_fpfromf(z));
      break;
    case GL_PROJECTION:
      context.matrixProjection.rotate(m_fpfromf(angle), m_fpfromf(x), m_fpfromf(y), m_fpfromf(z));
      break;
  };
}

//-----------------------------------------------------------------------------
void
glScalef(GLfloat x, GLfloat y, GLfloat z)
{
  switch(context.matrixMode)
  {
    case GL_MODELVIEW:
      context.matrixModelView.scale(m_fpfromf(x), m_fpfromf(y), m_fpfromf(z));
      break;
    case GL_PROJECTION:
      context.matrixProjection.scale(m_fpfromf(x), m_fpfromf(y), m_fpfromf(z));
      break;
  };
}

//-----------------------------------------------------------------------------
void
glTranslatef(GLfloat x, GLfloat y, GLfloat z)
{
  switch(context.matrixMode)
  {
    case GL_MODELVIEW:
      context.matrixModelView.translate(m_fpfromf(x), m_fpfromf(y), m_fpfromf(-z));
      break;
    case GL_PROJECTION:
      context.matrixProjection.translate(m_fpfromf(x), m_fpfromf(y), m_fpfromf(-z));
      break;
  };
}

//-----------------------------------------------------------------------------
// GBA 240x160(38400px):  54% @ 30fps ( 55fps max) ewram
// GBA 160x128(20480px):  29% @ 30fps (103fps max) ewram
// GBA 160x128(20480px):  22% @ 30fps (136fps max) vram
void
glClear(GLbitfield mask)
{
  unsigned short color(RGB(fptoi(FP_PRESICION_COLOR, context.clearColor.r), fptoi(FP_PRESICION_COLOR, context.clearColor.g), fptoi(FP_PRESICION_COLOR, context.clearColor.b)));
  long iCount(context.viewportByteCount >> 1);

  if(mask & GL_COLOR_BUFFER_BIT)
    dmaFill16(color, context.renderSurface->p, iCount);
  if((mask & GL_DEPTH_BUFFER_BIT) && (context.capabilities & GL_DEPTH_TEST))
    dmaFill16(ZBUFFER_MAX_DEPTH, context.zbuffer, iCount);
}

//-----------------------------------------------------------------------------
void
glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
  if(context.zbuffer)
    delete context.zbuffer;

  context.viewportXOffset    = x;
  context.viewportYOffset    = y;
  context.viewportWidth      = width;
  context.viewportHeight     = height;
  context.viewportPixelCount = width * height;
  context.viewportByteCount  = width * height * 2;
  context.zbuffer            = new zbuf_t[width * height];

  // Calculate field of view scalars
  context.fpFieldofviewXScalar = m_fpfromf(static_cast<float>(width)  / tan(80)); // 5.67 == tan(80)
  context.fpFieldofviewYScalar = m_fpfromf(static_cast<float>(height) / tan(80)); // 5.67 == tan(80)
}

//-----------------------------------------------------------------------------
void
glShadeModel(GLenum mode)
{
  context.iShadeModel = mode;
}

//-----------------------------------------------------------------------------
void
glEnable(GLenum cap)
{
  context.capabilities |= cap;
}

//-----------------------------------------------------------------------------
void
glDisable(GLenum cap)
{
  context.capabilities &= ~cap;
}

//-----------------------------------------------------------------------------
void
glLightModeliv(GLenum pname, const GLint * params)
{
  switch(pname)
  {
    case GL_LIGHT_MODEL_AMBIENT:
      context.globalLight[0].r = fpfromi(FP_PRESICION_COLOR, params[0]);
      context.globalLight[0].g = fpfromi(FP_PRESICION_COLOR, params[1]);
      context.globalLight[0].b = fpfromi(FP_PRESICION_COLOR, params[2]);
      context.globalLight[0].a = fpfromi(FP_PRESICION_COLOR, params[3]);
      break;
  }
}

//-----------------------------------------------------------------------------
void
glLightModelfv(GLenum pname, const GLfloat * params)
{
  switch(pname)
  {
    case GL_LIGHT_MODEL_AMBIENT:
      context.globalLight[0].r = fpfromf(FP_PRESICION_COLOR, params[0] * 255);
      context.globalLight[0].g = fpfromf(FP_PRESICION_COLOR, params[1] * 255);
      context.globalLight[0].b = fpfromf(FP_PRESICION_COLOR, params[2] * 255);
      context.globalLight[0].a = fpfromf(FP_PRESICION_COLOR, params[3] * 255);
      break;
  }
}

//-----------------------------------------------------------------------------
void
glLightfv(GLenum light, GLenum pname, const GLfloat * params)
{
  GLint iLight(-1), iType(-1);

  switch(light)
  {
  case GL_LIGHT0: iLight = 0; break;
  case GL_LIGHT1: iLight = 1; break;
  case GL_LIGHT2: iLight = 2; break;
  case GL_LIGHT3: iLight = 3; break;
  case GL_LIGHT4: iLight = 4; break;
  case GL_LIGHT5: iLight = 5; break;
  case GL_LIGHT6: iLight = 6; break;
  case GL_LIGHT7: iLight = 7; break;
  }

  switch(pname)
  {
  case GL_AMBIENT:  iType = 0; break;
  case GL_DIFFUSE:  iType = 1; break;
  case GL_SPECULAR: iType = 2; break;
  }

  if(iLight != -1 && iType != -1)
  {
    context.lights[iLight][iType].r = fpfromi(FP_PRESICION_COLOR, (GLint)(params[0] * 255));
    context.lights[iLight][iType].g = fpfromi(FP_PRESICION_COLOR, (GLint)(params[1] * 255));
    context.lights[iLight][iType].b = fpfromi(FP_PRESICION_COLOR, (GLint)(params[2] * 255));
    context.lights[iLight][iType].a = fpfromi(FP_PRESICION_COLOR, (GLint)(params[3] * 255));
  }
}

//-----------------------------------------------------------------------------
void
glColorMaterial(GLenum face, GLenum mode)
{
}

//-----------------------------------------------------------------------------
void
glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
  context.clearColor.r = fpfromf(FP_PRESICION_COLOR, red   * 255.0f);
  context.clearColor.g = fpfromf(FP_PRESICION_COLOR, green * 255.0f);
  context.clearColor.b = fpfromf(FP_PRESICION_COLOR, blue  * 255.0f);
  context.clearColor.a = fpfromf(FP_PRESICION_COLOR, alpha * 255.0f);
}

//-----------------------------------------------------------------------------
void
glClearColor3ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha)
{
  context.clearColor.r = fpfromi(FP_PRESICION_COLOR, red);
  context.clearColor.g = fpfromi(FP_PRESICION_COLOR, green);
  context.clearColor.b = fpfromi(FP_PRESICION_COLOR, blue);
  context.clearColor.a = fpfromi(FP_PRESICION_COLOR, alpha);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// GLU API
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void
gluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
  // Calculate field of view scalars
  context.fpFieldofviewYScalar = m_fpfromf(context.viewportHeight / tan(fovy));
  context.fpFieldofviewXScalar = m_fpmul(context.fpFieldofviewYScalar, m_fpfromf(aspect));
}
