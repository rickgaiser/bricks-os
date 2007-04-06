#include "context.h"
#include "matrix.h"
#include "fixedPoint.h"

#include "asm/arch/macros.h"
#include "stdlib.h"
typedef unsigned int wint_t;
#include <math.h>


#define SCREENX(v) (m_fptoi(m_fpdiv(m_fpmul(v[0], fpFieldofviewXScalar), -v[2])) + (viewportWidth  >> 1))
#define SCREENY(v) (m_fptoi(m_fpdiv(m_fpmul(v[1], fpFieldofviewYScalar), -v[2])) + (viewportHeight >> 1))
#define ZBUFFER_MAX_DEPTH z_fpfromi((1 << (FP_PRESICION_ZBUFFER - 1)) - 1)
#define fpRGB(r,g,b) (0x8000 | (((b*255) >> 9) & 0x7c00) | (((g*255) >> 14) & 0x03e0) | (((r*255) >> 19) & 0x001f))


//-----------------------------------------------------------------------------
CContext::CContext()
 : renderSurface(0)
 , zbuffer(0)
 , shadingModel_(GL_FLAT)
 , enableCapabilities_(0)
 , matrixMode_(GL_MODELVIEW)
 , pCurrentMatrix_(&matrixModelView)
 , bLighting_(false)
 , clearDepth_(m_fpfromi(1))
 , depthFunction_(GL_LESS)
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
  clCurrent.a = c_fpfromi(1);

  clClear.r = c_fpfromi(0);
  clClear.g = c_fpfromi(0);
  clClear.b = c_fpfromi(0);
  clClear.a = c_fpfromi(1);

  for(int iLight(0); iLight < 8; iLight++)
  {
    lights_[iLight].ambient.r = c_fpfromf(0.0f);
    lights_[iLight].ambient.g = c_fpfromf(0.0f);
    lights_[iLight].ambient.b = c_fpfromf(0.0f);
    lights_[iLight].ambient.a = c_fpfromf(1.0f);

    lights_[iLight].diffuse.r = c_fpfromf(1.0f);
    lights_[iLight].diffuse.g = c_fpfromf(1.0f);
    lights_[iLight].diffuse.b = c_fpfromf(1.0f);
    lights_[iLight].diffuse.a = c_fpfromf(1.0f);

    lights_[iLight].specular.r = c_fpfromf(0.0f);
    lights_[iLight].specular.g = c_fpfromf(0.0f);
    lights_[iLight].specular.b = c_fpfromf(0.0f);
    lights_[iLight].specular.a = c_fpfromf(1.0f);

    lights_[iLight].enabled = false;
  }
}

//-----------------------------------------------------------------------------
CContext::~CContext()
{
}

//-----------------------------------------------------------------------------
void
CContext::glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
  clClear.r = c_fpfromf(red  );
  clClear.g = c_fpfromf(green);
  clClear.b = c_fpfromf(blue );
  clClear.a = c_fpfromf(alpha);
}

//-----------------------------------------------------------------------------
void
CContext::glClearDepthf(GLclampf depth)
{
  clearDepth_ = z_fpmul(ZBUFFER_MAX_DEPTH, m_fpfromf(depth));
}

//-----------------------------------------------------------------------------
void
CContext::glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
  clCurrent.r = c_fpfromf(red  );
  clCurrent.g = c_fpfromf(green);
  clCurrent.b = c_fpfromf(blue );
  clCurrent.a = c_fpfromf(alpha);
}

//-----------------------------------------------------------------------------
void
CContext::glLightf(GLenum light, GLenum pname, GLfloat param)
{
}

//-----------------------------------------------------------------------------
void
CContext::glLightfv(GLenum light, GLenum pname, const GLfloat * params)
{
  SLight * pLight = 0;
  switch(light)
  {
    case GL_LIGHT0: pLight = &lights_[0]; break;
    case GL_LIGHT1: pLight = &lights_[1]; break;
    case GL_LIGHT2: pLight = &lights_[2]; break;
    case GL_LIGHT3: pLight = &lights_[3]; break;
    case GL_LIGHT4: pLight = &lights_[4]; break;
    case GL_LIGHT5: pLight = &lights_[5]; break;
    case GL_LIGHT6: pLight = &lights_[6]; break;
    case GL_LIGHT7: pLight = &lights_[7]; break;
    default:
      return;
  }

  SColor * pColor = 0;
  switch(pname)
  {
    case GL_AMBIENT:  pColor = &pLight->ambient; break;
    case GL_DIFFUSE:  pColor = &pLight->diffuse; break;
    case GL_SPECULAR: pColor = &pLight->specular; break;
    default:
      return;
  }

  pColor->r = c_fpfromf(params[0]);
  pColor->g = c_fpfromf(params[1]);
  pColor->b = c_fpfromf(params[2]);
  pColor->a = c_fpfromf(params[3]);
}

//-----------------------------------------------------------------------------
void
CContext::glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
  pCurrentMatrix_->rotate(m_fpfromf(angle), m_fpfromf(x), m_fpfromf(y), m_fpfromf(z));
  // FIXME
  matrixRotation.rotate(m_fpfromf(angle), m_fpfromf(x), m_fpfromf(y), m_fpfromf(z));
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
  unsigned short color(fpRGB(clClear.r, clClear.g, clClear.b));
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
  clClear.r = red;
  clClear.g = green;
  clClear.b = blue;
  clClear.a = alpha;
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
  clCurrent.r = c_fpfromi(red  ) / 255;
  clCurrent.g = c_fpfromi(green) / 255;
  clCurrent.b = c_fpfromi(blue ) / 255;
  clCurrent.a = c_fpfromi(alpha) / 255;
}

//-----------------------------------------------------------------------------
void
CContext::glColor4x(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha)
{
  clCurrent.r = red;
  clCurrent.g = green;
  clCurrent.b = blue;
  clCurrent.a = alpha;
}

//-----------------------------------------------------------------------------
void
CContext::glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz)
{
  normal_[0] = m_fpfromf(nx);
  normal_[1] = m_fpfromf(ny);
  normal_[2] = m_fpfromf(nz);

  //if((enableCapabilities_ & GL_NORMALIZE) == GL_NORMALIZE)
  //{
  //  // FIXME: Normalize normal
  //}
}

//-----------------------------------------------------------------------------
void
CContext::glNormal3x(GLfixed nx, GLfixed ny, GLfixed nz)
{
  normal_[0] = nx;
  normal_[1] = ny;
  normal_[2] = nz;

  //if((enableCapabilities_ & GL_NORMALIZE) == GL_NORMALIZE)
  //{
  //  // FIXME: Normalize normal
  //}
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
CContext::glDepthFunc(GLenum func)
{
  depthFunction_ = func;
}

//-----------------------------------------------------------------------------
void
CContext::glDisable(GLenum cap)
{
  switch(cap)
  {
    case GL_LIGHTING: bLighting_ = false; break;
    case GL_LIGHT0: lights_[0].enabled = false; break;
    case GL_LIGHT1: lights_[1].enabled = false; break;
    case GL_LIGHT2: lights_[2].enabled = false; break;
    case GL_LIGHT3: lights_[3].enabled = false; break;
    case GL_LIGHT4: lights_[4].enabled = false; break;
    case GL_LIGHT5: lights_[5].enabled = false; break;
    case GL_LIGHT6: lights_[6].enabled = false; break;
    case GL_LIGHT7: lights_[7].enabled = false; break;
    default:
      enableCapabilities_ &= ~cap;
  };
}

//-----------------------------------------------------------------------------
void
CContext::glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
  switch(mode)
  {
    case GL_TRIANGLES:
    {
      SPolygon polygon;
      SVertex  v[3];
      GLint idxVertex(0);
      GLint idxColor(0);
      GLint idxNormal(0);

      v[0].bProcessed = false;
      v[1].bProcessed = false;
      v[1].bProcessed = false;
      polygon.v[0] = &v[0];
      polygon.v[1] = &v[1];
      polygon.v[2] = &v[2];

      for(GLint i(0); i < count; i++)
      {
        // Vertices
        // v[0]
        v[0].v[0] = m_fpfromf(((GLfloat *)bufVertex_.pointer)[idxVertex++]);
        v[0].v[1] = m_fpfromf(((GLfloat *)bufVertex_.pointer)[idxVertex++]);
        v[0].v[2] = m_fpfromf(((GLfloat *)bufVertex_.pointer)[idxVertex++]);
        // v[1]
        v[1].v[0] = m_fpfromf(((GLfloat *)bufVertex_.pointer)[idxVertex++]);
        v[1].v[1] = m_fpfromf(((GLfloat *)bufVertex_.pointer)[idxVertex++]);
        v[1].v[2] = m_fpfromf(((GLfloat *)bufVertex_.pointer)[idxVertex++]);
        // v[2]
        v[2].v[0] = m_fpfromf(((GLfloat *)bufVertex_.pointer)[idxVertex++]);
        v[2].v[1] = m_fpfromf(((GLfloat *)bufVertex_.pointer)[idxVertex++]);
        v[2].v[2] = m_fpfromf(((GLfloat *)bufVertex_.pointer)[idxVertex++]);

        // Colors
        // v[0]
        v[0].oc.r  = c_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);
        v[0].oc.g  = c_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);
        v[0].oc.b  = c_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);
        v[0].oc.a  = c_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);
        if(shadingModel_ == GL_SMOOTH)
        {
          // v[1]
          v[1].oc.r  = c_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);
          v[1].oc.g  = c_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);
          v[1].oc.b  = c_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);
          v[1].oc.a  = c_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);
          // v[2]
          v[2].oc.r  = c_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);
          v[2].oc.g  = c_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);
          v[2].oc.b  = c_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);
          v[2].oc.a  = c_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);
        }

        // Normal
        //polygon.n[0] = normal_[0];
        //polygon.n[1] = normal_[1];
        //polygon.n[2] = normal_[2];
        polygon.n[0] = c_fpfromf(((GLfloat *)bufNormal_.pointer)[idxNormal++]);
        polygon.n[1] = c_fpfromf(((GLfloat *)bufNormal_.pointer)[idxNormal++]);
        polygon.n[2] = c_fpfromf(((GLfloat *)bufNormal_.pointer)[idxNormal++]);

        // Plot
        plotPoly(polygon);
      }
      break;
    }
    case GL_TRIANGLE_STRIP:
    {
      break;
    }
    case GL_TRIANGLE_FAN:
    {
      SPolygon polygon;
      SVertex  v[3];
      GLint idxVertex(6);
      GLint idxColor(8);
      GLint idxNormal(0);

      v[0].bProcessed = false;
      v[1].bProcessed = false;
      v[2].bProcessed = false;
      polygon.v[0] = &v[0];
      polygon.v[1] = &v[1];
      polygon.v[2] = &v[2];

      // Vertices
      v[0].v[0] = m_fpfromf(((GLfloat *)bufVertex_.pointer)[0]);
      v[0].v[1] = m_fpfromf(((GLfloat *)bufVertex_.pointer)[1]);
      v[0].v[2] = m_fpfromf(((GLfloat *)bufVertex_.pointer)[2]);
      v[2].v[0] = m_fpfromf(((GLfloat *)bufVertex_.pointer)[3]);
      v[2].v[1] = m_fpfromf(((GLfloat *)bufVertex_.pointer)[4]);
      v[2].v[2] = m_fpfromf(((GLfloat *)bufVertex_.pointer)[5]);
      // Colors
      v[0].oc.r  = c_fpfromf(((GLfloat *)bufColor_.pointer)[0]);
      v[0].oc.g  = c_fpfromf(((GLfloat *)bufColor_.pointer)[1]);
      v[0].oc.b  = c_fpfromf(((GLfloat *)bufColor_.pointer)[2]);
      v[0].oc.a  = c_fpfromf(((GLfloat *)bufColor_.pointer)[3]);
      if(shadingModel_ == GL_SMOOTH)
      {
        v[2].oc.r  = c_fpfromf(((GLfloat *)bufColor_.pointer)[4]);
        v[2].oc.g  = c_fpfromf(((GLfloat *)bufColor_.pointer)[5]);
        v[2].oc.b  = c_fpfromf(((GLfloat *)bufColor_.pointer)[6]);
        v[2].oc.a  = c_fpfromf(((GLfloat *)bufColor_.pointer)[7]);
      }

      for(GLint i(0); i < count; i++)
      {
        // Swap v[1] with v[2]
        SVertex * vtemp;
        vtemp = polygon.v[1];
        polygon.v[1] = polygon.v[2];
        polygon.v[2] = vtemp;

        vtemp->bProcessed = false;
        // Vertices
        vtemp->v[0] = m_fpfromf(((GLfloat *)bufVertex_.pointer)[idxVertex++]);
        vtemp->v[1] = m_fpfromf(((GLfloat *)bufVertex_.pointer)[idxVertex++]);
        vtemp->v[2] = m_fpfromf(((GLfloat *)bufVertex_.pointer)[idxVertex++]);
        // Colors
        vtemp->oc.r  = c_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);
        vtemp->oc.g  = c_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);
        vtemp->oc.b  = c_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);
        vtemp->oc.a  = c_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);

        // Normal
        //polygon.n[0] = normal_[0];
        //polygon.n[1] = normal_[1];
        //polygon.n[2] = normal_[2];
        polygon.n[0] = c_fpfromf(((GLfloat *)bufNormal_.pointer)[idxNormal++]);
        polygon.n[1] = c_fpfromf(((GLfloat *)bufNormal_.pointer)[idxNormal++]);
        polygon.n[2] = c_fpfromf(((GLfloat *)bufNormal_.pointer)[idxNormal++]);

        // Plot
        plotPoly(polygon);
      }
      break;
    }
  }
}

//-----------------------------------------------------------------------------
void
CContext::glEnable(GLenum cap)
{
  switch(cap)
  {
    case GL_LIGHTING: bLighting_ = true; break;
    case GL_LIGHT0: lights_[0].enabled = true; break;
    case GL_LIGHT1: lights_[1].enabled = true; break;
    case GL_LIGHT2: lights_[2].enabled = true; break;
    case GL_LIGHT3: lights_[3].enabled = true; break;
    case GL_LIGHT4: lights_[4].enabled = true; break;
    case GL_LIGHT5: lights_[5].enabled = true; break;
    case GL_LIGHT6: lights_[6].enabled = true; break;
    case GL_LIGHT7: lights_[7].enabled = true; break;
    default:
      enableCapabilities_ |= cap;
  };

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
CContext::glLightx(GLenum light, GLenum pname, GLfixed param)
{
}

//-----------------------------------------------------------------------------
void
CContext::glLightxv(GLenum light, GLenum pname, const GLfixed * params)
{
  SLight * pLight = 0;
  switch(light)
  {
    case GL_LIGHT0: pLight = &lights_[0]; break;
    case GL_LIGHT1: pLight = &lights_[1]; break;
    case GL_LIGHT2: pLight = &lights_[2]; break;
    case GL_LIGHT3: pLight = &lights_[3]; break;
    case GL_LIGHT4: pLight = &lights_[4]; break;
    case GL_LIGHT5: pLight = &lights_[5]; break;
    case GL_LIGHT6: pLight = &lights_[6]; break;
    case GL_LIGHT7: pLight = &lights_[7]; break;
    default:
      return;
  }

  SColor * pColor = 0;
  switch(pname)
  {
    case GL_AMBIENT:  pColor = &pLight->ambient; break;
    case GL_DIFFUSE:  pColor = &pLight->diffuse; break;
    case GL_SPECULAR: pColor = &pLight->specular; break;
    default:
      return;
  }

  pColor->r = params[0];
  pColor->g = params[1];
  pColor->b = params[2];
  pColor->a = params[3];
}

//-----------------------------------------------------------------------------
void
CContext::glLoadIdentity()
{
  pCurrentMatrix_->loadIdentity();
  // FIXME
  matrixRotation.loadIdentity();
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
  // FIXME
  matrixRotation.rotate(angle, x, y, z);
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
    short color(fpRGB(c.r, c.g, c.b));

    unsigned long index((y * viewportWidth) + from.x_[y]);
    for(GLint x(from.x_[y]); x < to.x_[y]; x++)
    {
      if(x >= viewportWidth)
        break;

      if(x >= 0)
      {
        // Depth test pixel
        if((enableCapabilities_ & GL_DEPTH_TEST) == GL_DEPTH_TEST)
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
        if((enableCapabilities_ & GL_DEPTH_TEST) == GL_DEPTH_TEST)
        {
          if(validDepth(z, zbuffer[index], depthFunction_))
          {
            zbuffer[index] = z;
            renderSurface->p[index] = fpRGB(r, g, b);
          }
          z += mz;
        }
        else
        {
          // No depth testing, always put pixel
          renderSurface->p[index] = fpRGB(r, g, b);
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
  // Normal Rotation
  matrixRotation.transform(poly.n, poly.n);
  GLfixed normal = -poly.n[2];

  // Backface culling is easy when we have normals
  if(bLighting_ == true)
    if((enableCapabilities_ & GL_CULL_FACE) == GL_CULL_FACE)
      if(normal < 0)
        return;

  for(int i(0); i < 3; i++)
  {
    if(poly.v[i]->bProcessed == false)
    {
      // ModelView Transformation
      matrixModelView.transform(poly.v[i]->v, poly.v[i]->v);
      // Projection Transformation
      matrixProjection.transform(poly.v[i]->v, poly.v[i]->v);
      // Perspective division, viewport transformation
      poly.v[i]->sx = SCREENX(poly.v[i]->v);
      poly.v[i]->sy = SCREENY(poly.v[i]->v);

      poly.v[i]->bProcessed = true;
    }
  }

  // Backface culling, when no normals available
  if(bLighting_ == false)
  {
    if((enableCapabilities_ & GL_CULL_FACE) == GL_CULL_FACE)
    {
      bool bBackFace;
      if((poly.v[1]->sx != poly.v[0]->sx) && (poly.v[2]->sx != poly.v[0]->sx))
      {
        bBackFace = ((((fpfromi(16, poly.v[1]->sy - poly.v[0]->sy) / (poly.v[1]->sx - poly.v[0]->sx)) -
                       (fpfromi(16, poly.v[2]->sy - poly.v[0]->sy) / (poly.v[2]->sx - poly.v[0]->sx))) < 0) ^
                       ((poly.v[0]->sx <= poly.v[1]->sx) == (poly.v[0]->sx > poly.v[2]->sx)));
      }
      else if((poly.v[2]->sx != poly.v[1]->sx) && (poly.v[0]->sx != poly.v[1]->sx))
      {
        bBackFace = ((((fpfromi(16, poly.v[2]->sy - poly.v[1]->sy) / (poly.v[2]->sx - poly.v[1]->sx)) -
                       (fpfromi(16, poly.v[0]->sy - poly.v[1]->sy) / (poly.v[0]->sx - poly.v[1]->sx))) < 0) ^
                       ((poly.v[1]->sx <= poly.v[2]->sx) == (poly.v[1]->sx > poly.v[0]->sx)));
      }
      else if((poly.v[0]->sx != poly.v[2]->sx) && (poly.v[1]->sx != poly.v[2]->sx))
      {
        bBackFace = ((((fpfromi(16, poly.v[0]->sy - poly.v[2]->sy) / (poly.v[0]->sx - poly.v[2]->sx)) -
                       (fpfromi(16, poly.v[1]->sy - poly.v[2]->sy) / (poly.v[1]->sx - poly.v[2]->sx))) < 0) ^
                       ((poly.v[2]->sx <= poly.v[0]->sx) == (poly.v[2]->sx > poly.v[1]->sx)));
      }
      else
      {
        // Triangle invisible
        return;
      }

      if(bBackFace == true)
        return;
    }
  }

  // Lighting
  if(bLighting_ == true)
  {
    for(int iLight(0); iLight < 8; iLight++)
    {
      if(lights_[iLight].enabled == true)
      {
        SColor & ambient = lights_[iLight].ambient;
        SColor & diffuse = lights_[iLight].diffuse;
        poly.v[0]->cc.r = c_max(c_fpmul(poly.v[0]->oc.r, ambient.r) + c_fpmul(c_fpmul(poly.v[0]->oc.r, normal), diffuse.r));
        poly.v[0]->cc.g = c_max(c_fpmul(poly.v[0]->oc.g, ambient.g) + c_fpmul(c_fpmul(poly.v[0]->oc.g, normal), diffuse.g));
        poly.v[0]->cc.b = c_max(c_fpmul(poly.v[0]->oc.b, ambient.b) + c_fpmul(c_fpmul(poly.v[0]->oc.b, normal), diffuse.b));
        if(shadingModel_ == GL_SMOOTH)
        {
          poly.v[1]->cc.r = c_max(c_fpmul(poly.v[1]->oc.r, ambient.r) + c_fpmul(c_fpmul(poly.v[1]->oc.r, normal), diffuse.r));
          poly.v[1]->cc.g = c_max(c_fpmul(poly.v[1]->oc.g, ambient.g) + c_fpmul(c_fpmul(poly.v[1]->oc.g, normal), diffuse.g));
          poly.v[1]->cc.b = c_max(c_fpmul(poly.v[1]->oc.b, ambient.b) + c_fpmul(c_fpmul(poly.v[1]->oc.b, normal), diffuse.b));
          poly.v[2]->cc.r = c_max(c_fpmul(poly.v[2]->oc.r, ambient.r) + c_fpmul(c_fpmul(poly.v[2]->oc.r, normal), diffuse.r));
          poly.v[2]->cc.g = c_max(c_fpmul(poly.v[2]->oc.g, ambient.g) + c_fpmul(c_fpmul(poly.v[2]->oc.g, normal), diffuse.g));
          poly.v[2]->cc.b = c_max(c_fpmul(poly.v[2]->oc.b, ambient.b) + c_fpmul(c_fpmul(poly.v[2]->oc.b, normal), diffuse.b));
        }
      }
    }
  }

  // Bubble sort the 3 vertexes
  SVertex * vtemp;
  SVertex * vhi(poly.v[0]);
  SVertex * vmi(poly.v[1]);
  SVertex * vlo(poly.v[2]);

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
        hline(*pEdgeLeft, *pEdgeRight, y, poly.v[2]->cc);
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
