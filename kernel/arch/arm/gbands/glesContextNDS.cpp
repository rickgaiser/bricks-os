#include "glesContextNDS.h"
#include "asm/arch/registers.h"
#include "asm/arch/macros.h"
typedef unsigned int wint_t;
#include <math.h>


#define fpRGB(r,g,b) (0x8000 | \
                      (((b*255) >>  9) & 0x7c00) | \
                      (((g*255) >> 14) & 0x03e0) | \
                      (((r*255) >> 19) & 0x001f))


//-----------------------------------------------------------------------------
// glPolyFmt constants
//-----------------------------------------------------------------------------
#define POLY_ALPHA(n)           ((n) << 16)
#define POLY_TOON_SHADING       0x20
#define POLY_CULL_BACK          0x80
#define POLY_CULL_FRONT         0x40
#define POLY_CULL_NONE          0xC0
#define POLY_ID(n)              ((n)<<24)

#define POLY_FORMAT_LIGHT0      0x1
#define POLY_FORMAT_LIGHT1      0x2
#define POLY_FORMAT_LIGHT2      0x4
#define POLY_FORMAT_LIGHT3      0x8

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CNDSGLESContext::CNDSGLESContext()
 : CAGLESFloatToFxContext()
 , CAGLESBuffers()
 , CAGLESMatrixNDSFx()
{
  // Power control
  REG_POWCNT |= POWER_LCD |POWER_2D_TOP |POWER_2D_BOTTOM | POWER_3D_CORE | POWER_3D_MATRIX;
  // Display control
  REG_DISPCNT = MODE_0 | BG0_ENABLE | ENABLE_3D;

  GFX_POLY_FORMAT = POLY_ALPHA(31) | POLY_CULL_NONE;

  GFX_CLEAR_DEPTH = 0x7fff;
}

//-----------------------------------------------------------------------------
CNDSGLESContext::~CNDSGLESContext()
{
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::setSurface(CSurface * surface)
{
  renderSurface = surface;
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glClear(GLbitfield mask)
{
  if(mask & GL_COLOR_BUFFER_BIT)
  {
  }

  if(mask & GL_DEPTH_BUFFER_BIT)
  {
  }
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glClearColorx(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha)
{
  BG_PALETTE[0] = fpRGB(red, green, blue);
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glClearDepthx(GLclampx depth)
{
  depthClear_ = clampfx(depth);
  zClearValue_ = gl_fptoi(depthClear_ * 0xffff);
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glColor4ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha)
{
  clCurrent.r = gl_fpfromi(red  ) / 255;
  clCurrent.g = gl_fpfromi(green) / 255;
  clCurrent.b = gl_fpfromi(blue ) / 255;
  clCurrent.a = gl_fpfromi(alpha) / 255;
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glColor4x(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha)
{
  clCurrent.r = red;
  clCurrent.g = green;
  clCurrent.b = blue;
  clCurrent.a = alpha;
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glNormal3x(GLfixed nx, GLfixed ny, GLfixed nz)
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
CNDSGLESContext::glCullFace(GLenum mode)
{
  cullFaceMode_ = mode;
  bCullBack_ = (cullFaceMode_ == GL_BACK);
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glDepthFunc(GLenum func)
{
  depthFunction_ = func;
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glDisable(GLenum cap)
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
CNDSGLESContext::glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
  if(bBufVertexEnabled_ == false)
    return;

  GLint idxVertex(first * bufVertex_.size);
  GLint idxColor (first * bufColor_.size);
  GLint idxNormal(first * bufNormal_.size);

  SVertexFx v;

  switch(mode)
  {
    case GL_TRIANGLES:
      GFX_BEGIN = NDS_TRIANGLES;
      break;
    case GL_TRIANGLE_STRIP:
      GFX_BEGIN = NDS_TRIANGLE_STRIP;
      break;
    // Simulate triangle fan using triangles
    case GL_TRIANGLE_FAN:
      GFX_BEGIN = NDS_TRIANGLES;
      break;
  };

  // Process all vertices
  for(GLint i(0); i < count; i++)
  {
    // Vertex
    switch(bufVertex_.type)
    {
      case GL_FLOAT:
        v.v1[0] = gl_fpfromf(((GLfloat *)bufVertex_.pointer)[idxVertex++]);
        v.v1[1] = gl_fpfromf(((GLfloat *)bufVertex_.pointer)[idxVertex++]);
        v.v1[2] = gl_fpfromf(((GLfloat *)bufVertex_.pointer)[idxVertex++]);
        v.v1[3] = gl_fpfromi(1);
        break;
      case GL_FIXED:
        v.v1[0] = ((GLfixed *)bufVertex_.pointer)[idxVertex++];
        v.v1[1] = ((GLfixed *)bufVertex_.pointer)[idxVertex++];
        v.v1[2] = ((GLfixed *)bufVertex_.pointer)[idxVertex++];
        v.v1[3] = gl_fpfromi(1);
        break;
    };

    // Normal
    if(bBufNormalEnabled_ == true)
    {
      switch(bufColor_.type)
      {
        case GL_FLOAT:
          v.n1[0] = gl_fpfromf(((GLfloat *)bufNormal_.pointer)[idxNormal++]);
          v.n1[1] = gl_fpfromf(((GLfloat *)bufNormal_.pointer)[idxNormal++]);
          v.n1[2] = gl_fpfromf(((GLfloat *)bufNormal_.pointer)[idxNormal++]);
          v.n1[3] = gl_fpfromi(1);
          break;
        case GL_FIXED:
          v.n1[0] = ((GLfixed *)bufNormal_.pointer)[idxNormal++];
          v.n1[1] = ((GLfixed *)bufNormal_.pointer)[idxNormal++];
          v.n1[2] = ((GLfixed *)bufNormal_.pointer)[idxNormal++];
          v.n1[3] = gl_fpfromi(1);
          break;
      };
    }

    // Color
    if(bBufColorEnabled_ == true)
    {
      switch(bufColor_.type)
      {
        case GL_FLOAT:
          v.c1.r = gl_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);
          v.c1.g = gl_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);
          v.c1.b = gl_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);
          v.c1.a = gl_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);
          break;
        case GL_FIXED:
          v.c1.r = ((GLfixed *)bufColor_.pointer)[idxColor++];
          v.c1.g = ((GLfixed *)bufColor_.pointer)[idxColor++];
          v.c1.b = ((GLfixed *)bufColor_.pointer)[idxColor++];
          v.c1.a = ((GLfixed *)bufColor_.pointer)[idxColor++];
          break;
      };
    }

    switch(mode)
    {
      case GL_TRIANGLES:
      case GL_TRIANGLE_STRIP:
        GFX_COLOR = fpRGB(v.c1.r, v.c1.g, v.c1.b);
        GFX_VERTEX16 = ((gl_to_nds(v.v1[1]) << 16) & 0xffff0000) | (gl_to_nds(v.v1[0]) & 0xffff);
        GFX_VERTEX16 = gl_to_nds(v.v1[2]) & 0xffff;
        break;
      case GL_TRIANGLE_FAN:
        addVertexToTriangleFan(v);
        break;
    };
  }
  GFX_END = 0;
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glEnable(GLenum cap)
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
CNDSGLESContext::glFinish(void)
{
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glFlush(void)
{
  GFX_FLUSH = 2;
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glFogx(GLenum pname, GLfixed param)
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
CNDSGLESContext::glFogxv(GLenum pname, const GLfixed * params)
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
CNDSGLESContext::glLightx(GLenum light, GLenum pname, GLfixed param)
{
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glLightxv(GLenum light, GLenum pname, const GLfixed * params)
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
CNDSGLESContext::glShadeModel(GLenum mode)
{
  shadingModel_ = mode;
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
  GFX_VIEWPORT = x + (y << 8) + ((width-1) << 16) + ((height-1) << 24);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void
CNDSGLESContext::addVertexToTriangleFan(SVertexFx & v)
{
  static SPolygonFx polygon;
  static SVertexFx vertices[3];
  static bool bInitialized(false);
  if(bInitialized == false)
  {
    polygon.v[0] = &vertices[0];
    polygon.v[1] = &vertices[1];
    polygon.v[2] = &vertices[2];
    bInitialized = true;
  }

  polygon.v[iVCount_]->bProcessed = false;
  // Copy vertex
  polygon.v[iVCount_]->v1[0] = v.v1[0];
  polygon.v[iVCount_]->v1[1] = v.v1[1];
  polygon.v[iVCount_]->v1[2] = v.v1[2];
  polygon.v[iVCount_]->v1[3] = v.v1[3];
  polygon.v[iVCount_]->n1[0] = v.n1[0];
  polygon.v[iVCount_]->n1[1] = v.n1[1];
  polygon.v[iVCount_]->n1[2] = v.n1[2];
  polygon.v[iVCount_]->n1[3] = v.n1[3];
  polygon.v[iVCount_]->c1    = v.c1;

  if(iVCount_ == 2)
  {
    plotPoly(polygon);

    // Swap 3rd and 2nd vertex
    if(polygon.v[1] == &vertices[1])
    {
      polygon.v[1] = &vertices[2];
      polygon.v[2] = &vertices[1];
    }
    else
    {
      polygon.v[1] = &vertices[1];
      polygon.v[2] = &vertices[2];
    }
  }
  else
    iVCount_++;
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::plotPoly(SPolygonFx & poly)
{
  for(int i(0); i < 3; i++)
  {
    GFX_COLOR = fpRGB(poly.v[i]->c1.r, poly.v[i]->c1.g, poly.v[i]->c1.b);
    GFX_VERTEX16 = ((gl_to_nds(poly.v[i]->v1[1]) << 16) & 0xffff0000) | (gl_to_nds(poly.v[i]->v1[0]) & 0xffff);
    GFX_VERTEX16 = gl_to_nds(poly.v[i]->v1[2]) & 0xffff;
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
IGLESContext *
getGLESContext()
{
  return new CNDSGLESContext;
}
