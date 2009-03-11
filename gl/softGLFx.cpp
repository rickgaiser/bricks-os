/*
 * Bricks-OS, Operating System for Game Consoles
 * Copyright (C) 2008 Maximus32 <Maximus32@bricks-os.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA
 */


#include "softGLFx.h"
#include "vhl/fixedPoint.h"
#include "vhl/matrix.h"

#include "stdlib.h"
#include "math.h"


//-----------------------------------------------------------------------------
// Model-View matrix
//   from 'object coordinates' to 'eye coordinates'
#define CALC_OBJ_TO_EYE(v) \
  pCurrentModelView_->transform4((v).vo, (v).ve)
//-----------------------------------------------------------------------------
// Projection matrix
//   from 'eye coordinates' to 'clip coordinates'
#define CALC_EYE_TO_CLIP(v) \
  pCurrentProjection_->transform4((v).ve, (v).vc)
//-----------------------------------------------------------------------------
// Perspective division
//   from 'clip coordinates' to 'normalized device coordinates'
#define CALC_CLIP_TO_NDEV(v) \
  CFixed inv_w; \
  inv_w.value = gl_fpinverse((v).vc.w.value); \
  (v).vd.x = (v).vc.x * inv_w; \
  (v).vd.y = (v).vc.y * inv_w; \
  (v).vd.z = (v).vc.z * inv_w; \
  (v).vd.w = inv_w

//-----------------------------------------------------------------------------
// Viewport transformation
//   from 'normalized device coordinates' to 'window coordinates'
#define CALC_NDEV_TO_WINDOW(v) \
  (v).sx = (GLint)   ((xA_ * (v).vd.x) + xB_); \
  (v).sy = (GLint)   ((yA_ * (v).vd.y) + yB_)

#define CLIP_ROUNDING_ERROR (CFixed(0.00001f)) // (1E-5)
inline void setClipFlags(SVertexFx & v)
{
  CFixed w = v.vc.w * (CFixed(1) + CLIP_ROUNDING_ERROR);
  v.clip = ((v.vc.x.value < -w.value)     ) |
           ((v.vc.x.value >  w.value) << 1) |
           ((v.vc.y.value < -w.value) << 2) |
           ((v.vc.y.value >  w.value) << 3) |
           ((v.vc.z.value < -w.value) << 4) |
           ((v.vc.z.value >  w.value) << 5);
}

#define CALC_INTERSECTION(a,b,t) (b + ((a - b) * t))


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CASoftGLESFixed::CASoftGLESFixed()
 : CAGLESFloatToFxContext()
 , CAGLESBase()
 , CAGLESMatrixFx()

 , depthTestEnabled_(false)
 , depthMask_(true)
 , depthFunction_(GL_LESS)
 , depthClear_(1)
 , zClearValue_(0x0000ffff)
 , zRangeNear_(0)
 , zRangeFar_(1)
 , zNear_(0)
 , zFar_(1)
 , shadingModel_(GL_FLAT)
 , bSmoothShading_(false)
 , blendingEnabled_(false)
 , blendSFactor_(GL_ONE)
 , blendDFactor_(GL_ZERO)
 , alphaTestEnabled_(false)
 , alphaFunc_(GL_ALWAYS)
 , alphaValue_(0.0f)
 , alphaValueFX_(0)
 , lightingEnabled_(false)
 , normalizeEnabled_(false)
 , fogEnabled_(false)
 , texturesEnabled_(false)
 , texEnvMode_(GL_MODULATE)
 , texEnvColor_(0, 0, 0, 0)
 , beginValid_(false)
{
  clCurrent.r = 1;
  clCurrent.g = 1;
  clCurrent.b = 1;
  clCurrent.a = 1;

  clClear.r = 0;
  clClear.g = 0;
  clClear.b = 0;
  clClear.a = 0;

  // Light properties
  for(int iLight(0); iLight < 8; iLight++)
  {
    lights_[iLight].ambient.r = 0;
    lights_[iLight].ambient.g = 0;
    lights_[iLight].ambient.b = 0;
    lights_[iLight].ambient.a = 1;

    if(iLight == 0)
    {
      lights_[iLight].diffuse.r = 1;
      lights_[iLight].diffuse.g = 1;
      lights_[iLight].diffuse.b = 1;
      lights_[iLight].diffuse.a = 1;

      lights_[iLight].specular.r = 1;
      lights_[iLight].specular.g = 1;
      lights_[iLight].specular.b = 1;
      lights_[iLight].specular.a = 1;
    }
    else
    {
      lights_[iLight].diffuse.r = 0;
      lights_[iLight].diffuse.g = 0;
      lights_[iLight].diffuse.b = 0;
      lights_[iLight].diffuse.a = 0;

      lights_[iLight].specular.r = 0;
      lights_[iLight].specular.g = 0;
      lights_[iLight].specular.b = 0;
      lights_[iLight].specular.a = 0;
    }

    lights_[iLight].position.x = 0;
    lights_[iLight].position.y = 0;
    lights_[iLight].position.z = 1;
    lights_[iLight].position.w = 0;
    lights_[iLight].direction  = lights_[iLight].position.getInverted();

    lights_[iLight].enabled = false;
  }

  // Material properties
  matColorAmbient_.r  = 0.2f;
  matColorAmbient_.g  = 0.2f;
  matColorAmbient_.b  = 0.2f;
  matColorAmbient_.a  = 1.0f;

  matColorDiffuse_.r  = 0.8f;
  matColorDiffuse_.g  = 0.8f;
  matColorDiffuse_.b  = 0.8f;
  matColorDiffuse_.a  = 1.0f;

  matColorSpecular_.r = 0.0f;
  matColorSpecular_.g = 0.0f;
  matColorSpecular_.b = 0.0f;
  matColorSpecular_.a = 1.0f;

  matColorEmission_.r = 0.0f;
  matColorEmission_.g = 0.0f;
  matColorEmission_.b = 0.0f;
  matColorEmission_.a = 1.0f;

  matShininess_       = 0.0f;

  float zsize = (1 << (DEPTH_Z + SHIFT_Z));
  zA_ = ((zsize - 0.5) / 2.0);
  zB_ = ((zsize - 0.5) / 2.0) + (1 << (SHIFT_Z-1));

  texEnvColorFX_.r = texEnvColor_.r.value >> (16 - SHIFT_COLOR);
  texEnvColorFX_.g = texEnvColor_.g.value >> (16 - SHIFT_COLOR);
  texEnvColorFX_.b = texEnvColor_.b.value >> (16 - SHIFT_COLOR);
  texEnvColorFX_.a = texEnvColor_.a.value >> (16 - SHIFT_COLOR);

  clipPlane_[0] = TVector4<CFixed>( 1.0,  0.0,  0.0,  1.0); // left
  clipPlane_[1] = TVector4<CFixed>(-1.0,  0.0,  0.0,  1.0); // right
  clipPlane_[2] = TVector4<CFixed>( 0.0,  1.0,  0.0,  1.0); // bottom
  clipPlane_[3] = TVector4<CFixed>( 0.0, -1.0,  0.0,  1.0); // top
  clipPlane_[4] = TVector4<CFixed>( 0.0,  0.0,  1.0,  1.0); // near
  clipPlane_[5] = TVector4<CFixed>( 0.0,  0.0, -1.0,  1.0); // far
}

//-----------------------------------------------------------------------------
CASoftGLESFixed::~CASoftGLESFixed()
{
}

//-----------------------------------------------------------------------------
void
CASoftGLESFixed::glAlphaFunc(GLenum func, GLclampf ref)
{
  switch(func)
  {
    case GL_NEVER:
    case GL_LESS:
    case GL_EQUAL:
    case GL_LEQUAL:
    case GL_GREATER:
    case GL_NOTEQUAL:
    case GL_GEQUAL:
    case GL_ALWAYS:
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };

  alphaFunc_    = func;
  alphaValue_   = clampf(ref);
  alphaValueFX_ = (int32_t)(alphaValue_ * (1 << SHIFT_COLOR));
}

//-----------------------------------------------------------------------------
void
CASoftGLESFixed::glClearColorx(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha)
{
  clClear.r.value = clampfx(red);
  clClear.g.value = clampfx(green);
  clClear.b.value = clampfx(blue);
  clClear.a.value = clampfx(alpha);
}

//-----------------------------------------------------------------------------
void
CASoftGLESFixed::glClearDepthx(GLclampx depth)
{
  depthClear_.value = clampfx(depth);

  zClearValue_ = (uint32_t)((float)depthClear_ * ((1<<DEPTH_Z)-1));
}

//-----------------------------------------------------------------------------
void
CASoftGLESFixed::glDepthRangex(GLclampx zNear, GLclampx zFar)
{
  zNear_.value = clampfx(zNear);
  zFar_.value  = clampfx(zFar);

  // FIXME: zA_ and zB_ need to be modified, this function is now useless
}

//-----------------------------------------------------------------------------
void
CASoftGLESFixed::glDepthFunc(GLenum func)
{
  depthFunction_ = func;
}

//-----------------------------------------------------------------------------
void
CASoftGLESFixed::glDepthMask(GLboolean flag)
{
  depthMask_ = flag;
}

//-----------------------------------------------------------------------------
void
CASoftGLESFixed::glDisable(GLenum cap)
{
  switch(cap)
  {
    case GL_ALPHA_TEST: alphaTestEnabled_  = false; break;
    case GL_BLEND:      blendingEnabled_   = false; break;
    case GL_LIGHTING:   lightingEnabled_   = false; break;
    case GL_LIGHT0:     lights_[0].enabled = false; break;
    case GL_LIGHT1:     lights_[1].enabled = false; break;
    case GL_LIGHT2:     lights_[2].enabled = false; break;
    case GL_LIGHT3:     lights_[3].enabled = false; break;
    case GL_LIGHT4:     lights_[4].enabled = false; break;
    case GL_LIGHT5:     lights_[5].enabled = false; break;
    case GL_LIGHT6:     lights_[6].enabled = false; break;
    case GL_LIGHT7:     lights_[7].enabled = false; break;

    case GL_DEPTH_TEST:
      depthTestEnabled_ = false;
      zbuffer(false); // Notify rasterizer
      break;
    case GL_CULL_FACE:  cullFaceEnabled_   = false; break;
    case GL_FOG:        fogEnabled_        = false; break;
    case GL_TEXTURE_2D: texturesEnabled_   = false; break;
    case GL_NORMALIZE:  normalizeEnabled_  = false; break;

    default:
      setError(GL_INVALID_ENUM);
      return;
  };
}

//-----------------------------------------------------------------------------
void
CASoftGLESFixed::glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
  _glDrawArrays(mode, first, count);
}

//-----------------------------------------------------------------------------
void
CASoftGLESFixed::glEnable(GLenum cap)
{
  switch(cap)
  {
    case GL_ALPHA_TEST: alphaTestEnabled_  = true; break;
    case GL_BLEND:      blendingEnabled_   = true; break;
    case GL_LIGHTING:   lightingEnabled_   = true; break;
    case GL_LIGHT0:     lights_[0].enabled = true; break;
    case GL_LIGHT1:     lights_[1].enabled = true; break;
    case GL_LIGHT2:     lights_[2].enabled = true; break;
    case GL_LIGHT3:     lights_[3].enabled = true; break;
    case GL_LIGHT4:     lights_[4].enabled = true; break;
    case GL_LIGHT5:     lights_[5].enabled = true; break;
    case GL_LIGHT6:     lights_[6].enabled = true; break;
    case GL_LIGHT7:     lights_[7].enabled = true; break;

    case GL_DEPTH_TEST:
      depthTestEnabled_ = true;
      zbuffer(true); // Notify rasterizer
      break;
    case GL_CULL_FACE:  cullFaceEnabled_   = true; break;
    case GL_FOG:        fogEnabled_        = true; break;
    case GL_TEXTURE_2D: texturesEnabled_   = true; break;
    case GL_NORMALIZE:  normalizeEnabled_  = true; break;

    default:
      setError(GL_INVALID_ENUM);
      return;
  };
}

//-----------------------------------------------------------------------------
void
CASoftGLESFixed::glFinish(void)
{
}

//-----------------------------------------------------------------------------
void
CASoftGLESFixed::glFlush(void)
{
  flush();
}

//-----------------------------------------------------------------------------
void
CASoftGLESFixed::glFogx(GLenum pname, GLfixed param)
{
  switch(pname)
  {
    case GL_FOG_DENSITY: fogDensity_ = param; break;
    case GL_FOG_START:   fogStart_   = param; break;
    case GL_FOG_END:     fogEnd_     = param; break;
    case GL_FOG_MODE:                         break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };
}

//-----------------------------------------------------------------------------
void
CASoftGLESFixed::glFogxv(GLenum pname, const GLfixed * params)
{
  switch(pname)
  {
    case GL_FOG_COLOR:
      fogColor_.r.value = params[0];
      fogColor_.g.value = params[1];
      fogColor_.b.value = params[2];
      fogColor_.a.value = params[3];
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };
}

//-----------------------------------------------------------------------------
void
CASoftGLESFixed::glLightx(GLenum light, GLenum pname, GLfixed param)
{
}

//-----------------------------------------------------------------------------
void
CASoftGLESFixed::glLightxv(GLenum light, GLenum pname, const GLfixed * params)
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
      setError(GL_INVALID_ENUM);
      return;
  }

  SColorFx * pColor = 0;
  switch(pname)
  {
    case GL_AMBIENT:  pColor = &pLight->ambient; break;
    case GL_DIFFUSE:  pColor = &pLight->diffuse; break;
    case GL_SPECULAR: pColor = &pLight->specular; break;
    case GL_POSITION:
      pLight->position.x.value = params[0];
      pLight->position.y.value = params[1];
      pLight->position.z.value = params[2];
      pLight->position.w.value = params[3];
      // Invert and normalize
      pLight->direction = pLight->position.getInverted().normalize();
      return;
    default:
      setError(GL_INVALID_ENUM);
      return;
  }

  pColor->r.value = params[0];
  pColor->g.value = params[1];
  pColor->b.value = params[2];
  pColor->a.value = params[3];
}

//-----------------------------------------------------------------------------
void
CASoftGLESFixed::glMaterialx(GLenum face, GLenum pname, GLfixed param)
{
  switch(pname)
  {
    case GL_SHININESS:
      matShininess_.value = param;
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  }
}

//-----------------------------------------------------------------------------
void
CASoftGLESFixed::glMaterialxv(GLenum face, GLenum pname, const GLfixed *params)
{
  switch(pname)
  {
    case GL_AMBIENT:
      matColorAmbient_.r.value = params[0];
      matColorAmbient_.g.value = params[1];
      matColorAmbient_.b.value = params[2];
      matColorAmbient_.a.value = params[3];
      break;
    case GL_DIFFUSE:
      matColorDiffuse_.r.value = params[0];
      matColorDiffuse_.g.value = params[1];
      matColorDiffuse_.b.value = params[2];
      matColorDiffuse_.a.value = params[3];
      break;
    case GL_SPECULAR:
      matColorSpecular_.r.value = params[0];
      matColorSpecular_.g.value = params[1];
      matColorSpecular_.b.value = params[2];
      matColorSpecular_.a.value = params[3];
      break;
    case GL_EMISSION:
      matColorEmission_.r.value = params[0];
      matColorEmission_.g.value = params[1];
      matColorEmission_.b.value = params[2];
      matColorEmission_.a.value = params[3];
      break;
    case GL_SHININESS:
      matShininess_.value = params[0];
      break;
    case GL_AMBIENT_AND_DIFFUSE:
      matColorAmbient_.r.value = params[0];
      matColorAmbient_.g.value = params[1];
      matColorAmbient_.b.value = params[2];
      matColorAmbient_.a.value = params[3];
      matColorDiffuse_.r.value = params[0];
      matColorDiffuse_.g.value = params[1];
      matColorDiffuse_.b.value = params[2];
      matColorDiffuse_.a.value = params[3];
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  }
}

//-----------------------------------------------------------------------------
void
CASoftGLESFixed::glShadeModel(GLenum mode)
{
  shadingModel_ = mode;
  bSmoothShading_ = (shadingModel_ == GL_SMOOTH);
}

//-----------------------------------------------------------------------------
void
CASoftGLESFixed::glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
  viewportXOffset    = x;
  viewportYOffset    = y;
  viewportWidth      = width;
  viewportHeight     = height;
  viewportPixelCount = width * height;

  xA_ =     (viewportWidth  >> 1);
  xB_ = x + (viewportWidth  >> 1);
  yA_ =     (viewportHeight >> 1);
  yB_ = y + (viewportHeight >> 1);

  // FIXME:
  yA_  = 0-yA_;
  xB_ -= x;
  yB_ -= y;

  // Use fixed point format for xy
//  xA_ *= (1<<SHIFT_XY);
//  xB_ *= (1<<SHIFT_XY);
//  yA_ *= (1<<SHIFT_XY);
//  yB_ *= (1<<SHIFT_XY);
}

//-----------------------------------------------------------------------------
void
CASoftGLESFixed::glBegin(GLenum mode)
{
  if(beginValid_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  switch(mode)
  {
//    case GL_POINTS:
//    case GL_LINES:
//    case GL_LINE_STRIP:
//    case GL_LINE_LOOP:
    case GL_TRIANGLES:
    case GL_TRIANGLE_STRIP:
    case GL_TRIANGLE_FAN:
    case GL_QUADS:
//    case GL_QUAD_STRIP:
    case GL_POLYGON:
      beginValid_ = true;
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  }

  rasterMode_ = mode;

  // Initialize for default triangle
  triangle_[0] = &vertices[0];
  triangle_[1] = &vertices[1];
  triangle_[2] = &vertices[2];
  bFlipFlop_ = true;
  vertIdx_   = 0;
}

//-----------------------------------------------------------------------------
void
CASoftGLESFixed::glEnd()
{
  if(beginValid_ == false)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  beginValid_ = false;
}

//-----------------------------------------------------------------------------
void
CASoftGLESFixed::glVertex4x(GLfixed x, GLfixed y, GLfixed z, GLfixed w)
{
  if(beginValid_ == false)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  SVertexFx v;

  // Set vertex
  v.vo.x.value = x;
  v.vo.y.value = y;
  v.vo.z.value = z;
  v.vo.w.value = w;
  // Set normal
  v.n = normal_;
  // Set color
  v.cl = clCurrent;
  // Set texture
  v.t[0] = texCoordCurrent_[0];
  v.t[1] = texCoordCurrent_[1];
  // Vertex not processed yet
  v.processed = false;

  vertexShaderTransform(v);
}

//-----------------------------------------------------------------------------
void
CASoftGLESFixed::glColor4x(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha)
{
  clCurrent.r.value = red;
  clCurrent.g.value = green;
  clCurrent.b.value = blue;
  clCurrent.a.value = alpha;
}

//-----------------------------------------------------------------------------
void
CASoftGLESFixed::glTexCoord4x(GLfixed s, GLfixed t, GLfixed r, GLfixed q)
{
  texCoordCurrent_[0].value = s;
  texCoordCurrent_[1].value = t;
  texCoordCurrent_[2].value = r;
  texCoordCurrent_[3].value = q;
}

//-----------------------------------------------------------------------------
void
CASoftGLESFixed::glNormal3x(GLfixed nx, GLfixed ny, GLfixed nz)
{
  normal_.x.value = nx;
  normal_.y.value = ny;
  normal_.z.value = nz;

  if(normalizeEnabled_  == true)
    normal_.normalize();
}

//-----------------------------------------------------------------------------
void
CASoftGLESFixed::glGetFloatv(GLenum pname, GLfloat * params)
{
  // Return 4x4 matrix
#ifdef ROW_MAJOR
  #define GL_GET_MATRIX_COPY(matrix) \
    for(int i(0); i < 4; i++) \
      for(int j(0); j < 4; j++) \
        params[i*4+j] = matrix[j*4+i]
#else
  #define GL_GET_MATRIX_COPY(matrix) \
    for(int i(0); i < 16; i++) \
      params[i] = matrix[i]
#endif

  switch(pname)
  {
    case GL_MATRIX_MODE:
    {
      params[0] = matrixMode_;
      break;
    }
    case GL_MODELVIEW_MATRIX:
    {
      GL_GET_MATRIX_COPY(pCurrentModelView_->matrix);
      break;
    }
    case GL_MAX_MODELVIEW_STACK_DEPTH:
    case GL_MODELVIEW_STACK_DEPTH:
    {
      params[0] = GL_MATRIX_MODELVIEW_STACK_SIZE;
      break;
    }
    case GL_PROJECTION_MATRIX:
    {
      GL_GET_MATRIX_COPY(pCurrentProjection_->matrix);
      break;
    }
    case GL_MAX_PROJECTION_STACK_DEPTH:
    case GL_PROJECTION_STACK_DEPTH:
    {
      params[0] = GL_MATRIX_PROJECTION_STACK_SIZE;
      break;
    }
    case GL_TEXTURE_MATRIX:
    {
      GL_GET_MATRIX_COPY(pCurrentTexture_->matrix);
      break;
    }
    case GL_MAX_TEXTURE_STACK_DEPTH:
    case GL_TEXTURE_STACK_DEPTH:
    {
      params[0] = GL_MATRIX_TEXTURE_STACK_SIZE;
      break;
    }
    default:
      setError(GL_INVALID_ENUM);
  };
}

//-----------------------------------------------------------------------------
void
CASoftGLESFixed::glBlendFunc(GLenum sfactor, GLenum dfactor)
{
  blendSFactor_ = sfactor;
  blendDFactor_ = dfactor;

  if((sfactor == GL_ONE) && (dfactor == GL_ZERO))
    blendFast_ = FB_SOURCE;
  else if((sfactor == GL_ZERO) && (dfactor == GL_ONE))
    blendFast_ = FB_DEST;
  else if((sfactor == GL_SRC_ALPHA) && (dfactor == GL_ONE_MINUS_SRC_ALPHA))
    blendFast_ = FB_BLEND;
  else
    blendFast_ = FB_OTHER;
}

//-----------------------------------------------------------------------------
void
CASoftGLESFixed::glTexEnvf(GLenum target, GLenum pname, GLfloat param)
{
  if(target != GL_TEXTURE_ENV)
  {
    setError(GL_INVALID_ENUM);
    return;
  }
  if(pname != GL_TEXTURE_ENV_MODE)
  {
    setError(GL_INVALID_ENUM);
    return;
  }

  switch((GLenum)param)
  {
    case GL_MODULATE:
    case GL_DECAL:
    case GL_BLEND:
    case GL_REPLACE:
      texEnvMode_ = (GLenum)param;
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };
}

//-----------------------------------------------------------------------------
void
CASoftGLESFixed::glTexEnvfv(GLenum target, GLenum pname, const GLfloat * params)
{
  if(target != GL_TEXTURE_ENV)
  {
    setError(GL_INVALID_ENUM);
    return;
  }
  if((pname != GL_TEXTURE_ENV_MODE) && (pname != GL_TEXTURE_ENV_COLOR))
  {
    setError(GL_INVALID_ENUM);
    return;
  }

  switch(pname)
  {
    case GL_TEXTURE_ENV_MODE:
      switch((GLenum)params[0])
      {
        case GL_MODULATE:
        case GL_DECAL:
        case GL_BLEND:
        case GL_REPLACE:
          texEnvMode_ = (GLenum)params[0];
          break;
        default:
          setError(GL_INVALID_ENUM);
          return;
      };
      break;
    case GL_TEXTURE_ENV_COLOR:
      texEnvColor_.r = params[0];
      texEnvColor_.g = params[1];
      texEnvColor_.b = params[2];
      texEnvColor_.a = params[3];
      texEnvColorFX_.r = texEnvColor_.r.value >> (16 - SHIFT_COLOR);
      texEnvColorFX_.g = texEnvColor_.g.value >> (16 - SHIFT_COLOR);
      texEnvColorFX_.b = texEnvColor_.b.value >> (16 - SHIFT_COLOR);
      texEnvColorFX_.a = texEnvColor_.a.value >> (16 - SHIFT_COLOR);
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };
}

//-----------------------------------------------------------------------------
inline CFixed
my_pow(CFixed x, int y)
{
  CFixed rv(x);
  for(int i(1); i < y; i++)
    rv *= x;
  return rv;
}

//-----------------------------------------------------------------------------
void
CASoftGLESFixed::vertexShaderTransform(SVertexFx & v)
{
  _vertexShaderTransform(v);
}

//-----------------------------------------------------------------------------
void
CASoftGLESFixed::vertexShaderLight(SVertexFx & v)
{
  _vertexShaderLight(v);
}

//-----------------------------------------------------------------------------
void
CASoftGLESFixed::fragmentCull(SVertexFx & v0, SVertexFx & v1, SVertexFx & v2)
{
  _fragmentCull(v0, v1, v2);
}

//-----------------------------------------------------------------------------
void
CASoftGLESFixed::fragmentClip(SVertexFx & v0, SVertexFx & v1, SVertexFx & v2)
{
  _fragmentClip(v0, v1, v2);
}

//-----------------------------------------------------------------------------
void
CASoftGLESFixed::primitiveAssembly(SVertexFx & v)
{
  _primitiveAssembly(v);
}

//-----------------------------------------------------------------------------
void
CASoftGLESFixed::_glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
  if(bBufVertexEnabled_ == false)
    return;

  GLint idxVertex  (first * bufVertex_.size);
  GLint idxColor   (first * bufColor_.size);
  GLint idxNormal  (first * bufNormal_.size);
  GLint idxTexCoord(first * bufTexCoord_.size);
  SVertexFx v;

  glBegin(mode);

  // Process all vertices
  for(GLint i(0); i < count; i++)
  {
    // Vertex
    switch(bufVertex_.type)
    {
      case GL_FLOAT:
        v.vo.x = (((GLfloat *)bufVertex_.pointer)[idxVertex++]);
        v.vo.y = (((GLfloat *)bufVertex_.pointer)[idxVertex++]);
        v.vo.z = (((GLfloat *)bufVertex_.pointer)[idxVertex++]);
        v.vo.w = 1;
        break;
      case GL_FIXED:
        v.vo.x.value = ((GLfixed *)bufVertex_.pointer)[idxVertex++];
        v.vo.y.value = ((GLfixed *)bufVertex_.pointer)[idxVertex++];
        v.vo.z.value = ((GLfixed *)bufVertex_.pointer)[idxVertex++];
        v.vo.w = 1;
        break;
    };

    // Normal
    if(bBufNormalEnabled_ == true)
    {
      switch(bufNormal_.type)
      {
        case GL_FLOAT:
          v.n.x = (((GLfloat *)bufNormal_.pointer)[idxNormal++]);
          v.n.y = (((GLfloat *)bufNormal_.pointer)[idxNormal++]);
          v.n.z = (((GLfloat *)bufNormal_.pointer)[idxNormal++]);
          break;
        case GL_FIXED:
          v.n.x.value = ((GLfixed *)bufNormal_.pointer)[idxNormal++];
          v.n.y.value = ((GLfixed *)bufNormal_.pointer)[idxNormal++];
          v.n.z.value = ((GLfixed *)bufNormal_.pointer)[idxNormal++];
          break;
      };
    }
    else
      v.n = normal_;

    // Color
    if(lightingEnabled_ == false)
    {
      if(bBufColorEnabled_ == true)
      {
        switch(bufColor_.type)
        {
          case GL_FLOAT:
            v.cl.r = ((GLfloat *)bufColor_.pointer)[idxColor++];
            v.cl.g = ((GLfloat *)bufColor_.pointer)[idxColor++];
            v.cl.b = ((GLfloat *)bufColor_.pointer)[idxColor++];
            v.cl.a = ((GLfloat *)bufColor_.pointer)[idxColor++];
            break;
          case GL_FIXED:
            v.cl.r.value = ((GLfixed *)bufColor_.pointer)[idxColor++];
            v.cl.g.value = ((GLfixed *)bufColor_.pointer)[idxColor++];
            v.cl.b.value = ((GLfixed *)bufColor_.pointer)[idxColor++];
            v.cl.a.value = ((GLfixed *)bufColor_.pointer)[idxColor++];
            break;
        };
      }
      else
        v.cl = clCurrent;
    }

    // Textures
    if((texturesEnabled_ == true) && (bBufTexCoordEnabled_ == true))
    {
      switch(bufTexCoord_.type)
      {
        case GL_FLOAT:
          v.t[0] =     CFixed(((GLfloat *)bufTexCoord_.pointer)[idxTexCoord++]);
          v.t[1] = 1 - CFixed(((GLfloat *)bufTexCoord_.pointer)[idxTexCoord++]);
          break;
        case GL_FIXED:
          v.t[0].value =                  ((GLfixed *)bufTexCoord_.pointer)[idxTexCoord++];
          v.t[1].value = (gl_fpfromi(1) - ((GLfixed *)bufTexCoord_.pointer)[idxTexCoord++]);
          break;
      };
    }

    v.processed = false;
    vertexShaderTransform(v);
  }

  glEnd();
}

//-----------------------------------------------------------------------------
void
CASoftGLESFixed::_vertexShaderTransform(SVertexFx & v)
{
  // --------------
  // Transformation
  // --------------
  // Model-View matrix
  //   from 'object coordinates' to 'eye coordinates'
  CALC_OBJ_TO_EYE(v);
  // Projection matrix
  //   from 'eye coordinates' to 'clip coordinates'
  CALC_EYE_TO_CLIP(v);

  // Set clip flags
  setClipFlags(v);

  if(v.clip == 0)
  {
    // Perspective division
    //   from 'clip coordinates' to 'normalized device coordinates'
    CALC_CLIP_TO_NDEV(v);
  }

  primitiveAssembly(v);
}

//-----------------------------------------------------------------------------
void
CASoftGLESFixed::_vertexShaderLight(SVertexFx & v)
{
  // --------
  // Lighting
  // --------
  if(lightingEnabled_ == true)
  {
    SColorFx c(0, 0, 0, 0);

    // Normal Rotation
    pCurrentModelView_->transform3(v.n, v.n2);

    for(int iLight(0); iLight < 8; iLight++)
    {
      if(lights_[iLight].enabled == true)
      {
        // Ambient light (it's everywhere!)
        c += lights_[iLight].ambient * matColorAmbient_;

        // Diffuse light
        CFixed diffuse = lights_[iLight].direction.dotProduct(v.n2);
        if(diffuse > 0.0f)
        {
          c += lights_[iLight].diffuse * matColorDiffuse_ * diffuse;
        }

        if(matShininess_ >= 0.5f)
        {
          // Specular light
          TVector3<CFixed> eye(v.ve);
          eye.normalize();
          CFixed specular = lights_[iLight].direction.getReflection(v.n2).dotProduct(eye);
          if(specular > 0.0f)
          {
            specular = my_pow(specular, (int)(matShininess_ + 0.5f));
            c += lights_[iLight].specular * matColorSpecular_ * specular;
          }
        }
      }
    }

    // Clamp to 0..1
    c.clamp();
    // Multiply vertex color by calculated color
    v.cl = c;
  }

  // ---
  // Fog
  // ---
  if(fogEnabled_ == true)
  {
    CFixed partFog, partColor;
    partFog.value = clampfx(gl_fpdiv(abs(v.ve.z.value) - fogStart_, fogEnd_ - fogStart_));
    partColor = 1 - partFog;
    v.cl = ((v.cl * partColor) + (fogColor_ * partFog)).getClamped();
  }
}

//-----------------------------------------------------------------------------
void
CASoftGLESFixed::_fragmentCull(SVertexFx & v0, SVertexFx & v1, SVertexFx & v2)
{
  if(v0.clip & v1.clip & v2.clip)
    return; // Not visible

  fragmentClip(v0, v1, v2);
}

//-----------------------------------------------------------------------------
void
CASoftGLESFixed::_fragmentClip(SVertexFx & v0, SVertexFx & v1, SVertexFx & v2)
{
  // ----------------------
  // Vertex shader lighting
  // ----------------------
  if(bSmoothShading_ == true)
  {
    if(v0.processed == false)
    {
      vertexShaderLight(v0);
      v0.processed = true;
    }
    if(v1.processed == false)
    {
      vertexShaderLight(v1);
      v1.processed = true;
    }
  }
  if(v2.processed == false)
  {
    vertexShaderLight(v2);
    v2.processed = true;
  }

  rasterTriangleClip(v0, v1, v2);
}

//-----------------------------------------------------------------------------
void
CASoftGLESFixed::_primitiveAssembly(SVertexFx & v)
{
  // Copy vertex into vertex buffer
  *triangle_[vertIdx_] = v;

  // ------------------
  // Primitive Assembly
  // ------------------
  switch(rasterMode_)
  {
    case GL_TRIANGLES:
    {
      if(vertIdx_ == 2)
        fragmentCull(*triangle_[0], *triangle_[1], *triangle_[2]);
      vertIdx_++;
      if(vertIdx_ > 2)
        vertIdx_ = 0;
      break;
    }
    case GL_TRIANGLE_STRIP:
    {
      if(vertIdx_ == 2)
      {
        fragmentCull(*triangle_[0], *triangle_[1], *triangle_[2]);
        // Swap 3rd with 1st or 2nd vertex pointer
        if(bFlipFlop_ == true)
        {
          SVertexFx * pTemp = triangle_[0];
          triangle_[0] = triangle_[2];
          triangle_[2] = pTemp;
        }
        else
        {
          SVertexFx * pTemp = triangle_[1];
          triangle_[1] = triangle_[2];
          triangle_[2] = pTemp;
        }
        bFlipFlop_ = !bFlipFlop_;
      }
      else
        vertIdx_++;
      break;
    }
    case GL_POLYGON:
    case GL_TRIANGLE_FAN:
    {
      if(vertIdx_ == 2)
      {
        fragmentCull(*triangle_[0], *triangle_[1], *triangle_[2]);
        // Swap 3rd with 2nd vertex pointer
        SVertexFx * pTemp = triangle_[1];
        triangle_[1] = triangle_[2];
        triangle_[2] = pTemp;
      }
      else
        vertIdx_++;
      break;
    }
    case GL_QUADS:
    {
      if(vertIdx_ == 2)
      {
        fragmentCull(*triangle_[0], *triangle_[1], *triangle_[2]);
        // Swap 3rd with 2nd vertex pointer
        SVertexFx * pTemp = triangle_[1];
        triangle_[1] = triangle_[2];
        triangle_[2] = pTemp;
        // QUADS are TRIANGLE_FAN with only 2 triangles
        if(bFlipFlop_ == true)
          bFlipFlop_ = false;
        else
          vertIdx_ = 0;
      }
      else
        vertIdx_++;
      break;
    }
  };
}

//-----------------------------------------------------------------------------
// Clipping based on code from TinyGL
// t = -(plane.dotProduct(from) + planeDistance) / plane.dotProduct(delta);
#define clip_func(name,sign,dir,dir1,dir2)         \
inline CFixed                                      \
name(TVector4<CFixed> & c, TVector4<CFixed> & a, TVector4<CFixed> & b) \
{                                                  \
  TVector4<CFixed> delta = b - a;                  \
  CFixed t, den;                                   \
                                                   \
  den = 0 - (0 sign delta.dir) + delta.w;          \
  if(den == 0)                                     \
    t = 0;                                         \
  else                                             \
    t = (0 sign a.dir - a.w) / den;                \
                                                   \
  c.dir1 = a.dir1 + t * delta.dir1;                \
  c.dir2 = a.dir2 + t * delta.dir2;                \
  c.w    = a.w    + t * delta.w;                   \
  c.dir  = 0 sign c.w;                             \
                                                   \
  return t;                                        \
}

clip_func(clip_xmin,-,x,y,z)
clip_func(clip_xmax,+,x,y,z)
clip_func(clip_ymin,-,y,x,z)
clip_func(clip_ymax,+,y,x,z)
clip_func(clip_zmin,-,z,x,y)
clip_func(clip_zmax,+,z,x,y)

CFixed (*clip_proc[6])(TVector4<CFixed> &, TVector4<CFixed> &, TVector4<CFixed> &) =
{
  clip_xmin,
  clip_xmax,
  clip_ymin,
  clip_ymax,
  clip_zmin,
  clip_zmax
};

#define CLIP_FUNC(plane,c,a,b) clip_proc[plane](c,a,b)

//-----------------------------------------------------------------------------
void
CASoftGLESFixed::rasterTriangleClip(SVertexFx & v0, SVertexFx & v1, SVertexFx & v2, uint32_t clipBit)
{
  uint32_t cc[3] =
  {
    v0.clip,
    v1.clip,
    v2.clip
  };
  uint32_t clipOr = cc[0] | cc[1] | cc[2];
  uint32_t clipAnd;

  if(clipOr == 0)
  {
    // Completely inside
    this->rasterTriangle(v0, v1, v2);
  }
  else
  {
    clipAnd = cc[0] & cc[1] & cc[2];
    if(clipAnd != 0)
      return; // Completely outside

    // find the next clip bit
    while(clipBit < 6 && (clipOr & (1 << clipBit)) == 0)
    {
      clipBit++;
    }

    if(clipBit >= 6)
    {
      // Rounding error?
      return;
    }

    uint32_t clipMask = (1 << clipBit);
    uint32_t clipXor  = (cc[0] ^ cc[1] ^ cc[2]) & clipMask;

    if(clipXor != 0)
    {
      // 1 vertex outside
      SVertexFx vNew1;  // Intersection of 1 line
      SVertexFx vNew2;  // Intersection of 1 line
      CFixed tt;

      // Rearrange vertices
      SVertexFx * v[3];
           if(cc[0] & clipMask){v[0] = &v0; v[1] = &v1; v[2] = &v2;} // v0 == outside
      else if(cc[1] & clipMask){v[0] = &v1; v[1] = &v2; v[2] = &v0;} // v1 == outside
      else                     {v[0] = &v2; v[1] = &v0; v[2] = &v1;} // v2 == outside

      // Interpolate 1-0
      tt = CLIP_FUNC(clipBit, vNew1.vc, v[1]->vc, v[0]->vc);
      interpolateVertex(vNew1, *v[1], *v[0], tt);
      // Interpolate 2-0
      tt = CLIP_FUNC(clipBit, vNew2.vc, v[2]->vc, v[0]->vc);
      interpolateVertex(vNew2, *v[2], *v[0], tt);

      // Raster 2 new triangles
      this->rasterTriangleClip(vNew1, *v[1], *v[2], clipBit + 1);
      this->rasterTriangleClip(vNew2, vNew1, *v[2], clipBit + 1);
    }
    else
    {
      // 2 vertices outside
      SVertexFx vNew1;  // Intersection of 1 line
      SVertexFx vNew2;  // Intersection of 1 line
      CFixed tt;

      // Rearrange vertices
      SVertexFx * v[3];
           if((cc[0] & clipMask) == 0){v[0] = &v0; v[1] = &v1; v[2] = &v2;} // v0 == inside
      else if((cc[1] & clipMask) == 0){v[0] = &v1; v[1] = &v2; v[2] = &v0;} // v1 == inside
      else                            {v[0] = &v2; v[1] = &v0; v[2] = &v1;} // v2 == inside

      // Interpolate 0-1
      tt = CLIP_FUNC(clipBit, vNew1.vc, v[0]->vc, v[1]->vc);
      interpolateVertex(vNew1, *v[0], *v[1], tt);
      // Interpolate 0-2
      tt = CLIP_FUNC(clipBit, vNew2.vc, v[0]->vc, v[2]->vc);
      interpolateVertex(vNew2, *v[0], *v[2], tt);

      // Raster new triangle
      this->rasterTriangleClip(*v[0], vNew1, vNew2, clipBit + 1);
    }
  }
}

//-----------------------------------------------------------------------------
void
CASoftGLESFixed::interpolateVertex(SVertexFx & c, SVertexFx & a, SVertexFx & b, CFixed t)
{
  // Color
  if(shadingModel_ == GL_SMOOTH)
    c.cl = CALC_INTERSECTION(a.cl, b.cl, t);
  else
    c.cl = b.cl;

  // Texture coordinates
  if(texturesEnabled_ == true)
  {
    c.t[0] = CALC_INTERSECTION(a.t[0], b.t[0], t);
    c.t[1] = CALC_INTERSECTION(a.t[1], b.t[1], t);
  }

  // Set clip flags
  setClipFlags(c);

  if(c.clip == 0)
  {
    // Perspective division
    //   from 'clip coordinates' to 'normalized device coordinates'
    CALC_CLIP_TO_NDEV(c);
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CSoftGLESFixed::CSoftGLESFixed()
 : CASoftGLESFixed()
 , CAGLESTextures()
 , pZBuffer_(NULL)
{
}

//-----------------------------------------------------------------------------
CSoftGLESFixed::~CSoftGLESFixed()
{
  if(pZBuffer_)
    delete pZBuffer_;
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glClear(GLbitfield mask)
{
  if(mask & GL_COLOR_BUFFER_BIT)
  {
    color_t color(fpRGB(clClear.r, clClear.g, clClear.b));
    int yoff = renderSurface->mode.height - (viewportHeight + viewportYOffset);
    int xoff = viewportXOffset;

    switch(renderSurface->mode.bpp)
    {
      case 8:
      {
        for(int32_t y(0); y < viewportHeight; y++)
          for(int32_t x(0); x < viewportWidth; x++)
            ((uint8_t  *)renderSurface->p)[(y + yoff) * renderSurface->mode.xpitch + (x + xoff)] = color;
        break;
      }
      case 16:
      {
        for(int32_t y(0); y < viewportHeight; y++)
          for(int32_t x(0); x < viewportWidth; x++)
            ((uint16_t *)renderSurface->p)[(y + yoff) * renderSurface->mode.xpitch + (x + xoff)] = color;
        break;
      }
      case 32:
      {
        for(int32_t y(0); y < viewportHeight; y++)
          for(int32_t x(0); x < viewportWidth; x++)
            ((uint32_t *)renderSurface->p)[(y + yoff) * renderSurface->mode.xpitch + (x + xoff)] = color;
        break;
      }
    };
  }
  if(mask & GL_DEPTH_BUFFER_BIT)
  {
    if(pZBuffer_ != NULL)
    {
      uint32_t zc = (zClearValue_ << 16) | zClearValue_;
      for(int i(0); i < (viewportPixelCount / 2); i++)
        ((uint32_t *)pZBuffer_)[i] = zc;
    }
  }
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
  CASoftGLESFixed::glViewport(x, y, width, height);

  // Update z-buffer
  if(depthTestEnabled_ == true)
    zbuffer(true);
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::rasterTriangle(SVertexFx & v0, SVertexFx & v1, SVertexFx & v2)
{
  _rasterTriangle(v0, v1, v2);
}

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::zbuffer(bool enable)
{
  if(enable == true)
  {
    // (Re)create z-buffer
    if(pZBuffer_ == 0)
    {
      if(pZBuffer_)
        delete pZBuffer_;

      // FIXME: Should be viewportPixelCount
      pZBuffer_ = new uint16_t[(viewportWidth + viewportXOffset) * (viewportHeight + viewportYOffset)];
      if(pZBuffer_ == NULL)
        setError(GL_OUT_OF_MEMORY);
    }
  }
}

//-----------------------------------------------------------------------------
inline void
fxFloorDivMod(int32_t Numerator, int32_t Denominator, unsigned int shift, int32_t &Floor, int32_t &Mod)
{
  if(Numerator >= 0)
  {
    // positive case, C is okay
    Floor = (((int64_t)Numerator) << shift) / Denominator;
    Mod   = (((int64_t)Numerator) << shift) % Denominator;
  }
  else
  {
    // Numerator is negative, do the right thing
    Floor = -((-(((int64_t)Numerator) << shift)) / Denominator);
    Mod   =   (-(((int64_t)Numerator) << shift)) % Denominator;
    if(Mod)
    {
      // there is a remainder
      Floor--; Mod = Denominator - Mod;
    }
  }
}

//-----------------------------------------------------------------------------
template <class T>
struct TInterpolation
{
  T current;
  T increment;
};

//-----------------------------------------------------------------------------
void
CSoftGLESFixed::_rasterTriangle(SVertexFx & v0, SVertexFx & v1, SVertexFx & v2)
{
  // -------
  // Culling
  // -------
  if(cullFaceEnabled_ == true)
  {
    // Always invisible when culling both front and back
    if(cullFaceMode_ == GL_FRONT_AND_BACK)
      return;

    CFixed vnz =
      (v0.vd.x - v2.vd.x) * (v1.vd.y - v2.vd.y) -
      (v0.vd.y - v2.vd.y) * (v1.vd.x - v2.vd.x);

    if(vnz.value == 0)
      return;

    if((vnz.value < 0) == bCullCW_)
      return;
  }

  SRasterVertex vtx0, vtx1, vtx2;

  vtx0.x   = ((xA_ * v0.vd.x) + xB_).value;
  vtx0.y   = ((yA_ * v0.vd.y) + yB_).value;
  vtx0.z   = ((zA_ * v0.vd.z) + zB_).value;
  vtx0.c.r = v0.cl.r.value >> (16 - SHIFT_COLOR);
  vtx0.c.g = v0.cl.g.value >> (16 - SHIFT_COLOR);
  vtx0.c.b = v0.cl.b.value >> (16 - SHIFT_COLOR);
  vtx0.c.a = v0.cl.a.value >> (16 - SHIFT_COLOR);
#ifdef CONFIG_GL_PERSPECTIVE_CORRECT_TEXTURES
  vtx0.t.u = v0.t[0] * (1 << pCurrentTex_->bitWidth_);
  vtx0.t.v = v0.t[1] * (1 << pCurrentTex_->bitHeight_);
  vtx0.t.w = v0.vd.w;
#else
  vtx0.t.u = v0.t[0].value << (SHIFT_TEX + pCurrentTex_->bitWidth_ - 16);
  vtx0.t.v = v0.t[1].value << (SHIFT_TEX + pCurrentTex_->bitWidth_ - 16);
#endif

  vtx1.x   = ((xA_ * v1.vd.x) + xB_).value;
  vtx1.y   = ((yA_ * v1.vd.y) + yB_).value;
  vtx1.z   = ((zA_ * v1.vd.z) + zB_).value;
  vtx1.c.r = v1.cl.r.value >> (16 - SHIFT_COLOR);
  vtx1.c.g = v1.cl.g.value >> (16 - SHIFT_COLOR);
  vtx1.c.b = v1.cl.b.value >> (16 - SHIFT_COLOR);
  vtx1.c.a = v1.cl.a.value >> (16 - SHIFT_COLOR);
#ifdef CONFIG_GL_PERSPECTIVE_CORRECT_TEXTURES
  vtx1.t.u = v1.t[0] * (1 << pCurrentTex_->bitWidth_);
  vtx1.t.v = v1.t[1] * (1 << pCurrentTex_->bitHeight_);
  vtx1.t.w = v1.vd.w;
#else
  vtx1.t.u = v1.t[0].value << (SHIFT_TEX + pCurrentTex_->bitWidth_ - 16);
  vtx1.t.v = v1.t[1].value << (SHIFT_TEX + pCurrentTex_->bitWidth_ - 16);
#endif

  vtx2.x   = ((xA_ * v2.vd.x) + xB_).value;
  vtx2.y   = ((yA_ * v2.vd.y) + yB_).value;
  vtx2.z   = ((zA_ * v2.vd.z) + zB_).value;
  vtx2.c.r = v2.cl.r.value >> (16 - SHIFT_COLOR);
  vtx2.c.g = v2.cl.g.value >> (16 - SHIFT_COLOR);
  vtx2.c.b = v2.cl.b.value >> (16 - SHIFT_COLOR);
  vtx2.c.a = v2.cl.a.value >> (16 - SHIFT_COLOR);
#ifdef CONFIG_GL_PERSPECTIVE_CORRECT_TEXTURES
  vtx2.t.u = v2.t[0] * (1 << pCurrentTex_->bitWidth_);
  vtx2.t.v = v2.t[1] * (1 << pCurrentTex_->bitHeight_);
  vtx2.t.w = v2.vd.w;
#else
  vtx2.t.u = v2.t[0].value << (SHIFT_TEX + pCurrentTex_->bitWidth_ - 16);
  vtx2.t.v = v2.t[1].value << (SHIFT_TEX + pCurrentTex_->bitWidth_ - 16);
#endif

  // Bubble sort the 3 vertexes
  const SRasterVertex * vhi(&vtx0);
  const SRasterVertex * vmi(&vtx1);
  const SRasterVertex * vlo(&vtx2);
  {
    const SRasterVertex * vtemp;
    // Swap bottom with middle?
    if(vlo->y > vmi->y)
    {
      vtemp = vmi;
      vmi   = vlo;
      vlo   = vtemp;
    }
    // Swap middle with top?
    if(vmi->y > vhi->y)
    {
      vtemp = vhi;
      vhi   = vmi;
      vmi   = vtemp;
      // Swap bottom with middle again?
      if(vlo->y > vmi->y)
      {
        vtemp = vmi;
        vmi   = vlo;
        vlo   = vtemp;
      }
    }
  }

  if(texturesEnabled_ == false)
  {
    if(bSmoothShading_ == false)
    {
      #include "raster.h"
    }
    else
    {
      #define RASTER_ENABLE_SMOOTH_COLORS
      #include "raster.h"
    }
  }
  else
  {
    #define RASTER_ENABLE_TEXTURES
    #include "raster.h"
  }
}
