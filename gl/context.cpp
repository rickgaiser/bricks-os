#include "context.h"
#include "matrix.h"
#include "fixedPoint.h"

#include "asm/arch/macros.h"
typedef unsigned int wint_t;
#include <math.h>


#define SCREENX(v) (m_fptoi(m_fpdiv(m_fpmul(v[0], fpFieldofviewXScalar), -v[2])) + (viewportWidth  >> 1))
#define SCREENY(v) (m_fptoi(m_fpdiv(m_fpmul(v[1], fpFieldofviewYScalar), -v[2])) + (viewportHeight >> 1))
#define ZBUFFER_MAX_DEPTH fpfromi(FP_PRESICION_ZBUFFER, (1 << (FP_PRESICION_ZBUFFER - 1)) - 1) // 2^7-1
#define RGB(r,g,b) (0x8000 | ((b << 7) & 0x7c00) | ((g << 2) & 0x03e0) | ((r >> 3) & 0x001f))

#define c_fpfromi(i)   fpfromi(FP_PRESICION_COLOR,i)
#define c_fptoi(i)     fptoi(FP_PRESICION_COLOR,i)
#define c_fpfromf(i)   fpfromf(FP_PRESICION_COLOR,i)
#define c_fptof(i)     fptof(FP_PRESICION_COLOR,i)
#define c_fpmul(i1,i2) fpmul(FP_PRESICION_COLOR,i1,i2)
#define c_fpdiv(i1,i2) fpdiv(FP_PRESICION_COLOR,i1,i2)


//-----------------------------------------------------------------------------
CContext::CContext()
 : renderSurface(0)
 , zbuffer(0)
 , beginMode_(GL_POINTS)
 , shadingModel_(GL_FLAT)
 , enableCapabilities_(0)
 , matrixMode_(GL_MODELVIEW)
 , iGlobalPolyVCount(0)
 , edge1(0)
 , edge2(0)
 , viewportXOffset(0)
 , viewportYOffset(0)
 , viewportWidth(0)
 , viewportHeight(0)
 , viewportPixelCount(0)
 , viewportByteCount(0)
 , fpFieldofviewXScalar(m_fpfromf(1.0f))
 , fpFieldofviewYScalar(m_fpfromf(1.0f))
{
  clCurrent.r = c_fpfromi(0);
  clCurrent.g = c_fpfromi(0);
  clCurrent.b = c_fpfromi(0);
  clCurrent.a = c_fpfromi(255);

  clClear.r = c_fpfromi(0);
  clClear.g = c_fpfromi(0);
  clClear.b = c_fpfromi(0);
  clClear.a = c_fpfromi(255);
}

//-----------------------------------------------------------------------------
CContext::~CContext()
{
}

//-----------------------------------------------------------------------------
void
CContext::glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
  clCurrent.r = c_fpfromf(red  ) * 255;
  clCurrent.g = c_fpfromf(green) * 255;
  clCurrent.b = c_fpfromf(blue ) * 255;
  clCurrent.a = c_fpfromf(alpha) * 255;
}

//-----------------------------------------------------------------------------
// Matrix Operations
//-----------------------------------------------------------------------------
void
CContext::glMatrixMode(GLenum mode)
{
  matrixMode_ = mode;
}

//-----------------------------------------------------------------------------
void
CContext::glLoadIdentity()
{
  switch(matrixMode_)
  {
    case GL_MODELVIEW:
      matrixModelView.loadIdentity();
      break;
    case GL_PROJECTION:
      matrixProjection.loadIdentity();
      break;
  };
}

//-----------------------------------------------------------------------------
void
CContext::glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
  switch(matrixMode_)
  {
    case GL_MODELVIEW:
      matrixModelView.rotate(m_fpfromf(angle), m_fpfromf(x), m_fpfromf(y), m_fpfromf(z));
      break;
    case GL_PROJECTION:
      matrixProjection.rotate(m_fpfromf(angle), m_fpfromf(x), m_fpfromf(y), m_fpfromf(z));
      break;
  };
}

//-----------------------------------------------------------------------------
void
CContext::glScalef(GLfloat x, GLfloat y, GLfloat z)
{
  switch(matrixMode_)
  {
    case GL_MODELVIEW:
      matrixModelView.scale(m_fpfromf(x), m_fpfromf(y), m_fpfromf(z));
      break;
    case GL_PROJECTION:
      matrixProjection.scale(m_fpfromf(x), m_fpfromf(y), m_fpfromf(z));
      break;
  };
}

//-----------------------------------------------------------------------------
void
CContext::glTranslatef(GLfloat x, GLfloat y, GLfloat z)
{
  switch(matrixMode_)
  {
    case GL_MODELVIEW:
      matrixModelView.translate(m_fpfromf(x), m_fpfromf(y), m_fpfromf(-z));
      break;
    case GL_PROJECTION:
      matrixProjection.translate(m_fpfromf(x), m_fpfromf(y), m_fpfromf(-z));
      break;
  };
}

//-----------------------------------------------------------------------------
void
CContext::glClear(GLbitfield mask)
{
  unsigned short color(RGB(c_fptoi(clClear.r), c_fptoi(clClear.g), c_fptoi(clClear.b)));
  long iCount(viewportByteCount >> 1);

  if(mask & GL_COLOR_BUFFER_BIT)
    dmaFill16(color, renderSurface->p, iCount);
  if((mask & GL_DEPTH_BUFFER_BIT) && (enableCapabilities_ & GL_DEPTH_TEST))
    dmaFill16(ZBUFFER_MAX_DEPTH, zbuffer, iCount);
}

//-----------------------------------------------------------------------------
void
CContext::glShadeModel(GLenum mode)
{
  shadingModel_ = mode;
}

//-----------------------------------------------------------------------------
void
CContext::glEnable(GLenum cap)
{
  enableCapabilities_ |= cap;
}

//-----------------------------------------------------------------------------
void
CContext::glDisable(GLenum cap)
{
  enableCapabilities_ &= ~cap;
}

//-----------------------------------------------------------------------------
void
CContext::glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
  clClear.r = c_fpfromf(red  ) * 255;
  clClear.g = c_fpfromf(green) * 255;
  clClear.b = c_fpfromf(blue ) * 255;
  clClear.a = c_fpfromf(alpha) * 255;
}

//-----------------------------------------------------------------------------
void
CContext::glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
  bufColor_.size    = size;
  bufColor_.type    = type;
  bufColor_.stride  = stride;
  bufColor_.pointer = pointer;
}

//-----------------------------------------------------------------------------
void
CContext::glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
  switch(mode)
  {
    case GL_TRIANGLES:
    {
      for(GLint i(first); i < count; i++)
      {
        // Store values
        globalPolygon.v[iGlobalPolyVCount].v[0] = m_fpfromf(((GLfloat *)bufVertex_.pointer)[i*3+0]);
        globalPolygon.v[iGlobalPolyVCount].v[1] = m_fpfromf(((GLfloat *)bufVertex_.pointer)[i*3+1]);
        globalPolygon.v[iGlobalPolyVCount].v[2] = m_fpfromf(((GLfloat *)bufVertex_.pointer)[i*3+2]);
        globalPolygon.v[iGlobalPolyVCount].c.r  = c_fpfromf(((GLfloat *)bufColor_.pointer)[i*4+0]) * 255;
        globalPolygon.v[iGlobalPolyVCount].c.g  = c_fpfromf(((GLfloat *)bufColor_.pointer)[i*4+1]) * 255;
        globalPolygon.v[iGlobalPolyVCount].c.b  = c_fpfromf(((GLfloat *)bufColor_.pointer)[i*4+2]) * 255;
        globalPolygon.v[iGlobalPolyVCount].c.a  = c_fpfromf(((GLfloat *)bufColor_.pointer)[i*4+3]) * 255;

        // Check for finished triangle
        if(iGlobalPolyVCount == 2)
        {
          plotPoly(globalPolygon);
          iGlobalPolyVCount = 0;
        }
        else
        {
          iGlobalPolyVCount++;
        }
      }
      break;
    }
  }
}

//-----------------------------------------------------------------------------
void
CContext::glFlush(void)
{
}

//-----------------------------------------------------------------------------
void
CContext::glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
  bufVertex_.size    = size;
  bufVertex_.type    = type;
  bufVertex_.stride  = stride;
  bufVertex_.pointer = pointer;
}

//-----------------------------------------------------------------------------
void
CContext::glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
  if(zbuffer)
    delete zbuffer;
  if(edge1)
    delete edge1;
  if(edge2)
    delete edge2;

  viewportXOffset    = x;
  viewportYOffset    = y;
  viewportWidth      = width;
  viewportHeight     = height;
  viewportPixelCount = width * height;
  viewportByteCount  = width * height * 2;
  zbuffer            = new zbuf_t[width * height];
  edge1              = new CEdge(viewportHeight);
  edge2              = new CEdge(viewportHeight);

  // Calculate field of view scalars
  fpFieldofviewXScalar = m_fpfromf(static_cast<float>(width)  / tan(80)); // 5.67 == tan(80)
  fpFieldofviewYScalar = m_fpfromf(static_cast<float>(height) / tan(80)); // 5.67 == tan(80)
}

//-----------------------------------------------------------------------------
// Horizontal Line Fill
void
CContext::hline(GLint x1, GLint x2, GLint y, SColor c)
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
  if(x2 >= viewportWidth)
    x2 = viewportWidth - 1;

  short color = RGB(c_fptoi(c.r), c_fptoi(c.g), c_fptoi(c.b));

  dmaFill16(color, &renderSurface->p[(y * viewportWidth) + x1], x2 - x1);
}

//-----------------------------------------------------------------------------
// Horizontal Line Fill, with depth test
void
CContext::hline_d(GLint x1, GLint x2, GLint y, zbuf_t z1, zbuf_t z2, SColor c)
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
  if(x2 >= viewportWidth)
  {
    x2 = viewportWidth - 1;
  }

  short color = RGB(c_fptoi(c.r), c_fptoi(c.g), c_fptoi(c.b));
  unsigned long index((y * viewportWidth) + x1);
  for(GLint x(x1); x < x2; x++)
  {
    if(zbuffer[index] > z)
    {
      zbuffer[index] = z;
      renderSurface->p[index] = color;
    }
    z += mz;
    index++;
  }
}

//-----------------------------------------------------------------------------
// Horizontal Line Fill, smooth colors
void
CContext::hline_s(CEdge & from, CEdge & to, GLint & y)
{
  if(from.x_[y] < to.x_[y])
  {
    GLint dx(to.x_[y] - from.x_[y]);
    GLfixed mr((to.c_[y].r - from.c_[y].r) / dx);
    GLfixed mg((to.c_[y].g - from.c_[y].g) / dx);
    GLfixed mb((to.c_[y].b - from.c_[y].b) / dx);
    GLfixed r(from.c_[y].r);
    GLfixed g(from.c_[y].g);
    GLfixed b(from.c_[y].b);

    unsigned long index((y * viewportWidth) + from.x_[y]);
    for(GLint x(from.x_[y]); x < to.x_[y]; x++)
    {
      if(x >= viewportWidth)
        break;

      if(x >= 0)
        renderSurface->p[index] = RGB(c_fptoi(r), c_fptoi(g), c_fptoi(b));

      r += mr;
      g += mg;
      b += mb;
      index++;
    }
  }
}

//-----------------------------------------------------------------------------
// Horizontal Line Fill, smooth colors, depth test
void
CContext::hline_sd(CEdge & from, CEdge & to, GLint & y)
{
  if(from.x_[y] < to.x_[y])
  {
    GLint dx(to.x_[y] - from.x_[y]);
    zbuf_t mz((to.z_[y] - from.z_[y]) / dx);
    zbuf_t z(from.z_[y]);

    GLfixed mr((to.c_[y].r - from.c_[y].r) / dx);
    GLfixed mg((to.c_[y].g - from.c_[y].g) / dx);
    GLfixed mb((to.c_[y].b - from.c_[y].b) / dx);
    GLfixed r(from.c_[y].r);
    GLfixed g(from.c_[y].g);
    GLfixed b(from.c_[y].b);

    unsigned long index((y * viewportWidth) + from.x_[y]);
    for(GLint x(from.x_[y]); x < to.x_[y]; x++)
    {
      if(x >= viewportWidth)
        break;

      if(x >= 0)
      {
        if(zbuffer[index] > z)
        {
          zbuffer[index] = z;
          renderSurface->p[index] = RGB(c_fptoi(r), c_fptoi(g), c_fptoi(b));
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
CContext::plotPoly(SPolygon & poly)
{
  // ModelView Transformation
  matrixModelView.transform(poly.v[0].v, poly.v[0].v);
  matrixModelView.transform(poly.v[1].v, poly.v[1].v);
  matrixModelView.transform(poly.v[2].v, poly.v[2].v);

  // Projection Transformation
  matrixProjection.transform(poly.v[0].v, poly.v[0].v);
  matrixProjection.transform(poly.v[1].v, poly.v[1].v);
  matrixProjection.transform(poly.v[2].v, poly.v[2].v);

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
  if(enableCapabilities_ & GL_CULL_FACE)
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

    if(bBackFace == true)
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
  edge1->add(vlo, vhi);
  edge2->add(vlo, vmi);
  edge2->add(vmi, vhi);

  CEdge * pEdgeLeft  = edge1;
  CEdge * pEdgeRight = edge2;
  GLint my(vlo->sy+((vhi->sy-vlo->sy)/2));
  if(edge1->x_[my] > edge2->x_[my])
  {
    // Swap
    pEdgeLeft  = edge2;
    pEdgeRight = edge1;
  }

  // Display triangle (horizontal lines forming the triangle)
  switch(shadingModel_)
  {
    case GL_FLAT:
    {
      if(enableCapabilities_ & GL_DEPTH_TEST)
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
      if(enableCapabilities_ & GL_DEPTH_TEST)
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
