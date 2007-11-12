#include "glesContextNDS.h"
#include "asm/arch/registers.h"
#include "asm/arch/macros.h"
typedef unsigned int wint_t;
#include <math.h>


#define fpRGB(r,g,b) (0x8000 | \
                      (((b*255) >>  9) & 0x7c00) | \
                      (((g*255) >> 14) & 0x03e0) | \
                      (((r*255) >> 19) & 0x001f))

#define fpFogRGBA(r,g,b,a) (0x8000 | \
                      (((a*255) >>  3) & 0x001f0000) | \
                      (((b*255) >>  9) & 0x00007c00) | \
                      (((g*255) >> 14) & 0x000003e0) | \
                      (((r*255) >> 19) & 0x0000001f))


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CNDSGLESContext::CNDSGLESContext()
 : CAGLESFloatToFxContext()
 , CAGLESBuffers()
 , CAGLESMatrixNDSFx()
 , CAGLESTexturesNDS()

 , texturesEnabled_(false)
{
  // Power control
  REG_POWCNT |= POWER_LCD |POWER_2D_TOP |POWER_2D_BOTTOM | POWER_3D_CORE | POWER_3D_MATRIX;
  // Display control
  REG_DISPCNT = MODE_0 | BG0_ENABLE | ENABLE_3D;

  iNDSGFXControl_ = NDS_SHADING_HIGHLIGHT;
  iNDSPolyFormat_ = NDS_POLY_ALPHA(31) | NDS_CULL_BACK | NDS_PM_MODULATION | NDS_POLY_FOG;

  GFX_CONTROL     = iNDSGFXControl_;
  GFX_POLY_FORMAT = iNDSPolyFormat_;
  GFX_CLEAR_DEPTH = 0x7fff;

  // Set the fog density table
  for(int i(0); i < 32; i++)
    GFX_FOG_TABLE[i] = (i << 2);
}

//-----------------------------------------------------------------------------
CNDSGLESContext::~CNDSGLESContext()
{
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
  zClearValue_ = gl_fptoi(depthClear_ * 0x7fff);
  GFX_CLEAR_DEPTH = zClearValue_;
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glColor4ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha)
{
  clCurrent.r = gl_fpfromi(red  ) / 255;
  clCurrent.g = gl_fpfromi(green) / 255;
  clCurrent.b = gl_fpfromi(blue ) / 255;
  clCurrent.a = gl_fpfromi(alpha) / 255;
  GFX_COLOR = fpRGB(clCurrent.r, clCurrent.g, clCurrent.b);
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glColor4x(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha)
{
  clCurrent.r = red;
  clCurrent.g = green;
  clCurrent.b = blue;
  clCurrent.a = alpha;
  GFX_COLOR = fpRGB(clCurrent.r, clCurrent.g, clCurrent.b);
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glNormal3x(GLfixed nx, GLfixed ny, GLfixed nz)
{
  normal_[0] = nx;
  normal_[1] = ny;
  normal_[2] = nz;
  normal_[3] = gl_fpfromi(1);
  //GFX_NORMAL = ...;

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

  if(cullFaceEnabled_ == true)
  {
    switch(mode)
    {
      case GL_FRONT:          iNDSPolyFormat_ = (iNDSPolyFormat_ & ~NDS_CULL_MASK) | NDS_CULL_FRONT;          break;
      case GL_BACK:           iNDSPolyFormat_ = (iNDSPolyFormat_ & ~NDS_CULL_MASK) | NDS_CULL_BACK;           break;
      case GL_FRONT_AND_BACK: iNDSPolyFormat_ = (iNDSPolyFormat_ & ~NDS_CULL_MASK) | NDS_CULL_BACK_AND_FRONT; break;
    };
    GFX_POLY_FORMAT = iNDSPolyFormat_;
  }
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
    case GL_LIGHTING:   lightingEnabled_   = false; updateLights(); break;
    case GL_LIGHT0:     lights_[0].enabled = false; updateLights(); break;
    case GL_LIGHT1:     lights_[1].enabled = false; updateLights(); break;
    case GL_LIGHT2:     lights_[2].enabled = false; updateLights(); break;
    case GL_LIGHT3:     lights_[3].enabled = false; updateLights(); break;

    case GL_DEPTH_TEST: depthTestEnabled_  = false; break;
    case GL_CULL_FACE:  cullFaceEnabled_   = false; iNDSPolyFormat_ &= ~NDS_CULL_MASK;  GFX_POLY_FORMAT = iNDSPolyFormat_; break;
    case GL_FOG:        fogEnabled_        = false; iNDSGFXControl_ &= ~NDS_FOG;        GFX_CONTROL     = iNDSGFXControl_; break;
    case GL_TEXTURE_2D: texturesEnabled_   = false; iNDSGFXControl_ &= ~NDS_TEXTURE_2D; GFX_CONTROL     = iNDSGFXControl_; break;

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

  GLint idxVertex  (first * bufVertex_.size);
  GLint idxColor   (first * bufColor_.size);
  GLint idxNormal  (first * bufNormal_.size);
  GLint idxTexCoord(first * bufTexCoord_.size);

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
        v.v[0] = gl_fpfromf(((GLfloat *)bufVertex_.pointer)[idxVertex++]);
        v.v[1] = gl_fpfromf(((GLfloat *)bufVertex_.pointer)[idxVertex++]);
        v.v[2] = gl_fpfromf(((GLfloat *)bufVertex_.pointer)[idxVertex++]);
        v.v[3] = gl_fpfromi(1);
        break;
      case GL_FIXED:
        v.v[0] = ((GLfixed *)bufVertex_.pointer)[idxVertex++];
        v.v[1] = ((GLfixed *)bufVertex_.pointer)[idxVertex++];
        v.v[2] = ((GLfixed *)bufVertex_.pointer)[idxVertex++];
        v.v[3] = gl_fpfromi(1);
        break;
    };

    // Normal
    if(bBufNormalEnabled_ == true)
    {
      switch(bufColor_.type)
      {
        case GL_FLOAT:
          v.n[0] = gl_fpfromf(((GLfloat *)bufNormal_.pointer)[idxNormal++]);
          v.n[1] = gl_fpfromf(((GLfloat *)bufNormal_.pointer)[idxNormal++]);
          v.n[2] = gl_fpfromf(((GLfloat *)bufNormal_.pointer)[idxNormal++]);
          v.n[3] = gl_fpfromi(1);
          break;
        case GL_FIXED:
          v.n[0] = ((GLfixed *)bufNormal_.pointer)[idxNormal++];
          v.n[1] = ((GLfixed *)bufNormal_.pointer)[idxNormal++];
          v.n[2] = ((GLfixed *)bufNormal_.pointer)[idxNormal++];
          v.n[3] = gl_fpfromi(1);
          break;
      };
    }

    // Textures/Colors
    if(texturesEnabled_ == true)
    {
      GFX_COLOR = 0xffff;
      // Textures
      if(bBufTexCoordEnabled_ == true)
      {
        switch(bufTexCoord_.type)
        {
          case GL_FLOAT:
            v.ts =                  gl_fpfromf(((GLfloat *)bufTexCoord_.pointer)[idxTexCoord++])  * pCurrentTex_->width;
            v.tt = (gl_fpfromi(1) - gl_fpfromf(((GLfloat *)bufTexCoord_.pointer)[idxTexCoord++])) * pCurrentTex_->height;
            break;
          case GL_FIXED:
            v.ts =                  ((GLfixed *)bufTexCoord_.pointer)[idxTexCoord++]  * pCurrentTex_->width;
            v.tt = (gl_fpfromi(1) - ((GLfixed *)bufTexCoord_.pointer)[idxTexCoord++]) * pCurrentTex_->height;
            break;
        };
      }
    }
    else
    {
      // Color
      if(bBufColorEnabled_ == true)
      {
        switch(bufColor_.type)
        {
          case GL_FLOAT:
            v.c.r = gl_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);
            v.c.g = gl_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);
            v.c.b = gl_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);
            v.c.a = gl_fpfromf(((GLfloat *)bufColor_.pointer)[idxColor++]);
            break;
          case GL_FIXED:
            v.c.r = ((GLfixed *)bufColor_.pointer)[idxColor++];
            v.c.g = ((GLfixed *)bufColor_.pointer)[idxColor++];
            v.c.b = ((GLfixed *)bufColor_.pointer)[idxColor++];
            v.c.a = ((GLfixed *)bufColor_.pointer)[idxColor++];
            break;
        };
      }
    }

    switch(mode)
    {
      case GL_TRIANGLES:
      case GL_TRIANGLE_STRIP:
        if(texturesEnabled_ == true)
          GFX_TEX_COORD = ((gl_to_ndst(v.ts) << 16) & 0xffff0000) | (gl_to_ndst(v.tt) & 0xffff);
        else
          GFX_COLOR = fpRGB(v.c.r, v.c.g, v.c.b);
        GFX_VERTEX16 = ((gl_to_ndsv(v.v[1]) << 16) & 0xffff0000) | (gl_to_ndsv(v.v[0]) & 0xffff);
        GFX_VERTEX16 = gl_to_ndsv(v.v[2]) & 0xffff;
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
    case GL_LIGHTING:   lightingEnabled_   = true; updateLights(); break;
    case GL_LIGHT0:     lights_[0].enabled = true; updateLights(); break;
    case GL_LIGHT1:     lights_[1].enabled = true; updateLights(); break;
    case GL_LIGHT2:     lights_[2].enabled = true; updateLights(); break;
    case GL_LIGHT3:     lights_[3].enabled = true; updateLights(); break;

    case GL_DEPTH_TEST: depthTestEnabled_  = true; break;
    case GL_CULL_FACE:  cullFaceEnabled_   = true; glCullFace(cullFaceMode_);                                            break;
    case GL_FOG:        fogEnabled_        = true; iNDSGFXControl_ |= NDS_FOG;        GFX_CONTROL     = iNDSGFXControl_; break;
    case GL_TEXTURE_2D: texturesEnabled_   = true; iNDSGFXControl_ |= NDS_TEXTURE_2D; GFX_CONTROL     = iNDSGFXControl_; break;

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
    case GL_FOG_DENSITY: fogDensity_ = param; updateFog(); break;
    case GL_FOG_START:   fogStart_   = param; updateFog(); break;
    case GL_FOG_END:     fogEnd_     = param; updateFog(); break;
    case GL_FOG_MODE:                                      break;
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
      GFX_FOG_COLOR = fpFogRGBA(fogColor_.r, fogColor_.g, fogColor_.b, fogColor_.a);
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
  polygon.v[iVCount_]->v[0] = v.v[0];
  polygon.v[iVCount_]->v[1] = v.v[1];
  polygon.v[iVCount_]->v[2] = v.v[2];
  polygon.v[iVCount_]->v[3] = v.v[3];
  polygon.v[iVCount_]->n[0] = v.n[0];
  polygon.v[iVCount_]->n[1] = v.n[1];
  polygon.v[iVCount_]->n[2] = v.n[2];
  polygon.v[iVCount_]->n[3] = v.n[3];
  polygon.v[iVCount_]->c    = v.c;

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
    if(texturesEnabled_ == true)
      GFX_TEX_COORD = ((gl_to_ndst(poly.v[i]->ts) << 16) & 0xffff0000) | (gl_to_ndst(poly.v[i]->tt) & 0xffff);
    else
      GFX_COLOR = fpRGB(poly.v[i]->c.r, poly.v[i]->c.g, poly.v[i]->c.b);
    GFX_VERTEX16 = ((gl_to_ndsv(poly.v[i]->v[1]) << 16) & 0xffff0000) | (gl_to_ndsv(poly.v[i]->v[0]) & 0xffff);
    GFX_VERTEX16 = gl_to_ndsv(poly.v[i]->v[2]) & 0xffff;
  }
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::updateLights()
{
  iNDSPolyFormat_ &= ~(NDS_LIGHT0 | NDS_LIGHT1 | NDS_LIGHT2 | NDS_LIGHT3);
  if(lightingEnabled_ = true)
  {
    if(lights_[0].enabled == true) iNDSPolyFormat_ |= NDS_LIGHT0;
    if(lights_[1].enabled == true) iNDSPolyFormat_ |= NDS_LIGHT1;
    if(lights_[2].enabled == true) iNDSPolyFormat_ |= NDS_LIGHT2;
    if(lights_[3].enabled == true) iNDSPolyFormat_ |= NDS_LIGHT3;
  }
  GFX_POLY_FORMAT = iNDSPolyFormat_;
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::updateFog()
{
  GFX_FOG_OFFSET = gl_to_ndsz(fogStart_);

//  fogDensity_
//  fogEnd_
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
IGLESContext *
getGLESContext()
{
  return new CNDSGLESContext;
}
