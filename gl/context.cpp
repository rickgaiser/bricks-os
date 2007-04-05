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


//-----------------------------------------------------------------------------
CContext::CContext()
 : renderSurface(0)
 , zbuffer(0)
 , beginMode_(GL_POINTS)
 , shadingModel_(GL_FLAT)
 , enableCapabilities_(0)
 , matrixMode_(GL_MODELVIEW)
 , pCurrentMatrix_(&matrixModelView)
 , clearDepth_(m_fpfromi(1))
 , depthFunction_(GL_LESS)
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
CContext::glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
  clClear.r = c_fpfromf(red  ) * 255;
  clClear.g = c_fpfromf(green) * 255;
  clClear.b = c_fpfromf(blue ) * 255;
  clClear.a = c_fpfromf(alpha) * 255;
}

//-----------------------------------------------------------------------------
void
CContext::glClearDepthf(GLclampf depth)
{
  clearDepth_ = m_fpfromf(depth);
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
void
CContext::glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
  pCurrentMatrix_->rotate(m_fpfromf(angle), m_fpfromf(x), m_fpfromf(y), m_fpfromf(z));
}

//-----------------------------------------------------------------------------
void
CContext::glScalef(GLfloat x, GLfloat y, GLfloat z)
{
  pCurrentMatrix_->scale(m_fpfromf(x), m_fpfromf(y), m_fpfromf(z));
}

//-----------------------------------------------------------------------------
void
CContext::glTranslatef(GLfloat x, GLfloat y, GLfloat z)
{
  pCurrentMatrix_->translate(m_fpfromf(x), m_fpfromf(y), m_fpfromf(-z));
}

//-----------------------------------------------------------------------------
void
CContext::glClear(GLbitfield mask)
{
  unsigned short color(RGB(c_fptoi(clClear.r), c_fptoi(clClear.g), c_fptoi(clClear.b)));
  long iCount(viewportByteCount >> 1);

  if(mask & GL_COLOR_BUFFER_BIT)
    dmaFill16(color, renderSurface->p, iCount);
  if(mask & GL_DEPTH_BUFFER_BIT)
    dmaFill32(clearDepth_, zbuffer, iCount);
}

//-----------------------------------------------------------------------------
void
CContext::glClearColorx(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha)
{
  clClear.r = red   * 255;
  clClear.g = green * 255;
  clClear.b = blue  * 255;
  clClear.a = alpha * 255;
}

//-----------------------------------------------------------------------------
void
CContext::glClearDepthx(GLclampx depth)
{
  clearDepth_ = depth;
}

//-----------------------------------------------------------------------------
void
CContext::glColor4ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha)
{
  clCurrent.r = c_fpfromi(red  );
  clCurrent.g = c_fpfromi(green);
  clCurrent.b = c_fpfromi(blue );
  clCurrent.a = c_fpfromi(alpha);
}

//-----------------------------------------------------------------------------
void
CContext::glColor4x(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha)
{
  clCurrent.r = red   * 255;
  clCurrent.g = green * 255;
  clCurrent.b = blue  * 255;
  clCurrent.a = alpha * 255;
}

//-----------------------------------------------------------------------------
void
CContext::glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz)
{
  normal_[0] = m_fpfromf(nx);
  normal_[1] = m_fpfromf(ny);
  normal_[2] = m_fpfromf(nz);
}

//-----------------------------------------------------------------------------
void
CContext::glNormal3x(GLfixed nx, GLfixed ny, GLfixed nz)
{
  normal_[0] = nx;
  normal_[1] = ny;
  normal_[2] = nz;
}

//-----------------------------------------------------------------------------
void
CContext::glNormalPointer(GLenum type, GLsizei stride, const GLvoid * pointer)
{
  bufNormal_.size    = 0;
  bufNormal_.type    = type;
  bufNormal_.stride  = stride;
  bufNormal_.pointer = pointer;
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
CContext::glDisable(GLenum cap)
{
  enableCapabilities_ &= ~cap;
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
CContext::glEnable(GLenum cap)
{
  enableCapabilities_ |= cap;
}

//-----------------------------------------------------------------------------
void
CContext::glEnableClientState(GLenum array)
{
  enableCapabilities_ |= array;
}

//-----------------------------------------------------------------------------
void
CContext::glFlush(void)
{
}

//-----------------------------------------------------------------------------
void
CContext::glLoadIdentity()
{
  pCurrentMatrix_->loadIdentity();
}

//-----------------------------------------------------------------------------
void
CContext::glMatrixMode(GLenum mode)
{
  matrixMode_ = mode;

  switch(mode)
  {
    case GL_MODELVIEW:
      pCurrentMatrix_ = &matrixModelView;
      break;
    case GL_PROJECTION:
      pCurrentMatrix_ = &matrixProjection;
      break;
  };
}

//-----------------------------------------------------------------------------
void
CContext::glRotatex(GLfixed angle, GLfixed x, GLfixed y, GLfixed z)
{
  pCurrentMatrix_->rotate(angle, x, y, z);
}

//-----------------------------------------------------------------------------
void
CContext::glScalex(GLfixed x, GLfixed y, GLfixed z)
{
  pCurrentMatrix_->scale(x, y, z);
}

//-----------------------------------------------------------------------------
void
CContext::glShadeModel(GLenum mode)
{
  shadingModel_ = mode;
}

//-----------------------------------------------------------------------------
void
CContext::glTranslatex(GLfixed x, GLfixed y, GLfixed z)
{
  pCurrentMatrix_->translate(x, y, -z);
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
  zbuffer            = new fxp_zbuf_t[width * height];
  edge1              = new CEdge(viewportHeight);
  edge2              = new CEdge(viewportHeight);

  // Calculate field of view scalars
  fpFieldofviewXScalar = m_fpfromf(static_cast<float>(width)  / tan(80)); // 5.67 == tan(80)
  fpFieldofviewYScalar = m_fpfromf(static_cast<float>(height) / tan(80)); // 5.67 == tan(80)
}

//-----------------------------------------------------------------------------
inline bool
validDepth(GLfixed z, GLfixed zbuf, GLenum zfunc)
{
  switch(zfunc)
  {
    case GL_LESS:     return (z < zbuf);
    case GL_EQUAL:    return (z == zbuf);
    case GL_LEQUAL:   return (z <= zbuf);
    case GL_GREATER:  return (z > zbuf);
    case GL_NOTEQUAL: return (z != zbuf);
    case GL_GEQUAL:   return (z >= zbuf);
    case GL_ALWAYS:   return true;
    case GL_NEVER:
    default:          return false;
  };
}

//-----------------------------------------------------------------------------
// Horizontal Line Fill, depth test
void
CContext::hline(CEdge & from, CEdge & to, GLint & y, SColor c)
{
  if(from.x_[y] < to.x_[y])
  {
    GLint dx(to.x_[y] - from.x_[y]);
    fxp_zbuf_t mz((to.z_[y] - from.z_[y]) / dx);
    fxp_zbuf_t z(from.z_[y]);
    short color(RGB(c_fptoi(c.r), c_fptoi(c.g), c_fptoi(c.b)));

    unsigned long index((y * viewportWidth) + from.x_[y]);
    for(GLint x(from.x_[y]); x < to.x_[y]; x++)
    {
      if(x >= viewportWidth)
        break;

      if(x >= 0)
      {
        // Depth test pixel
        if(enableCapabilities_ & GL_DEPTH_TEST)
        {
          if(validDepth(z, zbuffer[index], depthFunction_))
          {
            zbuffer[index] = z;
            renderSurface->p[index] = color;
          }
          z += mz;
        }
        else
        {
          // No depth testing, always put pixel
          renderSurface->p[index] = color;
        }
      }
      index++;
    }
  }
}

//-----------------------------------------------------------------------------
// Horizontal Line Fill, smooth colors, depth test
void
CContext::hline_s(CEdge & from, CEdge & to, GLint & y)
{
  if(from.x_[y] < to.x_[y])
  {
    GLint dx(to.x_[y] - from.x_[y]);
    fxp_zbuf_t mz((to.z_[y] - from.z_[y]) / dx);
    fxp_zbuf_t z(from.z_[y]);

    fxp_color_t mr((to.c_[y].r - from.c_[y].r) / dx);
    fxp_color_t mg((to.c_[y].g - from.c_[y].g) / dx);
    fxp_color_t mb((to.c_[y].b - from.c_[y].b) / dx);
    fxp_color_t r(from.c_[y].r);
    fxp_color_t g(from.c_[y].g);
    fxp_color_t b(from.c_[y].b);

    unsigned long index((y * viewportWidth) + from.x_[y]);
    for(GLint x(from.x_[y]); x < to.x_[y]; x++)
    {
      if(x >= viewportWidth)
        break;

      if(x >= 0)
      {
        // Depth test pixel
        if(enableCapabilities_ & GL_DEPTH_TEST)
        {
          if(validDepth(z, zbuffer[index], depthFunction_))
          {
            zbuffer[index] = z;
            renderSurface->p[index] = RGB(c_fptoi(r), c_fptoi(g), c_fptoi(b));
          }
          z += mz;
        }
        else
        {
          // No depth testing, always put pixel
          renderSurface->p[index] = RGB(c_fptoi(r), c_fptoi(g), c_fptoi(b));
        }
      }
      r += mr;
      g += mg;
      b += mb;
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
      for(GLint y(vlo->sy); y < vhi->sy; y++)
        hline(*pEdgeLeft, *pEdgeRight, y, poly.v[2].c);
      break;
    }
    case GL_SMOOTH:
    {
      for(GLint y(vlo->sy); y < vhi->sy; y++)
        hline_s(*pEdgeLeft, *pEdgeRight, y);
      break;
    }
  }
}
