#include "softGLF.h"
#include "matrix.h"
#include "fixedPoint.h"

#include "asm/arch/macros.h"
#include "stdlib.h"
typedef unsigned int wint_t;
#include <math.h>


#define SCREENX(v) ((int)((v[0] * fpFieldofviewXScalar) / -v[2]) + (viewportWidth  >> 1))
#define SCREENY(v) ((int)((v[1] * fpFieldofviewYScalar) / -v[2]) + (viewportHeight >> 1))
#define fpRGB(r,g,b) (0x8000 | \
                      (((unsigned int)(b*255) << 7) & 0x7c00) | \
                      (((unsigned int)(g*255) << 2) & 0x03e0) | \
                      (((unsigned int)(r*255) >> 3) & 0x001f))


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CEdgeF::CEdgeF(uint32_t height)
 : iHeight_(height)
{
  x_ = new GLint[iHeight_];
  z_ = new GLfloat[iHeight_];
  c_ = new SColorF[iHeight_];
}

//-----------------------------------------------------------------------------
CEdgeF::~CEdgeF()
{
  delete x_;
  delete z_;
  delete c_;
}

//-----------------------------------------------------------------------------
void
CEdgeF::add(SVertexF * vfrom, SVertexF * vto, GLenum shadingModel)
{
  if(vto->sy != vfrom->sy)
  {
    GLfloat dy(vto->sy - vfrom->sy);

    GLfloat x(vfrom->sx);
    GLfloat mx((vto->sx  - vfrom->sx) / dy);

    GLfloat z(vfrom->v2[2]);
    GLfloat mz((vto->v2[2] - vfrom->v2[2]) / dy);

    switch(shadingModel)
    {
      case GL_FLAT:
      {
        int yfrom = (vfrom->sy < 0) ? 0 : (vfrom->sy);
        int yto   = (vto->sy >= iHeight_) ? (iHeight_ - 1) : (vto->sy);
        for(int y(yfrom); y < yto; y++)
        {
          x_[y] = (GLint)x;
          z_[y] = z;

          x += mx;
          z += mz;
        }
        break;
      }
      case GL_SMOOTH:
      {
        GLfloat r(vfrom->c2.r);
        GLfloat g(vfrom->c2.g);
        GLfloat b(vfrom->c2.b);
        GLfloat mr((vto->c2.r - vfrom->c2.r) / dy);
        GLfloat mg((vto->c2.g - vfrom->c2.g) / dy);
        GLfloat mb((vto->c2.b - vfrom->c2.b) / dy);

        for(int y(vfrom->sy); y < vto->sy; y++)
        {
          if(y >= iHeight_)
            break;

          if(y >= 0)
          {
            x_[y]   = (GLint)x;
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
//-----------------------------------------------------------------------------
CSoftGLESFloat::CSoftGLESFloat()
 : renderSurface(0)
 , zbuffer(0)
 , shadingModel_(GL_FLAT)
 , cullFaceEnabled_(false)
 , cullFaceMode_(GL_BACK)
 , matrixMode_(GL_MODELVIEW)
 , pCurrentMatrix_(&matrixModelView)
 , bBufVertexEnabled_(false)
 , bBufNormalEnabled_(false)
 , bBufColorEnabled_(false)
 , lightingEnabled_(false)
 , fogEnabled_(false)
 , depthTestEnabled_(false)
 , depthFunction_(GL_LESS)
 , depthClear_(1.0f)
 , edge1(0)
 , edge2(0)
 , viewportXOffset(0)
 , viewportYOffset(0)
 , viewportPixelCount(0)
 , viewportByteCount(0)
 , viewportWidth(0)
 , viewportHeight(0)
 , fpFieldofviewXScalar(1.0f)
 , fpFieldofviewYScalar(1.0f)
{
  clCurrent.r = 0.0f;
  clCurrent.g = 0.0f;
  clCurrent.b = 0.0f;
  clCurrent.a = 1.0f;

  clClear.r = 0.0f;
  clClear.g = 0.0f;
  clClear.b = 0.0f;
  clClear.a = 1.0f;

  for(int iLight(0); iLight < 8; iLight++)
  {
    lights_[iLight].ambient.r = 0.0f;
    lights_[iLight].ambient.g = 0.0f;
    lights_[iLight].ambient.b = 0.0f;
    lights_[iLight].ambient.a = 1.0f;

    lights_[iLight].diffuse.r = 0.0f;
    lights_[iLight].diffuse.g = 0.0f;
    lights_[iLight].diffuse.b = 0.0f;
    lights_[iLight].diffuse.a = 1.0f;

    lights_[iLight].specular.r = 0.0f;
    lights_[iLight].specular.g = 0.0f;
    lights_[iLight].specular.b = 0.0f;
    lights_[iLight].specular.a = 1.0f;

    lights_[iLight].enabled = false;
  }
}

//-----------------------------------------------------------------------------
CSoftGLESFloat::~CSoftGLESFloat()
{
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::setSurface(CSurface * surface)
{
  renderSurface = surface;
}

//-----------------------------------------------------------------------------
CSurface *
CSoftGLESFloat::getSurface()
{
  return renderSurface;
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::glClear(GLbitfield mask)
{
  long iCount(viewportByteCount >> 1);

  if(mask & GL_COLOR_BUFFER_BIT)
  {
    unsigned short color(fpRGB(clClear.r, clClear.g, clClear.b));

    for(int i(0); i < iCount; i++)
      ((uint16_t *)renderSurface->p)[i] = color;
    //dmaFill16(color, renderSurface->p, iCount);
  }
  if(mask & GL_DEPTH_BUFFER_BIT)
  {
    for(int i(0); i < iCount; i++)
      zbuffer[i] = depthClear_ * 100.0f;
    //dmaFill32(depthClear_ * 100.0f, zbuffer, iCount);
  }
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
  clClear.r = red;
  clClear.g = green;
  clClear.b = blue;
  clClear.a = alpha;
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::glClearDepthf(GLclampf depth)
{
  depthClear_ = depth;
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::glColor4ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha)
{
  clCurrent.r = (GLfloat)red   / 255.0f;
  clCurrent.g = (GLfloat)green / 255.0f;
  clCurrent.b = (GLfloat)blue  / 255.0f;
  clCurrent.a = (GLfloat)alpha / 255.0f;
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
  clCurrent.r = red;
  clCurrent.g = green;
  clCurrent.b = blue;
  clCurrent.a = alpha;
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz)
{
  normal_[0] = nx;
  normal_[1] = ny;
  normal_[2] = nz;
  normal_[3] = 1.0f;

  //if((enableCapabilities_ & GL_NORMALIZE) == GL_NORMALIZE)
  //{
  //  // FIXME: Normalize normal
  //}
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::glNormalPointer(GLenum type, GLsizei stride, const GLvoid * pointer)
{
  bufNormal_.size    = 0;
  bufNormal_.type    = type;
  bufNormal_.stride  = stride;
  bufNormal_.pointer = pointer;
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
  bufColor_.size    = size;
  bufColor_.type    = type;
  bufColor_.stride  = stride;
  bufColor_.pointer = pointer;
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::glCullFace(GLenum mode)
{
  cullFaceMode_ = mode;
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::glDepthFunc(GLenum func)
{
  depthFunction_ = func;
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::glDisable(GLenum cap)
{
  switch(cap)
  {
    case GL_LIGHTING: lightingEnabled_ = false; break;
    case GL_LIGHT0: lights_[0].enabled = false; break;
    case GL_LIGHT1: lights_[1].enabled = false; break;
    case GL_LIGHT2: lights_[2].enabled = false; break;
    case GL_LIGHT3: lights_[3].enabled = false; break;
    case GL_LIGHT4: lights_[4].enabled = false; break;
    case GL_LIGHT5: lights_[5].enabled = false; break;
    case GL_LIGHT6: lights_[6].enabled = false; break;
    case GL_LIGHT7: lights_[7].enabled = false; break;

    case GL_DEPTH_TEST: depthTestEnabled_ = false; break;
    case GL_CULL_FACE:  cullFaceEnabled_  = false; break;
    case GL_FOG:        fogEnabled_ = false; break;

    default:
      ; // Not supported
  };
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::glDisableClientState(GLenum array)
{
  switch(array)
  {
    case GL_VERTEX_ARRAY: bBufVertexEnabled_ = false; break;
    case GL_NORMAL_ARRAY: bBufNormalEnabled_ = false; break;
    case GL_COLOR_ARRAY:  bBufColorEnabled_  = false; break;

    default:
      ; // Not supported
  };
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
  if(bBufVertexEnabled_ == false)
    return;

  switch(mode)
  {
    case GL_TRIANGLES:
    {
      SPolygonF polygon;
      SVertexF  v[3];
      GLint idxVertex(0);
      GLint idxColor(0);
      GLint idxNormal(0);

      polygon.v[0] = &v[0];
      polygon.v[1] = &v[1];
      polygon.v[2] = &v[2];

      for(GLint i(0); i < count; i++)
      {
        v[0].bProcessed = false;
        v[1].bProcessed = false;
        v[2].bProcessed = false;

        // Vertices
        v[0].v1[0] = ((GLfloat *)bufVertex_.pointer)[idxVertex++];
        v[0].v1[1] = ((GLfloat *)bufVertex_.pointer)[idxVertex++];
        v[0].v1[2] = ((GLfloat *)bufVertex_.pointer)[idxVertex++];
        v[0].v1[3] = 1.0f;
        v[1].v1[0] = ((GLfloat *)bufVertex_.pointer)[idxVertex++];
        v[1].v1[1] = ((GLfloat *)bufVertex_.pointer)[idxVertex++];
        v[1].v1[2] = ((GLfloat *)bufVertex_.pointer)[idxVertex++];
        v[1].v1[3] = 1.0f;
        v[2].v1[0] = ((GLfloat *)bufVertex_.pointer)[idxVertex++];
        v[2].v1[1] = ((GLfloat *)bufVertex_.pointer)[idxVertex++];
        v[2].v1[2] = ((GLfloat *)bufVertex_.pointer)[idxVertex++];
        v[2].v1[3] = 1.0f;

        // Normals
        if(bBufNormalEnabled_ == true)
        {
          // Use normals from array
          v[0].n1[0] = ((GLfloat *)bufNormal_.pointer)[idxNormal++];
          v[0].n1[1] = ((GLfloat *)bufNormal_.pointer)[idxNormal++];
          v[0].n1[2] = ((GLfloat *)bufNormal_.pointer)[idxNormal++];
          v[0].n1[3] = 1.0f;
          v[1].n1[0] = ((GLfloat *)bufNormal_.pointer)[idxNormal++];
          v[1].n1[1] = ((GLfloat *)bufNormal_.pointer)[idxNormal++];
          v[1].n1[2] = ((GLfloat *)bufNormal_.pointer)[idxNormal++];
          v[1].n1[3] = 1.0f;
          v[2].n1[0] = ((GLfloat *)bufNormal_.pointer)[idxNormal++];
          v[2].n1[1] = ((GLfloat *)bufNormal_.pointer)[idxNormal++];
          v[2].n1[2] = ((GLfloat *)bufNormal_.pointer)[idxNormal++];
          v[2].n1[3] = 1.0f;
        }
        else
        {
          // Use current normal
          v[0].n1[0] = normal_[0]; v[0].n1[1] = normal_[1]; v[0].n1[2] = normal_[2]; v[0].n1[3] = normal_[3];
          v[1].n1[0] = normal_[0]; v[1].n1[1] = normal_[1]; v[1].n1[2] = normal_[2]; v[1].n1[3] = normal_[3];
          v[2].n1[0] = normal_[0]; v[2].n1[1] = normal_[1]; v[2].n1[2] = normal_[2]; v[2].n1[3] = normal_[3];
        }

        // Colors
        if(bBufColorEnabled_ == true)
        {
          // Use colors from array
          v[0].c1.r  = ((GLfloat *)bufColor_.pointer)[idxColor++];
          v[0].c1.g  = ((GLfloat *)bufColor_.pointer)[idxColor++];
          v[0].c1.b  = ((GLfloat *)bufColor_.pointer)[idxColor++];
          v[0].c1.a  = ((GLfloat *)bufColor_.pointer)[idxColor++];
          if(shadingModel_ == GL_SMOOTH)
          {
            v[1].c1.r  = ((GLfloat *)bufColor_.pointer)[idxColor++];
            v[1].c1.g  = ((GLfloat *)bufColor_.pointer)[idxColor++];
            v[1].c1.b  = ((GLfloat *)bufColor_.pointer)[idxColor++];
            v[1].c1.a  = ((GLfloat *)bufColor_.pointer)[idxColor++];
            v[2].c1.r  = ((GLfloat *)bufColor_.pointer)[idxColor++];
            v[2].c1.g  = ((GLfloat *)bufColor_.pointer)[idxColor++];
            v[2].c1.b  = ((GLfloat *)bufColor_.pointer)[idxColor++];
            v[2].c1.a  = ((GLfloat *)bufColor_.pointer)[idxColor++];
          }
        }
        else
        {
          // Use current color
          v[0].c1 = clCurrent;
          v[1].c1 = clCurrent;
          v[2].c1 = clCurrent;
        }

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
      SPolygonF polygon;
      SVertexF  v[3];
      GLint idxVertex(6);
      GLint idxColor(8);
      GLint idxNormal(6);

      v[0].bProcessed = false;
      v[1].bProcessed = false;
      v[2].bProcessed = false;
      polygon.v[0] = &v[0];
      polygon.v[1] = &v[1];
      polygon.v[2] = &v[2];

      // Vertices
      v[0].v1[0] = ((GLfloat *)bufVertex_.pointer)[0];
      v[0].v1[1] = ((GLfloat *)bufVertex_.pointer)[1];
      v[0].v1[2] = ((GLfloat *)bufVertex_.pointer)[2];
      v[0].v1[3] = 1.0f;
      v[2].v1[0] = ((GLfloat *)bufVertex_.pointer)[3];
      v[2].v1[1] = ((GLfloat *)bufVertex_.pointer)[4];
      v[2].v1[2] = ((GLfloat *)bufVertex_.pointer)[5];
      v[2].v1[3] = 1.0f;

      // Normals
      if(bBufNormalEnabled_ == true)
      {
        // Use normals from array
        v[0].n1[0] = ((GLfloat *)bufNormal_.pointer)[0];
        v[0].n1[1] = ((GLfloat *)bufNormal_.pointer)[1];
        v[0].n1[2] = ((GLfloat *)bufNormal_.pointer)[2];
        v[2].n1[3] = 1.0f;
        v[2].n1[0] = ((GLfloat *)bufNormal_.pointer)[3];
        v[2].n1[1] = ((GLfloat *)bufNormal_.pointer)[4];
        v[2].n1[2] = ((GLfloat *)bufNormal_.pointer)[5];
        v[2].n1[3] = 1.0f;
      }
      else
      {
        // Use current normal
        v[0].n1[0] = normal_[0]; v[0].n1[1] = normal_[1]; v[0].n1[2] = normal_[2]; v[0].n1[3] = normal_[3];
        v[1].n1[0] = normal_[0]; v[1].n1[1] = normal_[1]; v[1].n1[2] = normal_[2]; v[1].n1[3] = normal_[3];
        v[2].n1[0] = normal_[0]; v[2].n1[1] = normal_[1]; v[2].n1[2] = normal_[2]; v[2].n1[3] = normal_[3];
      }

      // Colors
      if(bBufColorEnabled_ == true)
      {
        // Use colors from array
        v[0].c1.r  = ((GLfloat *)bufColor_.pointer)[0];
        v[0].c1.g  = ((GLfloat *)bufColor_.pointer)[1];
        v[0].c1.b  = ((GLfloat *)bufColor_.pointer)[2];
        v[0].c1.a  = ((GLfloat *)bufColor_.pointer)[3];
        if(shadingModel_ == GL_SMOOTH)
        {
          v[2].c1.r  = ((GLfloat *)bufColor_.pointer)[4];
          v[2].c1.g  = ((GLfloat *)bufColor_.pointer)[5];
          v[2].c1.b  = ((GLfloat *)bufColor_.pointer)[6];
          v[2].c1.a  = ((GLfloat *)bufColor_.pointer)[7];
        }
      }
      else
      {
        // Use current color
        v[0].c1 = clCurrent;
        v[1].c1 = clCurrent;
        v[2].c1 = clCurrent;
      }

      for(GLint i(0); i < count; i++)
      {
        // Swap v[1] with v[2]
        SVertexF * vtemp;
        vtemp = polygon.v[1];
        polygon.v[1] = polygon.v[2];
        polygon.v[2] = vtemp;

        vtemp->bProcessed = false;

        // Vertices
        vtemp->v1[0] = ((GLfloat *)bufVertex_.pointer)[idxVertex++];
        vtemp->v1[1] = ((GLfloat *)bufVertex_.pointer)[idxVertex++];
        vtemp->v1[2] = ((GLfloat *)bufVertex_.pointer)[idxVertex++];
        vtemp->v1[3] = 1.0f;

        // Normals
        if(bBufNormalEnabled_ == true)
        {
          // Normal
          vtemp->n1[0] = ((GLfloat *)bufNormal_.pointer)[idxNormal++];
          vtemp->n1[1] = ((GLfloat *)bufNormal_.pointer)[idxNormal++];
          vtemp->n1[2] = ((GLfloat *)bufNormal_.pointer)[idxNormal++];
          vtemp->n1[3] = 1.0f;
        }

        // Colors
        if(bBufColorEnabled_ == true)
        {
          vtemp->c1.r  = ((GLfloat *)bufColor_.pointer)[idxColor++];
          vtemp->c1.g  = ((GLfloat *)bufColor_.pointer)[idxColor++];
          vtemp->c1.b  = ((GLfloat *)bufColor_.pointer)[idxColor++];
          vtemp->c1.a  = ((GLfloat *)bufColor_.pointer)[idxColor++];
        }

        // Plot
        plotPoly(polygon);
      }
      break;
    }
  }
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::glEnable(GLenum cap)
{
  switch(cap)
  {
    case GL_LIGHTING: lightingEnabled_ = true; break;
    case GL_LIGHT0: lights_[0].enabled = true; break;
    case GL_LIGHT1: lights_[1].enabled = true; break;
    case GL_LIGHT2: lights_[2].enabled = true; break;
    case GL_LIGHT3: lights_[3].enabled = true; break;
    case GL_LIGHT4: lights_[4].enabled = true; break;
    case GL_LIGHT5: lights_[5].enabled = true; break;
    case GL_LIGHT6: lights_[6].enabled = true; break;
    case GL_LIGHT7: lights_[7].enabled = true; break;

    case GL_DEPTH_TEST: depthTestEnabled_ = true; break;
    case GL_CULL_FACE:  cullFaceEnabled_  = true; break;
    case GL_FOG:        fogEnabled_ = true; break;

    default:
      ; // Not supported
  };
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::glEnableClientState(GLenum array)
{
  switch(array)
  {
    case GL_VERTEX_ARRAY: bBufVertexEnabled_ = true; break;
    case GL_NORMAL_ARRAY: bBufNormalEnabled_ = true; break;
    case GL_COLOR_ARRAY:  bBufColorEnabled_  = true; break;

    default:
      ; // Not supported
  };
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::glFlush(void)
{
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::glFogf(GLenum pname, GLfloat param)
{
  switch(pname)
  {
    case GL_FOG_DENSITY:
      fogDensity_ = param;
      break;
    case GL_FOG_START:
      fogStart_ = param;
      break;
    case GL_FOG_END:
      fogEnd_ = param;
      break;
    case GL_FOG_MODE:
      break;
  };
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::glFogfv(GLenum pname, const GLfloat * params)
{
  switch(pname)
  {
    case GL_FOG_COLOR:
      fogColor_.r = params[0];
      fogColor_.g = params[1];
      fogColor_.b = params[2];
      fogColor_.a = params[3];
      break;
  };
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::glLightf(GLenum light, GLenum pname, GLfloat param)
{
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::glLightfv(GLenum light, GLenum pname, const GLfloat * params)
{
  SLightF * pLight = 0;
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

  SColorF * pColor = 0;
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
CSoftGLESFloat::glLoadIdentity()
{
  pCurrentMatrix_->loadIdentity();
  // FIXME
  matrixRotation.loadIdentity();
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::glMatrixMode(GLenum mode)
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
CSoftGLESFloat::glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
  pCurrentMatrix_->rotate(angle, x, y, z);
  // FIXME
  matrixRotation.rotate(angle, x, y, z);
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::glScalef(GLfloat x, GLfloat y, GLfloat z)
{
  pCurrentMatrix_->scale(x, y, z);
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::glShadeModel(GLenum mode)
{
  shadingModel_ = mode;
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::glTranslatef(GLfloat x, GLfloat y, GLfloat z)
{
  pCurrentMatrix_->translate(x, y, -z);
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
  bufVertex_.size    = size;
  bufVertex_.type    = type;
  bufVertex_.stride  = stride;
  bufVertex_.pointer = pointer;
}

//-----------------------------------------------------------------------------
void
CSoftGLESFloat::glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
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
  zbuffer            = new GLfloat[width * height];
  edge1              = new CEdgeF(viewportHeight);
  edge2              = new CEdgeF(viewportHeight);

  // Calculate field of view scalars
  //fpFieldofviewXScalar = gl_fpfromf(static_cast<float>(width)  / tan(80)); // 5.67 == tan(80)
  //fpFieldofviewYScalar = gl_fpfromf(static_cast<float>(height) / tan(80)); // 5.67 == tan(80)
  fpFieldofviewYScalar = viewportHeight;
  fpFieldofviewXScalar = viewportWidth;
  matrixPerspective.loadIdentity();
}

//-----------------------------------------------------------------------------
inline bool
validDepth(GLfloat z, GLfloat zbuf, GLenum zfunc)
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
// Horizontal Line Fill, flat colors
void
CSoftGLESFloat::hline(CEdgeF & from, CEdgeF & to, GLint & y, SColorF c)
{
  if(from.x_[y] < to.x_[y])
  {
    GLint dx(to.x_[y] - from.x_[y]);
    GLfloat mz((to.z_[y] - from.z_[y]) / dx);
    GLfloat z(from.z_[y]);
    short color(fpRGB(c.r, c.g, c.b));

    unsigned long index((y * viewportWidth) + from.x_[y]);
    for(GLint x(from.x_[y]); x < to.x_[y]; x++)
    {
      if(x >= viewportWidth)
        break;

      if(x >= 0)
      {
        // Depth test pixel
        if(depthTestEnabled_ == true)
        {
          if(validDepth(z, zbuffer[index], depthFunction_))
          {
            zbuffer[index] = z;
            ((uint16_t *)renderSurface->p)[index] = color;
          }
          z += mz;
        }
        else
        {
          // No depth testing, always put pixel
          ((uint16_t *)renderSurface->p)[index] = color;
        }
      }
      index++;
    }
  }
}

//-----------------------------------------------------------------------------
// Horizontal Line Fill, smooth colors
void
CSoftGLESFloat::hline_s(CEdgeF & from, CEdgeF & to, GLint & y)
{
  if(from.x_[y] < to.x_[y])
  {
    GLint dx(to.x_[y] - from.x_[y]);
    GLfloat mz((to.z_[y] - from.z_[y]) / dx);
    GLfloat z(from.z_[y]);

    GLfloat mr((to.c_[y].r - from.c_[y].r) / dx);
    GLfloat mg((to.c_[y].g - from.c_[y].g) / dx);
    GLfloat mb((to.c_[y].b - from.c_[y].b) / dx);
    GLfloat r(from.c_[y].r);
    GLfloat g(from.c_[y].g);
    GLfloat b(from.c_[y].b);

    unsigned long index((y * viewportWidth) + from.x_[y]);
    for(GLint x(from.x_[y]); x < to.x_[y]; x++)
    {
      if(x >= viewportWidth)
        break;

      if(x >= 0)
      {
        // Depth test pixel
        if(depthTestEnabled_ == true)
        {
          if(validDepth(z, zbuffer[index], depthFunction_))
          {
            zbuffer[index] = z;
            ((uint16_t *)renderSurface->p)[index] = fpRGB(r, g, b);
          }
          z += mz;
        }
        else
        {
          // No depth testing, always put pixel
          ((uint16_t *)renderSurface->p)[index] = fpRGB(r, g, b);
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
CSoftGLESFloat::plotPoly(SPolygonF & poly)
{
  for(int i(0); i < 3; i++)
  {
    if(poly.v[i]->bProcessed == false)
    {
      // ModelView Transformation
      matrixModelView.transform(poly.v[i]->v1, poly.v[i]->v2);
      // Projection Transformation
      matrixProjection.transform(poly.v[i]->v2, poly.v[i]->v2);
      // Perspective division, viewport transformation
      matrixPerspective.transform(poly.v[i]->v2, poly.v[i]->v2);
      poly.v[i]->sx = SCREENX(poly.v[i]->v2);
      poly.v[i]->sy = SCREENY(poly.v[i]->v2);

      poly.v[i]->bProcessed = true;
    }
  }

  // Backface culling
  if(cullFaceEnabled_ == true)
  {
    bool bBackFace;
    if((poly.v[1]->sx != poly.v[0]->sx) && (poly.v[2]->sx != poly.v[0]->sx))
    {
      bBackFace = (((((poly.v[1]->sy - poly.v[0]->sy) / (poly.v[1]->sx - poly.v[0]->sx)) -
                     ((poly.v[2]->sy - poly.v[0]->sy) / (poly.v[2]->sx - poly.v[0]->sx))) < 0) ^
                     ((poly.v[0]->sx <= poly.v[1]->sx) == (poly.v[0]->sx > poly.v[2]->sx)));
    }
    else if((poly.v[2]->sx != poly.v[1]->sx) && (poly.v[0]->sx != poly.v[1]->sx))
    {
      bBackFace = (((((poly.v[2]->sy - poly.v[1]->sy) / (poly.v[2]->sx - poly.v[1]->sx)) -
                     ((poly.v[0]->sy - poly.v[1]->sy) / (poly.v[0]->sx - poly.v[1]->sx))) < 0) ^
                     ((poly.v[1]->sx <= poly.v[2]->sx) == (poly.v[1]->sx > poly.v[0]->sx)));
    }
    else if((poly.v[0]->sx != poly.v[2]->sx) && (poly.v[1]->sx != poly.v[2]->sx))
    {
      bBackFace = (((((poly.v[0]->sy - poly.v[2]->sy) / (poly.v[0]->sx - poly.v[2]->sx)) -
                     ((poly.v[1]->sy - poly.v[2]->sy) / (poly.v[1]->sx - poly.v[2]->sx))) < 0) ^
                     ((poly.v[2]->sx <= poly.v[0]->sx) == (poly.v[2]->sx > poly.v[1]->sx)));
    }
    else
    {
      // Triangle invisible
      return;
    }

    switch(cullFaceMode_)
    {
      case GL_FRONT:
        if(bBackFace == false)
          return;
        break;
      case GL_BACK:
        if(bBackFace == true)
          return;
        break;
      case GL_FRONT_AND_BACK:
      default:
        return;
    };
  }

  // Lighting
  if(lightingEnabled_ == true)
  {
    // Normal Rotation
    matrixRotation.transform(poly.v[0]->n1, poly.v[0]->n2);
    matrixRotation.transform(poly.v[1]->n1, poly.v[1]->n2);
    matrixRotation.transform(poly.v[2]->n1, poly.v[2]->n2);
    // FIXME: Light value of normal
    GLfloat normal[3] = {abs(poly.v[0]->n2[2]), abs(poly.v[1]->n2[2]), abs(poly.v[2]->n2[2])};

    for(int iLight(0); iLight < 8; iLight++)
    {
      if(lights_[iLight].enabled == true)
      {
        SColorF & ambient = lights_[iLight].ambient;
        SColorF & diffuse = lights_[iLight].diffuse;

        poly.v[0]->c2.r = gl_fpclamp((poly.v[0]->c1.r * ambient.r) + ((poly.v[0]->c1.r * normal[0]) * diffuse.r));
        poly.v[0]->c2.g = gl_fpclamp((poly.v[0]->c1.g * ambient.g) + ((poly.v[0]->c1.g * normal[0]) * diffuse.g));
        poly.v[0]->c2.b = gl_fpclamp((poly.v[0]->c1.b * ambient.b) + ((poly.v[0]->c1.b * normal[0]) * diffuse.b));

        if(shadingModel_ == GL_SMOOTH)
        {
          poly.v[1]->c2.r = gl_fpclamp((poly.v[1]->c1.r * ambient.r) + ((poly.v[1]->c1.r * normal[1]) * diffuse.r));
          poly.v[1]->c2.g = gl_fpclamp((poly.v[1]->c1.g * ambient.g) + ((poly.v[1]->c1.g * normal[1]) * diffuse.g));
          poly.v[1]->c2.b = gl_fpclamp((poly.v[1]->c1.b * ambient.b) + ((poly.v[1]->c1.b * normal[1]) * diffuse.b));

          poly.v[2]->c2.r = gl_fpclamp((poly.v[2]->c1.r * ambient.r) + ((poly.v[2]->c1.r * normal[2]) * diffuse.r));
          poly.v[2]->c2.g = gl_fpclamp((poly.v[2]->c1.g * ambient.g) + ((poly.v[2]->c1.g * normal[2]) * diffuse.g));
          poly.v[2]->c2.b = gl_fpclamp((poly.v[2]->c1.b * ambient.b) + ((poly.v[2]->c1.b * normal[2]) * diffuse.b));
        }
      }
    }
  }
  else
  {
    // No lighting, lust copy colors
    for(int i(0); i < 3; i++)
    {
      poly.v[i]->c2.r = poly.v[i]->c1.r;
      poly.v[i]->c2.g = poly.v[i]->c1.g;
      poly.v[i]->c2.b = poly.v[i]->c1.b;
      poly.v[i]->c2.a = poly.v[i]->c1.a;
    }
  }

  // Fog
  if(fogEnabled_ == true)
  {
    for(int i(0); i < 3; i++)
    {
      GLfloat partFog   = gl_fpclamp((abs(poly.v[i]->v2[2]) - fogStart_) / (fogEnd_ - fogStart_));
      GLfloat partColor = 1.0f - partFog;
      poly.v[i]->c2.r = gl_fpclamp((poly.v[i]->c2.r * partColor) + (fogColor_.r * partFog));
      poly.v[i]->c2.g = gl_fpclamp((poly.v[i]->c2.g * partColor) + (fogColor_.g * partFog));
      poly.v[i]->c2.b = gl_fpclamp((poly.v[i]->c2.b * partColor) + (fogColor_.b * partFog));
    }
  }

  // Bubble sort the 3 vertexes
  SVertexF * vtemp;
  SVertexF * vhi(poly.v[0]);
  SVertexF * vmi(poly.v[1]);
  SVertexF * vlo(poly.v[2]);

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
  edge1->add(vlo, vhi, shadingModel_);
  edge2->add(vlo, vmi, shadingModel_);
  edge2->add(vmi, vhi, shadingModel_);

  CEdgeF * pEdgeLeft  = edge1;
  CEdgeF * pEdgeRight = edge2;
  GLint my(vlo->sy + ((vhi->sy - vlo->sy)/2));
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
        hline(*pEdgeLeft, *pEdgeRight, y, poly.v[2]->c2);
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
