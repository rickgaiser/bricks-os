#include "softGLFx.h"
#include "matrix.h"
#include "fixedPoint.h"

#include "stdlib.h"
typedef unsigned int wint_t;
#include <math.h>


#define SCREENX(v) (gl_fptoi(gl_fpdiv(gl_fpmul(v[0], fpFieldofviewXScalar), -v[2])) + (viewportWidth  >> 1))
#define SCREENY(v) (gl_fptoi(gl_fpdiv(gl_fpmul(v[1], fpFieldofviewYScalar), -v[2])) + (viewportHeight >> 1))
#define fpRGB(r,g,b) (0x8000 | \
                      (((b*255) >>  9) & 0x7c00) | \
                      (((g*255) >> 14) & 0x03e0) | \
                      (((r*255) >> 19) & 0x001f))


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CSoftGLESFixed::CSoftGLESFixed()
 : renderSurface(0)
 , zbuffer(0)
 , shadingModel_(GL_FLAT)
 , cullFaceEnabled_(false)
 , bCullBack_(true)
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
 , depthClear_(gl_fpfromi(1))
 , edge1(0)
 , edge2(0)
 , viewportXOffset(0)
 , viewportYOffset(0)
 , viewportPixelCount(0)
 , viewportByteCount(0)
 , viewportWidth(0)
 , viewportHeight(0)
 , fpFieldofviewXScalar(gl_fpfromi(1))
 , fpFieldofviewYScalar(gl_fpfromi(1))
{
  clCurrent.r = gl_fpfromi(0);
  clCurrent.g = gl_fpfromi(0);
  clCurrent.b = gl_fpfromi(0);
  clCurrent.a = gl_fpfromi(1);

  clClear.r = gl_fpfromi(0);
  clClear.g = gl_fpfromi(0);
  clClear.b = gl_fpfromi(0);
  clClear.a = gl_fpfromi(1);

  for(int iLight(0); iLight < 8; iLight++)
  {
    lights_[iLight].ambient.r = gl_fpfromi(0);
    lights_[iLight].ambient.g = gl_fpfromi(0);
    lights_[iLight].ambient.b = gl_fpfromi(0);
    lights_[iLight].ambient.a = gl_fpfromi(1);

    lights_[iLight].diffuse.r = gl_fpfromi(0);
    lights_[iLight].diffuse.g = gl_fpfromi(0);
    lights_[iLight].diffuse.b = gl_fpfromi(0);
    lights_[iLight].diffuse.a = gl_fpfromi(1);

    lights_[iLight].specular.r = gl_fpfromi(0);
    lights_[iLight].specular.g = gl_fpfromi(0);
    lights_[iLight].specular.b = gl_fpfromi(0);
    lights_[iLight].specular.a = gl_fpfromi(1);

    lights_[iLight].enabled = false;
  }
}

//-----------------------------------------------------------------------------
CSoftGLESFixed::~CSoftGLESFixed()
{
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::setSurface(CSurface * surface)
{
  renderSurface = surface;
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glClear(GLbitfield mask)
{
  long iCount(viewportByteCount >> 1);

  if(mask & GL_COLOR_BUFFER_BIT)
  {
    color_t color(fpRGB(clClear.r, clClear.g, clClear.b));

    for(int i(0); i < iCount; i++)
      ((uint16_t *)renderSurface->p)[i] = color;
  }
  if(mask & GL_DEPTH_BUFFER_BIT)
  {
    //uint32_t zvalue = depthClear_ * 0xffffffff;
    uint32_t zvalue = 0xffffffff;

    for(int i(0); i < iCount; i++)
      zbuffer[i] = zvalue;
  }
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glClearColorx(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha)
{
  clClear.r = red;
  clClear.g = green;
  clClear.b = blue;
  clClear.a = alpha;
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glClearDepthx(GLclampx depth)
{
  depthClear_ = depth;
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glColor4ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha)
{
  clCurrent.r = gl_fpfromi(red  ) / 255;
  clCurrent.g = gl_fpfromi(green) / 255;
  clCurrent.b = gl_fpfromi(blue ) / 255;
  clCurrent.a = gl_fpfromi(alpha) / 255;
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glColor4x(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha)
{
  clCurrent.r = red;
  clCurrent.g = green;
  clCurrent.b = blue;
  clCurrent.a = alpha;
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glNormal3x(GLfixed nx, GLfixed ny, GLfixed nz)
{
  normal_[0] = nx;
  normal_[1] = ny;
  normal_[2] = nz;
  normal_[3] = gl_fpfromi(1);

  //if((enableCapabilities_ & GL_NORMALIZE) == GL_NORMALIZE)
  //{
  //  // FIXME: Normalize normal
  //}
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glNormalPointer(GLenum type, GLsizei stride, const GLvoid * pointer)
{
  bufNormal_.size    = 0;
  bufNormal_.type    = type;
  bufNormal_.stride  = stride;
  bufNormal_.pointer = pointer;
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
  bufColor_.size    = size;
  bufColor_.type    = type;
  bufColor_.stride  = stride;
  bufColor_.pointer = pointer;
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glCullFace(GLenum mode)
{
  cullFaceMode_ = mode;
  bCullBack_ = (cullFaceMode_ == GL_BACK);
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glDepthFunc(GLenum func)
{
  depthFunction_ = func;
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glDisable(GLenum cap)
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
CSoftGLESFixed::glDisableClientState(GLenum array)
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
CSoftGLESFixed::glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
  if(bBufVertexEnabled_ == false)
    return;

  switch(mode)
  {
    case GL_TRIANGLES:
    {
      SPolygonFx polygon;
      SVertexFx  v[3];
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
        v[0].v1[0] = gl_fpfromf(((GLfloat *)bufVertex_.pointer)[idxVertex++]);
        v[0].v1[1] = gl_fpfromf(((GLfloat *)bufVertex_.pointer)[idxVertex++]);
        v[0].v1[2] = gl_fpfromf(((GLfloat *)bufVertex_.pointer)[idxVertex++]);
        v[0].v1[3] = gl_fpfromi(1);
        v[1].v1[0] = gl_fpfromf(((GLfloat *)bufVertex_.pointer)[idxVertex++]);
        v[1].v1[1] = gl_fpfromf(((GLfloat *)bufVertex_.pointer)[idxVertex++]);
        v[1].v1[2] = gl_fpfromf(((GLfloat *)bufVertex_.pointer)[idxVertex++]);
        v[1].v1[3] = gl_fpfromi(1);
        v[2].v1[0] = gl_fpfromf(((GLfloat *)bufVertex_.pointer)[idxVertex++]);
        v[2].v1[1] = gl_fpfromf(((GLfloat *)bufVertex_.pointer)[idxVertex++]);
        v[2].v1[2] = gl_fpfromf(((GLfloat *)bufVertex_.pointer)[idxVertex++]);
        v[2].v1[3] = gl_fpfromi(1);

        // Normals
        if(bBufNormalEnabled_ == true)
        {
          // Use normals from array
          v[0].n1[0] = gl_fpfromf(((GLfloat *)bufNormal_.pointer)[idxNormal++]);
          v[0].n1[1] = gl_fpfromf(((GLfloat *)bufNormal_.pointer)[idxNormal++]);
          v[0].n1[2] = gl_fpfromf(((GLfloat *)bufNormal_.pointer)[idxNormal++]);
          v[0].n1[3] = gl_fpfromi(1);
          v[1].n1[0] = gl_fpfromf(((GLfloat *)bufNormal_.pointer)[idxNormal++]);
          v[1].n1[1] = gl_fpfromf(((GLfloat *)bufNormal_.pointer)[idxNormal++]);
          v[1].n1[2] = gl_fpfromf(((GLfloat *)bufNormal_.pointer)[idxNormal++]);
          v[1].n1[3] = gl_fpfromi(1);
          v[2].n1[0] = gl_fpfromf(((GLfloat *)bufNormal_.pointer)[idxNormal++]);
          v[2].n1[1] = gl_fpfromf(((GLfloat *)bufNormal_.pointer)[idxNormal++]);
          v[2].n1[2] = gl_fpfromf(((GLfloat *)bufNormal_.pointer)[idxNormal++]);
          v[2].n1[3] = gl_fpfromi(1);
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
          v[0].c1.r  = gl_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);
          v[0].c1.g  = gl_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);
          v[0].c1.b  = gl_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);
          v[0].c1.a  = gl_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);
          if(shadingModel_ == GL_SMOOTH)
          {
            v[1].c1.r  = gl_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);
            v[1].c1.g  = gl_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);
            v[1].c1.b  = gl_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);
            v[1].c1.a  = gl_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);
            v[2].c1.r  = gl_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);
            v[2].c1.g  = gl_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);
            v[2].c1.b  = gl_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);
            v[2].c1.a  = gl_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);
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
      SPolygonFx polygon;
      SVertexFx  v[3];
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
      v[0].v1[0] = gl_fpfromf(((GLfloat *)bufVertex_.pointer)[0]);
      v[0].v1[1] = gl_fpfromf(((GLfloat *)bufVertex_.pointer)[1]);
      v[0].v1[2] = gl_fpfromf(((GLfloat *)bufVertex_.pointer)[2]);
      v[0].v1[3] = gl_fpfromi(1);
      v[2].v1[0] = gl_fpfromf(((GLfloat *)bufVertex_.pointer)[3]);
      v[2].v1[1] = gl_fpfromf(((GLfloat *)bufVertex_.pointer)[4]);
      v[2].v1[2] = gl_fpfromf(((GLfloat *)bufVertex_.pointer)[5]);
      v[2].v1[3] = gl_fpfromi(1);

      // Normals
      if(bBufNormalEnabled_ == true)
      {
        // Use normals from array
        v[0].n1[0] = gl_fpfromf(((GLfloat *)bufNormal_.pointer)[0]);
        v[0].n1[1] = gl_fpfromf(((GLfloat *)bufNormal_.pointer)[1]);
        v[0].n1[2] = gl_fpfromf(((GLfloat *)bufNormal_.pointer)[2]);
        v[2].n1[3] = gl_fpfromi(1);
        v[2].n1[0] = gl_fpfromf(((GLfloat *)bufNormal_.pointer)[3]);
        v[2].n1[1] = gl_fpfromf(((GLfloat *)bufNormal_.pointer)[4]);
        v[2].n1[2] = gl_fpfromf(((GLfloat *)bufNormal_.pointer)[5]);
        v[2].n1[3] = gl_fpfromi(1);
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
        v[0].c1.r  = gl_fpfromf(((GLfloat *)bufColor_.pointer)[0]);
        v[0].c1.g  = gl_fpfromf(((GLfloat *)bufColor_.pointer)[1]);
        v[0].c1.b  = gl_fpfromf(((GLfloat *)bufColor_.pointer)[2]);
        v[0].c1.a  = gl_fpfromf(((GLfloat *)bufColor_.pointer)[3]);
        if(shadingModel_ == GL_SMOOTH)
        {
          v[2].c1.r  = gl_fpfromf(((GLfloat *)bufColor_.pointer)[4]);
          v[2].c1.g  = gl_fpfromf(((GLfloat *)bufColor_.pointer)[5]);
          v[2].c1.b  = gl_fpfromf(((GLfloat *)bufColor_.pointer)[6]);
          v[2].c1.a  = gl_fpfromf(((GLfloat *)bufColor_.pointer)[7]);
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
        SVertexFx * vtemp;
        vtemp = polygon.v[1];
        polygon.v[1] = polygon.v[2];
        polygon.v[2] = vtemp;

        vtemp->bProcessed = false;

        // Vertices
        vtemp->v1[0] = gl_fpfromf(((GLfloat *)bufVertex_.pointer)[idxVertex++]);
        vtemp->v1[1] = gl_fpfromf(((GLfloat *)bufVertex_.pointer)[idxVertex++]);
        vtemp->v1[2] = gl_fpfromf(((GLfloat *)bufVertex_.pointer)[idxVertex++]);
        vtemp->v1[3] = gl_fpfromf(1);

        // Normals
        if(bBufNormalEnabled_ == true)
        {
          // Normal
          vtemp->n1[0] = gl_fpfromf(((GLfloat *)bufNormal_.pointer)[idxNormal++]);
          vtemp->n1[1] = gl_fpfromf(((GLfloat *)bufNormal_.pointer)[idxNormal++]);
          vtemp->n1[2] = gl_fpfromf(((GLfloat *)bufNormal_.pointer)[idxNormal++]);
          vtemp->n1[3] = gl_fpfromi(1);
        }

        // Colors
        if(bBufColorEnabled_ == true)
        {
          vtemp->c1.r  = gl_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);
          vtemp->c1.g  = gl_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);
          vtemp->c1.b  = gl_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);
          vtemp->c1.a  = gl_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);
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
CSoftGLESFixed::glEnable(GLenum cap)
{
  switch(cap)
  {
    case GL_LIGHTING:   lightingEnabled_   = true; break;
    case GL_LIGHT0:     lights_[0].enabled = true; break;
    case GL_LIGHT1:     lights_[1].enabled = true; break;
    case GL_LIGHT2:     lights_[2].enabled = true; break;
    case GL_LIGHT3:     lights_[3].enabled = true; break;
    case GL_LIGHT4:     lights_[4].enabled = true; break;
    case GL_LIGHT5:     lights_[5].enabled = true; break;
    case GL_LIGHT6:     lights_[6].enabled = true; break;
    case GL_LIGHT7:     lights_[7].enabled = true; break;

    case GL_DEPTH_TEST: depthTestEnabled_  = true; break;
    case GL_CULL_FACE:  cullFaceEnabled_   = true; break;
    case GL_FOG:        fogEnabled_        = true; break;

    default:
      ; // Not supported
  };
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glEnableClientState(GLenum array)
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
CSoftGLESFixed::glFlush(void)
{
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glFogx(GLenum pname, GLfixed param)
{
  switch(pname)
  {
    case GL_FOG_DENSITY: fogDensity_ = param; break;
    case GL_FOG_START:   fogStart_   = param; break;
    case GL_FOG_END:     fogEnd_     = param; break;
    case GL_FOG_MODE:                         break;
  };
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glFogxv(GLenum pname, const GLfixed * params)
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
CSoftGLESFixed::glLightx(GLenum light, GLenum pname, GLfixed param)
{
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glLightxv(GLenum light, GLenum pname, const GLfixed * params)
{
  SLightFx * pLight = 0;
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

  SColorFx * pColor = 0;
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
CSoftGLESFixed::glLoadIdentity()
{
  pCurrentMatrix_->loadIdentity();

  // FIXME
  if(lightingEnabled_ == true)
    matrixRotation.loadIdentity();
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glMatrixMode(GLenum mode)
{
  matrixMode_ = mode;

  switch(mode)
  {
    case GL_MODELVIEW:  pCurrentMatrix_ = &matrixModelView;  break;
    case GL_PROJECTION: pCurrentMatrix_ = &matrixProjection; break;
  };
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glRotatex(GLfixed angle, GLfixed x, GLfixed y, GLfixed z)
{
  pCurrentMatrix_->rotate(angle, x, y, z);

  // FIXME
  if(lightingEnabled_ == true)
    matrixRotation.rotate(angle, x, y, z);
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glScalex(GLfixed x, GLfixed y, GLfixed z)
{
  pCurrentMatrix_->scale(x, y, z);
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glShadeModel(GLenum mode)
{
  shadingModel_ = mode;
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glTranslatex(GLfixed x, GLfixed y, GLfixed z)
{
  pCurrentMatrix_->translate(x, y, -z);
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
  bufVertex_.size    = size;
  bufVertex_.type    = type;
  bufVertex_.stride  = stride;
  bufVertex_.pointer = pointer;
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
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
  zbuffer            = new uint32_t[width * height];
  edge1              = new CEdgeFx(viewportHeight);
  edge2              = new CEdgeFx(viewportHeight);

  // Calculate field of view scalars
  //fpFieldofviewXScalar = gl_fpfromf(static_cast<float>(width)  / tan(80)); // 5.67 == tan(80)
  //fpFieldofviewYScalar = gl_fpfromf(static_cast<float>(height) / tan(80)); // 5.67 == tan(80)
  fpFieldofviewYScalar = gl_fpfromi(viewportHeight);
  fpFieldofviewXScalar = gl_fpfromi(viewportWidth);
  matrixPerspective.loadIdentity();
}

//-----------------------------------------------------------------------------
bool
CSoftGLESFixed::testAndSetDepth(GLfixed z, uint32_t index)
{
  bool bValid(false);

  if((z >= gl_fpfromf(0.1f)) && (z <= gl_fpfromf(100.0f)))
  {
    uint32_t zval = z;

    switch(depthFunction_)
    {
      case GL_LESS:     bValid = (zval <  zbuffer[index]); break;
      case GL_EQUAL:    bValid = (zval == zbuffer[index]); break;
      case GL_LEQUAL:   bValid = (zval <= zbuffer[index]); break;
      case GL_GREATER:  bValid = (zval >  zbuffer[index]); break;
      case GL_NOTEQUAL: bValid = (zval != zbuffer[index]); break;
      case GL_GEQUAL:   bValid = (zval >= zbuffer[index]); break;
      case GL_ALWAYS:   bValid = true;                     break;
      case GL_NEVER:
      default:          bValid = false;
    };

    if(bValid == true)
      zbuffer[index] = zval;
  }

  return bValid;
}

//-----------------------------------------------------------------------------
// Horizontal Line Fill, flat colors
void
CSoftGLESFixed::hline(CEdgeFx & from, CEdgeFx & to, GLint & y, SColorFx c)
{
  if(from.x_[y] < to.x_[y])
  {
    GLint dx(to.x_[y] - from.x_[y]);
    GLfixed mz((to.z_[y] - from.z_[y]) / dx);
    GLfixed z(from.z_[y]);
    color_t color(fpRGB(c.r, c.g, c.b));

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
          if(testAndSetDepth(z, index) == false)
          {
            z += mz;
            continue;
          }
          z += mz;
        }
        ((uint16_t *)renderSurface->p)[index] = color;
      }
      index++;
    }
  }
}

//-----------------------------------------------------------------------------
// Horizontal Line Fill, smooth colors
void
CSoftGLESFixed::hline_s(CEdgeFx & from, CEdgeFx & to, GLint & y)
{
  if(from.x_[y] < to.x_[y])
  {
    GLint dx(to.x_[y] - from.x_[y]);
    GLfixed mz((to.z_[y] - from.z_[y]) / dx);
    GLfixed z(from.z_[y]);

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
        // Depth test pixel
        if(depthTestEnabled_ == true)
        {
          if(testAndSetDepth(z, index) == false)
          {
            z += mz;
            continue;
          }
          z += mz;
        }
        ((uint16_t *)renderSurface->p)[index] = fpRGB(r, g, b);
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
CSoftGLESFixed::plotPoly(SPolygonFx & poly)
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
    // Always invisible when culling both front and back
    if(cullFaceMode_ == GL_FRONT_AND_BACK)
      return;

    // Figure out if we need to cull
    if((poly.v[1]->sx != poly.v[0]->sx) && (poly.v[2]->sx != poly.v[0]->sx))
    {
      if(((((gl_fpfromi(poly.v[1]->sy - poly.v[0]->sy) / (poly.v[1]->sx - poly.v[0]->sx)) - (gl_fpfromi(poly.v[2]->sy - poly.v[0]->sy) / (poly.v[2]->sx - poly.v[0]->sx))) < 0) ^ ((poly.v[0]->sx <= poly.v[1]->sx) == (poly.v[0]->sx > poly.v[2]->sx))) == bCullBack_)
        return;
    }
    else if((poly.v[2]->sx != poly.v[1]->sx) && (poly.v[0]->sx != poly.v[1]->sx))
    {
      if(((((gl_fpfromi(poly.v[2]->sy - poly.v[1]->sy) / (poly.v[2]->sx - poly.v[1]->sx)) - (gl_fpfromi(poly.v[0]->sy - poly.v[1]->sy) / (poly.v[0]->sx - poly.v[1]->sx))) < 0) ^ ((poly.v[1]->sx <= poly.v[2]->sx) == (poly.v[1]->sx > poly.v[0]->sx))) == bCullBack_)
        return;
    }
    else if((poly.v[0]->sx != poly.v[2]->sx) && (poly.v[1]->sx != poly.v[2]->sx))
    {
      if(((((gl_fpfromi(poly.v[0]->sy - poly.v[2]->sy) / (poly.v[0]->sx - poly.v[2]->sx)) - (gl_fpfromi(poly.v[1]->sy - poly.v[2]->sy) / (poly.v[1]->sx - poly.v[2]->sx))) < 0) ^ ((poly.v[2]->sx <= poly.v[0]->sx) == (poly.v[2]->sx > poly.v[1]->sx))) == bCullBack_)
        return;
    }
    else
      return; // Triangle invisible
  }

  // Lighting
  if(lightingEnabled_ == true)
  {
    // Normal Rotation
    matrixRotation.transform(poly.v[0]->n1, poly.v[0]->n2);
    matrixRotation.transform(poly.v[1]->n1, poly.v[1]->n2);
    matrixRotation.transform(poly.v[2]->n1, poly.v[2]->n2);
    // FIXME: Light value of normal
    GLfixed normal[3] = {abs(poly.v[0]->n2[2]), abs(poly.v[1]->n2[2]), abs(poly.v[2]->n2[2])};

    for(int iLight(0); iLight < 8; iLight++)
    {
      if(lights_[iLight].enabled == true)
      {
        SColorFx & ambient = lights_[iLight].ambient;
        SColorFx & diffuse = lights_[iLight].diffuse;

        poly.v[0]->c2.r = gl_fpclamp(gl_fpmul(poly.v[0]->c1.r, ambient.r) + gl_fpmul(gl_fpmul(poly.v[0]->c1.r, normal[0]), diffuse.r));
        poly.v[0]->c2.g = gl_fpclamp(gl_fpmul(poly.v[0]->c1.g, ambient.g) + gl_fpmul(gl_fpmul(poly.v[0]->c1.g, normal[0]), diffuse.g));
        poly.v[0]->c2.b = gl_fpclamp(gl_fpmul(poly.v[0]->c1.b, ambient.b) + gl_fpmul(gl_fpmul(poly.v[0]->c1.b, normal[0]), diffuse.b));

        if(shadingModel_ == GL_SMOOTH)
        {
          poly.v[1]->c2.r = gl_fpclamp(gl_fpmul(poly.v[1]->c1.r, ambient.r) + gl_fpmul(gl_fpmul(poly.v[1]->c1.r, normal[1]), diffuse.r));
          poly.v[1]->c2.g = gl_fpclamp(gl_fpmul(poly.v[1]->c1.g, ambient.g) + gl_fpmul(gl_fpmul(poly.v[1]->c1.g, normal[1]), diffuse.g));
          poly.v[1]->c2.b = gl_fpclamp(gl_fpmul(poly.v[1]->c1.b, ambient.b) + gl_fpmul(gl_fpmul(poly.v[1]->c1.b, normal[1]), diffuse.b));

          poly.v[2]->c2.r = gl_fpclamp(gl_fpmul(poly.v[2]->c1.r, ambient.r) + gl_fpmul(gl_fpmul(poly.v[2]->c1.r, normal[2]), diffuse.r));
          poly.v[2]->c2.g = gl_fpclamp(gl_fpmul(poly.v[2]->c1.g, ambient.g) + gl_fpmul(gl_fpmul(poly.v[2]->c1.g, normal[2]), diffuse.g));
          poly.v[2]->c2.b = gl_fpclamp(gl_fpmul(poly.v[2]->c1.b, ambient.b) + gl_fpmul(gl_fpmul(poly.v[2]->c1.b, normal[2]), diffuse.b));
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
      GLfixed partFog   = gl_fpclamp(gl_fpdiv(abs(poly.v[i]->v2[2]) - fogStart_, fogEnd_ - fogStart_));
      GLfixed partColor = gl_fpfromi(1) - partFog;
      poly.v[i]->c2.r = gl_fpclamp(gl_fpmul(poly.v[i]->c2.r, partColor) + gl_fpmul(fogColor_.r, partFog));
      poly.v[i]->c2.g = gl_fpclamp(gl_fpmul(poly.v[i]->c2.g, partColor) + gl_fpmul(fogColor_.g, partFog));
      poly.v[i]->c2.b = gl_fpclamp(gl_fpmul(poly.v[i]->c2.b, partColor) + gl_fpmul(fogColor_.b, partFog));
    }
  }

  rasterPoly(poly);
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::rasterPoly(SPolygonFx & poly)
{
  // Bubble sort the 3 vertexes
  SVertexFx * vtemp;
  SVertexFx * vhi(poly.v[0]);
  SVertexFx * vmi(poly.v[1]);
  SVertexFx * vlo(poly.v[2]);

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
  if(depthTestEnabled_ == true)
  {
    if(shadingModel_ == GL_SMOOTH)
    {
      edge1->addZC(vlo->sx, vlo->sy, vlo->v2[2], vlo->c2, vhi->sx, vhi->sy, vhi->v2[2], vhi->c2);
      edge2->addZC(vlo->sx, vlo->sy, vlo->v2[2], vlo->c2, vmi->sx, vmi->sy, vmi->v2[2], vmi->c2);
      edge2->addZC(vmi->sx, vmi->sy, vmi->v2[2], vmi->c2, vhi->sx, vhi->sy, vhi->v2[2], vhi->c2);
    }
    else
    {
      edge1->addZ(vlo->sx, vlo->sy, vlo->v2[2], vhi->sx, vhi->sy, vhi->v2[2]);
      edge2->addZ(vlo->sx, vlo->sy, vlo->v2[2], vmi->sx, vmi->sy, vmi->v2[2]);
      edge2->addZ(vmi->sx, vmi->sy, vmi->v2[2], vhi->sx, vhi->sy, vhi->v2[2]);
    }
  }
  else
  {
    if(shadingModel_ == GL_SMOOTH)
    {
      edge1->addC(vlo->sx, vlo->sy, vlo->c2, vhi->sx, vhi->sy, vhi->c2);
      edge2->addC(vlo->sx, vlo->sy, vlo->c2, vmi->sx, vmi->sy, vmi->c2);
      edge2->addC(vmi->sx, vmi->sy, vmi->c2, vhi->sx, vhi->sy, vhi->c2);
    }
    else
    {
      edge1->add(vlo->sx, vlo->sy, vhi->sx, vhi->sy);
      edge2->add(vlo->sx, vlo->sy, vmi->sx, vmi->sy);
      edge2->add(vmi->sx, vmi->sy, vhi->sx, vhi->sy);
    }
  }

  CEdgeFx * pEdgeLeft  = edge1;
  CEdgeFx * pEdgeRight = edge2;
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
