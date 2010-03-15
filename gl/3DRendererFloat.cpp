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

#include "3DRendererFloat.h"
#include "vhl/fixedPoint.h"
#include "vhl/matrix.h"
#include "color.h"
#ifdef ENABLE_PROFILING
#include "prof/prof.h"
#endif

#include "stdlib.h"
#include "math.h"
#include "mathlib.h"


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
  GLfloat inv_w; \
  inv_w = 1.0f / (v).vc.w; \
  (v).vd.x = (v).vc.x * inv_w; \
  (v).vd.y = (v).vc.y * inv_w; \
  (v).vd.z = (v).vc.z * inv_w; \
  (v).vd.w = inv_w

#define CLIP_ROUNDING_ERROR (0.00001f) // (1E-5)
inline void setClipFlags(SVertexF & v)
{
  GLfloat w = v.vc.w * (1.0f + CLIP_ROUNDING_ERROR);
  v.clip = ((v.vc.x < -w)     ) |
           ((v.vc.x >  w) << 1) |
           ((v.vc.y < -w) << 2) |
           ((v.vc.y >  w) << 3) |
           ((v.vc.z < -w) << 4) |
           ((v.vc.z >  w) << 5);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CSoft3DRendererFloat::CSoft3DRendererFloat()
 : CAGLFixedToFloat()
 , CAGLErrorHandler()
 , CAGLBuffers()
 , CAGLMatrixFloat()
 , pRaster_(NULL)
{
  state_.culling.enabled = false;
  state_.culling.mode = GL_BACK;
  state_.culling.front = GL_CCW;
  state_.culling.cullCW = true;

  state_.hints.fog = GL_DONT_CARE;
  state_.hints.lineSmooth = GL_DONT_CARE;
  state_.hints.perspectiveCorrection = GL_DONT_CARE;
  state_.hints.pointSmooth = GL_DONT_CARE;

  state_.clCurrent.r = 1.0f;
  state_.clCurrent.g = 1.0f;
  state_.clCurrent.b = 1.0f;
  state_.clCurrent.a = 1.0f;

  state_.clClear.r = 0.0f;
  state_.clClear.g = 0.0f;
  state_.clClear.b = 0.0f;
  state_.clClear.a = 0.0f;

  state_.shadingModel = GL_FLAT;
  state_.smoothShading = (state_.shadingModel == GL_SMOOTH);

  state_.texturing.enabled = false;
  state_.texturing.coordCurrent[0] = 0.0f;
  state_.texturing.coordCurrent[1] = 0.0f;
  state_.texturing.coordCurrent[2] = 0.0f;
  state_.texturing.coordCurrent[3] = 1.0f;
  state_.texturing.envMode = GL_MODULATE;
  state_.texturing.envColor = TColor<GLfloat>(0, 0, 0, 0);

  state_.depthTest.enabled = false;
  state_.depthTest.mask = true;
  state_.depthTest.function = GL_LESS;
  state_.depthTest.clear = 0.0f;
  state_.depthTest.rangeNear = 0.0f;
  state_.depthTest.rangeFar = 1.0f;

  state_.blending.enabled = false;
  state_.blending.sourceFactor = GL_ONE;
  state_.blending.destFactor = GL_ZERO;

  state_.alphaTest.enabled = false;
  state_.alphaTest.func = GL_ALWAYS;
  state_.alphaTest.value = 0.0f;

  state_.lighting.enabled = false;
  state_.lighting.materialColorEnabled = false;
  state_.materialFront.colorMode = GL_AMBIENT_AND_DIFFUSE;
  state_.materialBack.colorMode = GL_AMBIENT_AND_DIFFUSE;
  for(int iLight(0); iLight < 8; iLight++)
  {
    state_.lighting.light[iLight].enabled = false;

    state_.lighting.light[iLight].ambient.r = 0.0f;
    state_.lighting.light[iLight].ambient.g = 0.0f;
    state_.lighting.light[iLight].ambient.b = 0.0f;
    state_.lighting.light[iLight].ambient.a = 1.0f;

    if(iLight == 0)
    {
      state_.lighting.light[iLight].diffuse.r = 1.0f;
      state_.lighting.light[iLight].diffuse.g = 1.0f;
      state_.lighting.light[iLight].diffuse.b = 1.0f;
      state_.lighting.light[iLight].diffuse.a = 1.0f;

      state_.lighting.light[iLight].specular.r = 1.0f;
      state_.lighting.light[iLight].specular.g = 1.0f;
      state_.lighting.light[iLight].specular.b = 1.0f;
      state_.lighting.light[iLight].specular.a = 1.0f;
    }
    else
    {
      state_.lighting.light[iLight].diffuse.r = 0.0f;
      state_.lighting.light[iLight].diffuse.g = 0.0f;
      state_.lighting.light[iLight].diffuse.b = 0.0f;
      state_.lighting.light[iLight].diffuse.a = 0.0f;

      state_.lighting.light[iLight].specular.r = 0.0f;
      state_.lighting.light[iLight].specular.g = 0.0f;
      state_.lighting.light[iLight].specular.b = 0.0f;
      state_.lighting.light[iLight].specular.a = 0.0f;
    }

    state_.lighting.light[iLight].position.x = 0.0f;
    state_.lighting.light[iLight].position.y = 0.0f;
    state_.lighting.light[iLight].position.z = 1.0f;
    state_.lighting.light[iLight].position.w = 0.0f;
    state_.lighting.light[iLight].positionNormal = state_.lighting.light[iLight].position;
  }
  state_.lighting.normalizeEnabled = false;
  state_.lighting.normal = TVector3<GLfloat>(0.0f, 0.0f, 1.0f);

  state_.materialFront.ambient.r  = state_.materialBack.ambient.r  = 0.2f;
  state_.materialFront.ambient.g  = state_.materialBack.ambient.g  = 0.2f;
  state_.materialFront.ambient.b  = state_.materialBack.ambient.b  = 0.2f;
  state_.materialFront.ambient.a  = state_.materialBack.ambient.a  = 1.0f;
  state_.materialFront.diffuse.r  = state_.materialBack.diffuse.r  = 0.8f;
  state_.materialFront.diffuse.g  = state_.materialBack.diffuse.g  = 0.8f;
  state_.materialFront.diffuse.b  = state_.materialBack.diffuse.b  = 0.8f;
  state_.materialFront.diffuse.a  = state_.materialBack.diffuse.a  = 1.0f;
  state_.materialFront.specular.r = state_.materialBack.specular.r = 0.0f;
  state_.materialFront.specular.g = state_.materialBack.specular.g = 0.0f;
  state_.materialFront.specular.b = state_.materialBack.specular.b = 0.0f;
  state_.materialFront.specular.a = state_.materialBack.specular.a = 1.0f;
  state_.materialFront.emission.r = state_.materialBack.emission.r = 0.0f;
  state_.materialFront.emission.g = state_.materialBack.emission.g = 0.0f;
  state_.materialFront.emission.b = state_.materialBack.emission.b = 0.0f;
  state_.materialFront.emission.a = state_.materialBack.emission.a = 1.0f;
  state_.materialFront.shininess  = state_.materialBack.shininess  = 0.0f;

  state_.fog.enabled = false;
  state_.fog.mode = GL_EXP;
  state_.fog.density = 1.0f;
  state_.fog.start = 0.0f;
  state_.fog.end = 1.0f;
  state_.fog.linear_scale = 1.0f;
  state_.fog.color = TColor<GLfloat>(0.0f, 0.0f, 0.0f, 0.0f);


  float zsize = (1 << (DEPTH_Z + SHIFT_Z));
  zA_ = ((zsize - 0.5) / 2.0);
  zB_ = ((zsize - 0.5) / 2.0) + (1 << (SHIFT_Z-1));

  bInBeginEnd_ = false;

  clipPlane_[0] = TVector4<GLfloat>( 1.0,  0.0,  0.0,  1.0); // left
  clipPlane_[1] = TVector4<GLfloat>(-1.0,  0.0,  0.0,  1.0); // right
  clipPlane_[2] = TVector4<GLfloat>( 0.0,  1.0,  0.0,  1.0); // bottom
  clipPlane_[3] = TVector4<GLfloat>( 0.0, -1.0,  0.0,  1.0); // top
  clipPlane_[4] = TVector4<GLfloat>( 0.0,  0.0,  1.0,  1.0); // near
  clipPlane_[5] = TVector4<GLfloat>( 0.0,  0.0, -1.0,  1.0); // far
}

//-----------------------------------------------------------------------------
CSoft3DRendererFloat::~CSoft3DRendererFloat()
{
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::setRaster(raster::IRasterizer * rast)
{
  pRaster_ = rast;

  if(renderSurface != NULL)
    pRaster_->setSurface(renderSurface);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::setSurface(CSurface * surface)
{
  IRenderer::setSurface(surface);

  if(pRaster_ != NULL)
    pRaster_->setSurface(renderSurface);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::glAlphaFunc(GLenum func, GLclampf ref)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

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

  state_.alphaTest.func = func;
  state_.alphaTest.value = mathlib::clamp<GLclampf>(ref, 0.0f, 1.0f);

  pRaster_->alphaFunc(state_.alphaTest.func, state_.alphaTest.value);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::glBindTexture(GLenum target, GLuint texture)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  pRaster_->bindTexture(target, texture);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::glBlendFunc(GLenum sfactor, GLenum dfactor)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  switch(sfactor)
  {
    case GL_ZERO:
    case GL_ONE:
    case GL_DST_COLOR:
    case GL_ONE_MINUS_DST_COLOR:
    case GL_SRC_ALPHA:
    case GL_ONE_MINUS_SRC_ALPHA:
    case GL_DST_ALPHA:
    case GL_ONE_MINUS_DST_ALPHA:
    //case GL_CONSTANT_COLOR:
    //case GL_CONSTANT_COLOR_EXT:
    //case GL_ONE_MINUS_CONSTANT_COLOR:
    //case GL_ONE_MINUS_CONSTANT_COLOR_EXT:
    //case GL_CONSTANT_ALPHA:
    //case GL_CONSTANT_ALPHA_EXT:
    //case GL_ONE_MINUS_CONSTANT_ALPHA:
    //case GL_ONE_MINUS_CONSTANT_ALPHA_EXT:
    //case GL_SRC_ALPHA_SATURATE:
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };

  switch(dfactor)
  {
    case GL_ZERO:
    case GL_ONE:
    case GL_SRC_COLOR:
    case GL_ONE_MINUS_SRC_COLOR:
    case GL_SRC_ALPHA:
    case GL_ONE_MINUS_SRC_ALPHA:
    case GL_DST_ALPHA:
    case GL_ONE_MINUS_DST_ALPHA:
    //case GL_CONSTANT_COLOR:
    //case GL_CONSTANT_COLOR_EXT:
    //case GL_ONE_MINUS_CONSTANT_COLOR:
    //case GL_ONE_MINUS_CONSTANT_COLOR_EXT:
    //case GL_CONSTANT_ALPHA:
    //case GL_CONSTANT_ALPHA_EXT:
    //case GL_ONE_MINUS_CONSTANT_ALPHA:
    //case GL_ONE_MINUS_CONSTANT_ALPHA_EXT:
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };

  state_.blending.sourceFactor = sfactor;
  state_.blending.destFactor   = dfactor;

  pRaster_->blendFunc(state_.blending.sourceFactor, state_.blending.destFactor);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::glClear(GLbitfield mask)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  if(mask &
     ~(
        GL_COLOR_BUFFER_BIT
      | GL_DEPTH_BUFFER_BIT
      //| GL_ACCUM_BUFFER_BIT
      //| GL_STENCIL_BUFFER_BIT
      )
    )
  {
    setError(GL_INVALID_VALUE);
    return;
  }

  pRaster_->clear(mask);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  state_.clClear.r = mathlib::clamp<GLclampf>(red,   0.0f, 1.0f);
  state_.clClear.g = mathlib::clamp<GLclampf>(green, 0.0f, 1.0f);
  state_.clClear.b = mathlib::clamp<GLclampf>(blue,  0.0f, 1.0f);
  state_.clClear.a = mathlib::clamp<GLclampf>(alpha, 0.0f, 1.0f);

  pRaster_->clearColor(state_.clClear.r, state_.clClear.g, state_.clClear.b, state_.clClear.a);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::glClearDepth(GLclampd depth)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  state_.depthTest.clear = mathlib::clamp<GLclampf>(depth, 0.0f, 1.0f);

  pRaster_->clearDepthf(state_.depthTest.clear);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::glColorMaterial(GLenum face, GLenum mode)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  GLenum * pColorMaterial;

  switch(face)
  {
    case GL_FRONT:
      pColorMaterial = &state_.materialFront.colorMode;
      break;
    case GL_BACK:
      pColorMaterial = &state_.materialBack.colorMode;
      break;
    case GL_FRONT_AND_BACK:
      glColorMaterial(GL_FRONT, mode);
      glColorMaterial(GL_BACK,  mode);
      return;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };

  switch(mode)
  {
    case GL_EMISSION:
    case GL_AMBIENT:
    case GL_DIFFUSE:
    case GL_SPECULAR:
    case GL_AMBIENT_AND_DIFFUSE:
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };

  *pColorMaterial = mode;
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::glColorTable(GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid * table)
{
  // NOTE: Parameters validated in rasterizer
  pRaster_->colorTable(target, internalformat, width, format, type, table);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::glCullFace(GLenum mode)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  switch(mode)
  {
    case GL_FRONT:
    case GL_BACK:
    case GL_FRONT_AND_BACK:
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };

  state_.culling.mode = mode;
  state_.culling.cullCW = (state_.culling.front == GL_CCW) == (state_.culling.mode == GL_BACK);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::glDeleteTextures(GLsizei n, const GLuint *textures)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  pRaster_->deleteTextures(n, textures);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::glDepthRangef(GLclampf zNear, GLclampf zFar)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  // FIXME: zA_ and zB_ need to be modified, this function is now useless
  state_.depthTest.rangeNear = mathlib::clamp<GLclampf>(zNear, 0.0f, 1.0f);
  state_.depthTest.rangeFar  = mathlib::clamp<GLclampf>(zFar,  0.0f, 1.0f);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::glDepthFunc(GLenum func)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

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

  state_.depthTest.function = func;

  pRaster_->depthFunc(state_.depthTest.function);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::glDepthMask(GLboolean flag)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  state_.depthTest.mask = flag;

  pRaster_->depthMask(state_.depthTest.mask);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::glDisable(GLenum cap)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  switch(cap)
  {
    case GL_ALPHA_TEST: state_.alphaTest.enabled = false; pRaster_->enableAlphaTest(false); break;
    //case GL_AUTO_NORMAL:
    case GL_BLEND: state_.blending.enabled  = false; pRaster_->enableBlending(false); break;
    //case GL_CLIP_PLANEi:
    //case GL_COLOR_ARRAY_EXT:
    //case GL_COLOR_LOGIC_OP:
    case GL_COLOR_MATERIAL: state_.lighting.materialColorEnabled = false; break;
    //case GL_COLOR_SUM_EXT:
    case GL_CULL_FACE: state_.culling.enabled = false; break;
    case GL_DEPTH_TEST: state_.depthTest.enabled = false; pRaster_->enableDepthTest(false); break;
    //case GL_DITHER:
    //case GL_EDGE_FLAG_ARRAY_EXT:
    case GL_FOG: state_.fog.enabled = false; break;
    //case GL_INDEX_ARRAY_EXT:
    case GL_LIGHT0: state_.lighting.light[0].enabled = false; break;
    case GL_LIGHT1: state_.lighting.light[1].enabled = false; break;
    case GL_LIGHT2: state_.lighting.light[2].enabled = false; break;
    case GL_LIGHT3: state_.lighting.light[3].enabled = false; break;
    case GL_LIGHT4: state_.lighting.light[4].enabled = false; break;
    case GL_LIGHT5: state_.lighting.light[5].enabled = false; break;
    case GL_LIGHT6: state_.lighting.light[6].enabled = false; break;
    case GL_LIGHT7: state_.lighting.light[7].enabled = false; break;
    case GL_LIGHTING: state_.lighting.enabled = false; break;
    //case GL_LINE_SMOOTH:
    //case GL_LINE_STIPPLE:
    //case GL_LOGIC_OP:
    //case GL_MAP1_COLOR_4:
    //case GL_MAP1_INDEX:
    //case GL_MAP1_NORMAL:
    //case GL_MAP1_TEXTURE_COORD_1:
    //case GL_MAP1_TEXTURE_COORD_2:
    //case GL_MAP1_TEXTURE_COORD_3:
    //case GL_MAP1_TEXTURE_COORD_4:
    //case GL_MAP1_VERTEX_3:
    //case GL_MAP1_VERTEX_4:
    //case GL_MAP2_COLOR_4:
    //case GL_MAP2_INDEX:
    //case GL_MAP2_NORMAL:
    //case GL_MAP2_TEXTURE_COORD_1:
    //case GL_MAP2_TEXTURE_COORD_2:
    //case GL_MAP2_TEXTURE_COORD_3:
    //case GL_MAP2_TEXTURE_COORD_4:
    //case GL_MAP2_VERTEX_3:
    //case GL_MAP2_VERTEX_4:
    //case GL_NORMAL_ARRAY_EXT:
    case GL_NORMALIZE:  state_.lighting.normalizeEnabled = false; break;
    //case GL_POLYGON_OFFSET_EXT:
    //case GL_POLYGON_OFFSET_FILL:
    //case GL_POLYGON_OFFSET_LINE:
    //case GL_POLYGON_OFFSET_POINT:
    //case GL_POINT_SMOOTH:
    //case GL_POLYGON_SMOOTH:
    //case GL_POLYGON_STIPPLE:
    //case GL_RESCALE_NORMAL:
    //case GL_RESCALE_NORMAL_EXT:
    //case GL_SCISSOR_TEST:
    //case GL_STENCIL_TEST:
    //case GL_TEXTURE_1D:
    case GL_TEXTURE_2D: state_.texturing.enabled = false; pRaster_->enableTextures(false); break;
    //case GL_TEXTURE_3D:
    //case GL_TEXTURE_3D_EXT:
    //case GL_TEXTURE_COLOR_TABLE_EXT:
    //case GL_TEXTURE_COORD_ARRAY_EXT:
    //case GL_TEXTURE_GEN_Q:
    //case GL_TEXTURE_GEN_R:
    //case GL_TEXTURE_GEN_S:
    //case GL_TEXTURE_GEN_T:
    //case GL_UPDATE_CLIP_VOLUME_HINT:
    //case GL_VERTEX_ARRAY_EXT:
    default:
      setError(GL_INVALID_ENUM);
      return;
  };
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  switch(mode)
  {
    //case GL_POINTS:
    //case GL_LINE_STRIP:
    //case GL_LINE_LOOP:
    //case GL_LINES:
    case GL_TRIANGLE_STRIP:
    case GL_TRIANGLE_FAN:
    case GL_TRIANGLES:
    case GL_QUADS:
    //case GL_QUAD_STRIP:
    case GL_POLYGON:
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };

  if(count < 0)
  {
    setError(GL_INVALID_VALUE);
    return;
  }

  _glDrawArrays(mode, first, count);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::glEnable(GLenum cap)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  switch(cap)
  {
    case GL_ALPHA_TEST: state_.alphaTest.enabled = true; pRaster_->enableAlphaTest(true); break;
    //case GL_AUTO_NORMAL:
    case GL_BLEND: state_.blending.enabled = true; pRaster_->enableBlending(true); break;
    //case GL_CLIP_PLANEi:
    //case GL_COLOR_ARRAY_EXT:
    //case GL_COLOR_LOGIC_OP:
    case GL_COLOR_MATERIAL: state_.lighting.materialColorEnabled = true; break;
    //case GL_COLOR_SUM_EXT:
    case GL_CULL_FACE: state_.culling.enabled = true; break;
    case GL_DEPTH_TEST: state_.depthTest.enabled = true; pRaster_->enableDepthTest(true); break;
    //case GL_DITHER:
    //case GL_EDGE_FLAG_ARRAY_EXT:
    case GL_FOG: state_.fog.enabled = true; break;
    //case GL_INDEX_ARRAY_EXT:
    case GL_LIGHT0: state_.lighting.light[0].enabled = true; break;
    case GL_LIGHT1: state_.lighting.light[1].enabled = true; break;
    case GL_LIGHT2: state_.lighting.light[2].enabled = true; break;
    case GL_LIGHT3: state_.lighting.light[3].enabled = true; break;
    case GL_LIGHT4: state_.lighting.light[4].enabled = true; break;
    case GL_LIGHT5: state_.lighting.light[5].enabled = true; break;
    case GL_LIGHT6: state_.lighting.light[6].enabled = true; break;
    case GL_LIGHT7: state_.lighting.light[7].enabled = true; break;
    case GL_LIGHTING: state_.lighting.enabled = true; break;
    //case GL_LINE_SMOOTH:
    //case GL_LINE_STIPPLE:
    //case GL_LOGIC_OP:
    //case GL_MAP1_COLOR_4:
    //case GL_MAP1_INDEX:
    //case GL_MAP1_NORMAL:
    //case GL_MAP1_TEXTURE_COORD_1:
    //case GL_MAP1_TEXTURE_COORD_2:
    //case GL_MAP1_TEXTURE_COORD_3:
    //case GL_MAP1_TEXTURE_COORD_4:
    //case GL_MAP1_VERTEX_3:
    //case GL_MAP1_VERTEX_4:
    //case GL_MAP2_COLOR_4:
    //case GL_MAP2_INDEX:
    //case GL_MAP2_NORMAL:
    //case GL_MAP2_TEXTURE_COORD_1:
    //case GL_MAP2_TEXTURE_COORD_2:
    //case GL_MAP2_TEXTURE_COORD_3:
    //case GL_MAP2_TEXTURE_COORD_4:
    //case GL_MAP2_VERTEX_3:
    //case GL_MAP2_VERTEX_4:
    //case GL_NORMAL_ARRAY_EXT:
    case GL_NORMALIZE: state_.lighting.normalizeEnabled = true; break;
    //case GL_POLYGON_OFFSET_EXT:
    //case GL_POLYGON_OFFSET_FILL:
    //case GL_POLYGON_OFFSET_LINE:
    //case GL_POLYGON_OFFSET_POINT:
    //case GL_POINT_SMOOTH:
    //case GL_POLYGON_SMOOTH:
    //case GL_POLYGON_STIPPLE:
    //case GL_RESCALE_NORMAL:
    //case GL_RESCALE_NORMAL_EXT:
    //case GL_SCISSOR_TEST:
    //case GL_STENCIL_TEST:
    //case GL_TEXTURE_1D:
    case GL_TEXTURE_2D: state_.texturing.enabled = true; pRaster_->enableTextures(true); break;
    //case GL_TEXTURE_3D:
    //case GL_TEXTURE_3D_EXT:
    //case GL_TEXTURE_COLOR_TABLE_EXT:
    //case GL_TEXTURE_COORD_ARRAY_EXT:
    //case GL_TEXTURE_GEN_Q:
    //case GL_TEXTURE_GEN_R:
    //case GL_TEXTURE_GEN_S:
    //case GL_TEXTURE_GEN_T:
    //case GL_UPDATE_CLIP_VOLUME_HINT:
    //case GL_VERTEX_ARRAY_EXT:
    default:
      setError(GL_INVALID_ENUM);
      return;
  };
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::glFinish(void)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  //pRaster_->finish();
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::glFlush(void)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  flush();

  pRaster_->flush();
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::glFogf(GLenum pname, GLfloat param)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  bool bUpdateScale = false;

  switch(pname)
  {
    case GL_FOG_DENSITY:
      if(param < 0)
      {
        setError(GL_INVALID_VALUE);
        return;
      }
      state_.fog.density = param;
      break;
    case GL_FOG_START:
      state_.fog.start = param;
      bUpdateScale = true;
      break;
    case GL_FOG_END:
      state_.fog.end = param;
      bUpdateScale = true;
      break;
    case GL_FOG_MODE:
      state_.fog.mode = (GLint)param;
      break;
    //case GL_FOG_INDEX:
    //case GL_FOG_COORDINATE_SOURCE_EXT:
    default:
      setError(GL_INVALID_ENUM);
      return;
  };

  if(bUpdateScale == true)
  {
    if((state_.fog.end - state_.fog.start) != 0)
      state_.fog.linear_scale = 1.0f / (state_.fog.end - state_.fog.start);
  }

  // FIXME: We assume fogging in the vertex shader, fogging in the fragment shader not possible.
  //pRaster_->fogf(pname, param);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::glFogfv(GLenum pname, const GLfloat * params)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  bool bUpdateScale = false;

  switch(pname)
  {
    case GL_FOG_DENSITY:
      state_.fog.density = params[0];
      break;
    case GL_FOG_START:
      state_.fog.start = params[0];
      bUpdateScale = true;
      break;
    case GL_FOG_END:
      state_.fog.end = params[0];
      bUpdateScale = true;
      break;
    case GL_FOG_MODE:
      state_.fog.mode = (GLint)params[0];
      break;
    case GL_FOG_COLOR:
      state_.fog.color.r = params[0];
      state_.fog.color.g = params[1];
      state_.fog.color.b = params[2];
      state_.fog.color.a = params[3];
      break;
    //case GL_FOG_INDEX:
    //case GL_FOG_COORDINATE_SOURCE_EXT:
    default:
      setError(GL_INVALID_ENUM);
      return;
  };

  if(bUpdateScale == true)
  {
    if((state_.fog.end - state_.fog.start) != 0)
      state_.fog.linear_scale = 1.0f / (state_.fog.end - state_.fog.start);
  }

  // FIXME: We assume fogging in the vertex shader, fogging in the fragment shader not possible.
  //pRaster_->fogfv(pname, params);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::glFrontFace(GLenum mode)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  switch(mode)
  {
    case GL_CW:
    case GL_CCW:
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };

  state_.culling.front = mode;
  state_.culling.cullCW = (state_.culling.front == GL_CCW) == (state_.culling.mode == GL_BACK);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::glGenTextures(GLsizei n, GLuint *textures)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  pRaster_->genTextures(n, textures);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::glGetFloatv(GLenum pname, GLfloat * params)
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

  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  switch(pname)
  {
    case GL_MATRIX_MODE:
      params[0] = matrixMode_;
      break;
    case GL_MODELVIEW_MATRIX:
      {
        GL_GET_MATRIX_COPY(pCurrentModelView_->matrix);
      }
      break;
    case GL_MAX_MODELVIEW_STACK_DEPTH:
    case GL_MODELVIEW_STACK_DEPTH:
      {
        params[0] = GL_MATRIX_MODELVIEW_STACK_SIZE;
      }
      break;
    case GL_PROJECTION_MATRIX:
      {
        GL_GET_MATRIX_COPY(pCurrentProjection_->matrix);
      }
      break;
    case GL_MAX_PROJECTION_STACK_DEPTH:
    case GL_PROJECTION_STACK_DEPTH:
      {
        params[0] = GL_MATRIX_PROJECTION_STACK_SIZE;
      }
      break;
    case GL_TEXTURE_MATRIX:
      {
        GL_GET_MATRIX_COPY(pCurrentTexture_->matrix);
      }
      break;
    case GL_MAX_TEXTURE_STACK_DEPTH:
    case GL_TEXTURE_STACK_DEPTH:
      params[0] = GL_MATRIX_TEXTURE_STACK_SIZE;
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };
}

//-----------------------------------------------------------------------------
// NOTE: No hints are used
void
CSoft3DRendererFloat::glHint(GLenum target, GLenum mode)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  GLenum * pHint;

  switch(target)
  {
    case GL_FOG_HINT:
      pHint = &state_.hints.fog;
      break;
    case GL_LINE_SMOOTH_HINT:
      pHint = &state_.hints.lineSmooth;
      break;
    case GL_PERSPECTIVE_CORRECTION_HINT:
      pHint = &state_.hints.perspectiveCorrection;
      break;
    case GL_POINT_SMOOTH_HINT:
      pHint = &state_.hints.pointSmooth;
      break;
    //case GL_POLYGON_SMOOTH_HINT:
    //case GL_CLIP_VOLUME_CLIPPING_HINT_EXT:
    default:
      setError(GL_INVALID_ENUM);
      return;
  };

  switch(mode)
  {
    case GL_FASTEST:
    case GL_NICEST:
    case GL_DONT_CARE:
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };

  *pHint = mode;
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::glLightf(GLenum light, GLenum pname, GLfloat param)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  TLight<GLfloat> * pLight = 0;
  switch(light)
  {
    case GL_LIGHT0: pLight = &state_.lighting.light[0]; break;
    case GL_LIGHT1: pLight = &state_.lighting.light[1]; break;
    case GL_LIGHT2: pLight = &state_.lighting.light[2]; break;
    case GL_LIGHT3: pLight = &state_.lighting.light[3]; break;
    case GL_LIGHT4: pLight = &state_.lighting.light[4]; break;
    case GL_LIGHT5: pLight = &state_.lighting.light[5]; break;
    case GL_LIGHT6: pLight = &state_.lighting.light[6]; break;
    case GL_LIGHT7: pLight = &state_.lighting.light[7]; break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  }

  switch(pname)
  {
    //case GL_SPOT_EXPONENT:
    //case GL_SPOT_CUTOFF:
    //case GL_CONSTANT_ATTENUATION:
    //case GL_LINEAR_ATTENUATION:
    //case GL_QUADRATIC_ATTENUATION:
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::glLightfv(GLenum light, GLenum pname, const GLfloat * params)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  TLight<GLfloat> * pLight = 0;
  switch(light)
  {
    case GL_LIGHT0: pLight = &state_.lighting.light[0]; break;
    case GL_LIGHT1: pLight = &state_.lighting.light[1]; break;
    case GL_LIGHT2: pLight = &state_.lighting.light[2]; break;
    case GL_LIGHT3: pLight = &state_.lighting.light[3]; break;
    case GL_LIGHT4: pLight = &state_.lighting.light[4]; break;
    case GL_LIGHT5: pLight = &state_.lighting.light[5]; break;
    case GL_LIGHT6: pLight = &state_.lighting.light[6]; break;
    case GL_LIGHT7: pLight = &state_.lighting.light[7]; break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  }

  TColor<GLfloat> * pColor = 0;
  switch(pname)
  {
    case GL_AMBIENT:  pColor = &pLight->ambient; break;
    case GL_DIFFUSE:  pColor = &pLight->diffuse; break;
    case GL_SPECULAR: pColor = &pLight->specular; break;
    case GL_POSITION:
      pLight->position.x = params[0];
      pLight->position.y = params[1];
      pLight->position.z = params[2];
      pLight->position.w = params[3];
      pLight->positionNormal = pLight->position.getNormalized();
      return;
    //case GL_SPOT_DIRECTION:
    //case GL_SPOT_EXPONENT:
    //case GL_SPOT_CUTOFF:
    //case GL_CONSTANT_ATTENUATION:
    //case GL_LINEAR_ATTENUATION:
    //case GL_QUADRATIC_ATTENUATION:
    default:
      setError(GL_INVALID_ENUM);
      return;
  }

  pColor->r = params[0];
  pColor->g = params[1];
  pColor->b = params[2];
  pColor->a = params[3];
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::glMaterialf(GLenum face, GLenum pname, GLfloat param)
{
  TMaterial<GLfloat> * pMaterial;

  switch(face)
  {
    case GL_FRONT:
      pMaterial = &state_.materialFront;
      break;
    case GL_BACK:
      pMaterial = &state_.materialBack;
      break;
    case GL_FRONT_AND_BACK:
      glMaterialf(GL_FRONT, pname, param);
      glMaterialf(GL_BACK,  pname, param);
      return;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };

  switch(pname)
  {
    case GL_SHININESS:
      if((param < 0) || (param > 128))
      {
        setError(GL_INVALID_VALUE);
        return;
      }
      pMaterial->shininess = param;
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  }
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::glMaterialfv(GLenum face, GLenum pname, const GLfloat * params)
{
  TMaterial<GLfloat> * pMaterial;

  switch(face)
  {
    case GL_FRONT:
      pMaterial = &state_.materialFront;
      break;
    case GL_BACK:
      pMaterial = &state_.materialBack;
      break;
    case GL_FRONT_AND_BACK:
      glMaterialfv(GL_FRONT, pname, params);
      glMaterialfv(GL_BACK,  pname, params);
      return;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };

  switch(pname)
  {
    case GL_AMBIENT:
      pMaterial->ambient.r = params[0];
      pMaterial->ambient.g = params[1];
      pMaterial->ambient.b = params[2];
      pMaterial->ambient.a = params[3];
      break;
    case GL_DIFFUSE:
      pMaterial->diffuse.r = params[0];
      pMaterial->diffuse.g = params[1];
      pMaterial->diffuse.b = params[2];
      pMaterial->diffuse.a = params[3];
      break;
    case GL_SPECULAR:
      pMaterial->specular.r = params[0];
      pMaterial->specular.g = params[1];
      pMaterial->specular.b = params[2];
      pMaterial->specular.a = params[3];
      break;
    case GL_EMISSION:
      pMaterial->emission.r = params[0];
      pMaterial->emission.g = params[1];
      pMaterial->emission.b = params[2];
      pMaterial->emission.a = params[3];
      break;
    case GL_SHININESS:
      if((params[0] < 0) || (params[0] > 128))
      {
        setError(GL_INVALID_VALUE);
        return;
      }
      pMaterial->shininess = params[0];
      break;
    case GL_AMBIENT_AND_DIFFUSE:
      pMaterial->ambient.r = params[0];
      pMaterial->ambient.g = params[1];
      pMaterial->ambient.b = params[2];
      pMaterial->ambient.a = params[3];
      pMaterial->diffuse.r = params[0];
      pMaterial->diffuse.g = params[1];
      pMaterial->diffuse.b = params[2];
      pMaterial->diffuse.a = params[3];
      break;
    //case GL_COLOR_INDEXES:
    default:
      setError(GL_INVALID_ENUM);
      return;
  }
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::glShadeModel(GLenum mode)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  switch(mode)
  {
    case GL_FLAT:
    case GL_SMOOTH:
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };

  state_.shadingModel = mode;
  state_.smoothShading = (state_.shadingModel == GL_SMOOTH);

  pRaster_->enableSmoothShading(state_.smoothShading);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::glTexEnvf(GLenum target, GLenum pname, GLfloat param)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  switch(target)
  {
    case GL_TEXTURE_ENV:
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };

  switch(pname)
  {
    case GL_TEXTURE_ENV_MODE:
      switch((GLenum)param)
      {
        case GL_MODULATE:
        case GL_DECAL:
        case GL_BLEND:
        //case GL_COMBINE_EXT:
        case GL_ADD:
        case GL_REPLACE:
        //case GL_ADD_SIGNED_EXT:
        //case GL_INTERPOLATE_EXT:
          break;
        default:
          setError(GL_INVALID_ENUM);
          return;
      };
      state_.texturing.envMode = (GLenum)param;
      break;
    //case GL_COMBINE_RGB_EXT:
    //case GL_COMBINE_ALPHA_EXT:
    //case GL_SOURCE0_RGB_EXT:
    //case GL_SOURCE1_RGB_EXT:
    //case GL_SOURCE2_RGB_EXT:
    //case GL_SOURCE0_ALPHA_EXT:
    //case GL_SOURCE1_ALPHA_EXT:
    //case GL_SOURCE2_ALPHA_EXT:
    //case GL_OPERAND0_RGB_EXT:
    //case GL_OPERAND1_RGB_EXT:
    //case GL_OPERAND2_RGB_EXT:
    //case GL_OPERAND0_ALPHA_EXT:
    //case GL_OPERAND1_ALPHA_EXT:
    //case GL_OPERAND2_ALPHA_EXT:
    //case GL_RGB_SCALE_EXT:
    //case GL_ALPHA_SCALE:
    default:
      setError(GL_INVALID_ENUM);
      return;
  }

  pRaster_->texEnvf(target, pname, param);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::glTexEnvfv(GLenum target, GLenum pname, const GLfloat * params)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  switch(target)
  {
    case GL_TEXTURE_ENV:
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };

  switch(pname)
  {
    case GL_TEXTURE_ENV_MODE:
      switch((GLenum)params[0])
      {
        case GL_MODULATE:
        case GL_DECAL:
        case GL_BLEND:
        //case GL_COMBINE_EXT:
        case GL_ADD:
        case GL_REPLACE:
        //case GL_ADD_SIGNED_EXT:
        //case GL_INTERPOLATE_EXT:
          break;
        default:
          setError(GL_INVALID_ENUM);
          return;
      };
      state_.texturing.envMode = (GLenum)params[0];
      break;
    case GL_TEXTURE_ENV_COLOR:
      state_.texturing.envColor.r = params[0];
      state_.texturing.envColor.g = params[1];
      state_.texturing.envColor.b = params[2];
      state_.texturing.envColor.a = params[3];
      break;
    //case GL_COMBINE_RGB_EXT:
    //case GL_COMBINE_ALPHA_EXT:
    //case GL_SOURCE0_RGB_EXT:
    //case GL_SOURCE1_RGB_EXT:
    //case GL_SOURCE2_RGB_EXT:
    //case GL_SOURCE0_ALPHA_EXT:
    //case GL_SOURCE1_ALPHA_EXT:
    //case GL_SOURCE2_ALPHA_EXT:
    //case GL_OPERAND0_RGB_EXT:
    //case GL_OPERAND1_RGB_EXT:
    //case GL_OPERAND2_RGB_EXT:
    //case GL_OPERAND0_ALPHA_EXT:
    //case GL_OPERAND1_ALPHA_EXT:
    //case GL_OPERAND2_ALPHA_EXT:
    //case GL_RGB_SCALE_EXT:
    //case GL_ALPHA_SCALE:
    default:
      setError(GL_INVALID_ENUM);
      return;
  };

  // FIXME: Implement glTexEnvfv in rasterizer
  //pRaster_->texEnvfv(target, pname, params);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  // NOTE: Parameters validated in rasterizer
  pRaster_->texImage2D(target, level, internalformat, width, height, border, format, type, pixels);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::glTexParameterf(GLenum target, GLenum pname, GLfloat param)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  // NOTE: Parameters validated in rasterizer
  pRaster_->texParameterf(target, pname, param);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::glTexParameterx(GLenum target, GLenum pname, GLfixed param)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  // NOTE: Parameters validated in rasterizer
  pRaster_->texParameterf(target, pname, gl_fptof(param));
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  // FIXME: Implement glTexSubImage2D
  //pRaster_->texSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  if((width < 0) || (height < 0))
  {
    setError(GL_INVALID_VALUE);
    return;
  }

  pRaster_->viewport(x, y, width, height);

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
  yA_  = -yA_;
  xB_ -= x;
  yB_ -= y;

  // Use fixed point format for xy
  xA_ *= (1<<SHIFT_XY);
  xB_ *= (1<<SHIFT_XY);
  yA_ *= (1<<SHIFT_XY);
  yB_ *= (1<<SHIFT_XY);
}

//-----------------------------------------------------------------------------
// (OpenGL 1.2) Accepted functions within glBegin/glEnd:
//  - glVertex
//  - glColor
//  - glIndex
//  - glNormal
//  - glTexCoord
//  - glEvalCoord
//  - glEvalPoint
//  - glMaterial
//  - glEdgeFlag
//  - glCallList
//  - glCallLists
//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::glBegin(GLenum mode)
{
  if(bInBeginEnd_ == true)
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
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  }

  bInBeginEnd_ = true;
  rasterMode_ = mode;

  // Initialize for default triangle
  triangle_[0] = &vertices[0];
  triangle_[1] = &vertices[1];
  triangle_[2] = &vertices[2];
  bFlipFlop_ = true;
  vertIdx_   = 0;

  pRaster_->begin(GL_TRIANGLES);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::glEnd()
{
  if(bInBeginEnd_ == false)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  bInBeginEnd_ = false;

  pRaster_->end();
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::glVertex4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
  if(bInBeginEnd_ == false)
  {
    // No error, but undefined behaviour
    //setError(GL_INVALID_OPERATION);
    return;
  }

  SVertexF v;

  // Set vertex
  v.vo.x = x;
  v.vo.y = y;
  v.vo.z = z;
  v.vo.w = w;
  // Set normal
  v.n = state_.lighting.normal;
  // Set color
  v.cl = state_.clCurrent;
  // Set texture
  v.t[0] = state_.texturing.coordCurrent[0];
  v.t[1] = state_.texturing.coordCurrent[1];
  // Vertex not processed yet
  v.processed = false;

  vertexShaderTransform(v);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
  state_.clCurrent.r = red;
  state_.clCurrent.g = green;
  state_.clCurrent.b = blue;
  state_.clCurrent.a = alpha;
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::glTexCoord4f(GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
  state_.texturing.coordCurrent[0] = s;
  state_.texturing.coordCurrent[1] = t;
  state_.texturing.coordCurrent[2] = r;
  state_.texturing.coordCurrent[3] = q;
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz)
{
  state_.lighting.normal.x = nx;
  state_.lighting.normal.y = ny;
  state_.lighting.normal.z = nz;

  if(state_.lighting.normalizeEnabled == true)
    state_.lighting.normal.normalize();
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::vertexShaderTransform(SVertexF & v)
{
  _vertexShaderTransform(v);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::vertexShaderLight(SVertexF & v)
{
  _vertexShaderLight(v);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::fragmentCull(SVertexF & v0, SVertexF & v1, SVertexF & v2)
{
  _fragmentCull(v0, v1, v2);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::fragmentClip(SVertexF & v0, SVertexF & v1, SVertexF & v2)
{
  _fragmentClip(v0, v1, v2);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::primitiveAssembly(SVertexF & v)
{
  _primitiveAssembly(v);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::_glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
  if(bBufVertexEnabled_ == false)
    return;

  GLint idxVertex  (first * bufVertex_.size);
  GLint idxColor   (first * bufColor_.size);
  GLint idxNormal  (first * bufNormal_.size);
  GLint idxTexCoord(first * bufTexCoord_.size);
  SVertexF v;

  glBegin(mode);

  // Process all vertices
  for(GLint i(0); i < count; i++)
  {
    // Vertex
    switch(bufVertex_.type)
    {
      case GL_FLOAT:
        v.vo.x = ((GLfloat *)bufVertex_.pointer)[idxVertex++];
        v.vo.y = ((GLfloat *)bufVertex_.pointer)[idxVertex++];
        v.vo.z = ((GLfloat *)bufVertex_.pointer)[idxVertex++];
        v.vo.w = 1.0f;
        break;
      case GL_FIXED:
        v.vo.x = gl_fptof(((GLfixed *)bufVertex_.pointer)[idxVertex++]);
        v.vo.y = gl_fptof(((GLfixed *)bufVertex_.pointer)[idxVertex++]);
        v.vo.z = gl_fptof(((GLfixed *)bufVertex_.pointer)[idxVertex++]);
        v.vo.w = 1.0f;
        break;
    };

    // Normal
    if(bBufNormalEnabled_ == true)
    {
      switch(bufNormal_.type)
      {
        case GL_FLOAT:
          v.n.x = ((GLfloat *)bufNormal_.pointer)[idxNormal++];
          v.n.y = ((GLfloat *)bufNormal_.pointer)[idxNormal++];
          v.n.z = ((GLfloat *)bufNormal_.pointer)[idxNormal++];
          break;
        case GL_FIXED:
          v.n.x = gl_fptof(((GLfixed *)bufNormal_.pointer)[idxNormal++]);
          v.n.y = gl_fptof(((GLfixed *)bufNormal_.pointer)[idxNormal++]);
          v.n.z = gl_fptof(((GLfixed *)bufNormal_.pointer)[idxNormal++]);
          break;
      };
    }
    else
      v.n = state_.lighting.normal;

    // Color
    if(state_.lighting.enabled == false)
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
            v.cl.r = gl_fptof(((GLfixed *)bufColor_.pointer)[idxColor++]);
            v.cl.g = gl_fptof(((GLfixed *)bufColor_.pointer)[idxColor++]);
            v.cl.b = gl_fptof(((GLfixed *)bufColor_.pointer)[idxColor++]);
            v.cl.a = gl_fptof(((GLfixed *)bufColor_.pointer)[idxColor++]);
            break;
        };
      }
      else
        v.cl = state_.clCurrent;
    }

    // Textures
    if((state_.texturing.enabled == true) && (bBufTexCoordEnabled_ == true))
    {
      switch(bufTexCoord_.type)
      {
        case GL_FLOAT:
          v.t[0] = ((GLfloat *)bufTexCoord_.pointer)[idxTexCoord++];
          v.t[1] = ((GLfloat *)bufTexCoord_.pointer)[idxTexCoord++];
          break;
        case GL_FIXED:
          v.t[0] = gl_fptof(((GLfixed *)bufTexCoord_.pointer)[idxTexCoord++]);
          v.t[1] = gl_fptof(((GLfixed *)bufTexCoord_.pointer)[idxTexCoord++]);
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
CSoft3DRendererFloat::_vertexShaderTransform(SVertexF & v)
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
CSoft3DRendererFloat::_vertexShaderLight(SVertexF & v)
{
  // --------
  // Lighting
  // --------
  if(state_.lighting.enabled == true)
  {
    TMaterial<GLfloat> * pMaterial;
    TColor<GLfloat> cAmbient (0, 0, 0, 0);
    TColor<GLfloat> cDiffuse (0, 0, 0, 0);
    TColor<GLfloat> cSpecular(0, 0, 0, 0);

    // Normalized vertex normal
    TVector3<GLfloat> vVertexNormal;
    pCurrentModelView_->transform3(v.n, vVertexNormal);

    // Normalized vector from vertex to eye
    TVector3<GLfloat> vVertexToEye = v.ve.getInverted();
    vVertexToEye.normalize();

#if 0
    // Front or backfacing vertex
    // FIXME: We need the fragment front/back, not the normal's direction
    if(vVertexNormal.dot(vVertexToEye) > 0)
    {
      pMaterial = &state_.materialFront;
    }
    else
    {
      pMaterial = &state_.materialBack;
      vVertexNormal.invert();
    }
#else
    pMaterial = &state_.materialFront;
#endif

#define INFINITE_LIGHT
    for(int iLight(0); iLight < 8; iLight++)
    {
      if(state_.lighting.light[iLight].enabled == true)
      {
#ifndef INFINITE_LIGHT
        TVector3<GLfloat> vVertexToLightNormal = TVector3<GLfloat>(state_.lighting.light[iLight].position) - TVector3<GLfloat>(v.ve);
        vVertexToLightNormal.normalize();
#endif

        // Ambient light (it's everywhere!)
        cAmbient += state_.lighting.light[iLight].ambient * pMaterial->ambient;

        // Diffuse light
#ifndef INFINITE_LIGHT
        GLfloat diffuse = vVertexToLightNormal.dot(vVertexNormal);
#else
        GLfloat diffuse = state_.lighting.light[iLight].positionNormal.dot(vVertexNormal);
#endif
        if(diffuse > 0.0f)
        {
          cDiffuse += state_.lighting.light[iLight].diffuse * pMaterial->diffuse * diffuse;
        }

        if(pMaterial->shininess >= 0.5f)
        {
          // Specular light
#ifndef INFINITE_LIGHT
          GLfloat specular = vVertexToLightNormal.getReflection(vVertexNormal).dot(vVertexToEye);
#else
          GLfloat specular = state_.lighting.light[iLight].positionNormal.getReflection(vVertexNormal).dot(vVertexToEye);
#endif
          if(specular > 0.0f)
          {
            specular = mathlib::fast_int_pow<GLfloat>(specular, (int)(pMaterial->shininess + 0.5f));
            cSpecular += state_.lighting.light[iLight].specular * pMaterial->specular * specular;
          }
        }
      }
    }

    // Color material
    if(state_.lighting.materialColorEnabled = true)
    {
      switch(pMaterial->colorMode)
      {
        case GL_EMISSION:
          break;
        case GL_AMBIENT:
          cAmbient *= v.cl;
          break;
        case GL_DIFFUSE:
          cDiffuse *= v.cl;
          break;
        case GL_SPECULAR:
          cSpecular *= v.cl;
          break;
        case GL_AMBIENT_AND_DIFFUSE:
        default:
          cAmbient *= v.cl;
          cDiffuse *= v.cl;
      };
    }

    // Final color
    v.cl = cAmbient + cDiffuse + cSpecular;
    // Clamp to 0..1
    v.cl.clamp();
  }

  // ---
  // Fog
  // ---
  if(state_.fog.enabled == true)
  {
    GLfloat f;

    switch(state_.fog.mode)
    {
      case GL_LINEAR:
        //f = (state_.fog.end - (-v.ve.z)) * state_.fog.linear_scale;
        f = (state_.fog.end + v.ve.z) * state_.fog.linear_scale;
        break;
      case GL_EXP2:
        //f = mathlib::fast_exp2(-state_.fog.density * (-v.ve.z));
        f = mathlib::fast_exp2(state_.fog.density * v.ve.z);
        break;
      case GL_EXP:
      default:
        //f = mathlib::fast_exp (-state_.fog.density * (-v.ve.z));
        f = mathlib::fast_exp (state_.fog.density * v.ve.z);
    };

    f = mathlib::clamp<GLfloat>(f, 0.0f, 1.0f);

    v.cl = mathlib_LERP(f, state_.fog.color, v.cl);
  }
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::_fragmentCull(SVertexF & v0, SVertexF & v1, SVertexF & v2)
{
  GLfloat vnz =
    (v0.vd.x - v2.vd.x) * (v1.vd.y - v2.vd.y) -
    (v0.vd.y - v2.vd.y) * (v1.vd.x - v2.vd.x);

  // Not visible
  if(vnz == 0)
    return;

  // -------
  // Culling
  // -------
  if(state_.culling.enabled == true)
  {
    // Always invisible when culling both front and back
    switch(state_.culling.mode)
    {
      case GL_BACK:
        if(vnz < 0)
          return;
        break;
      case GL_FRONT:
        if(vnz > 0)
          return;
        break;
      case GL_FRONT_AND_BACK:
        return;
        break;
    };
  }

  rasterTriangle(v0, v1, v2);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::_fragmentClip(SVertexF & v0, SVertexF & v1, SVertexF & v2)
{
  // ----------------------
  // Vertex shader lighting
  // ----------------------
  if(state_.smoothShading == true)
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
CSoft3DRendererFloat::_primitiveAssembly(SVertexF & v)
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
        fragmentClip(*triangle_[0], *triangle_[1], *triangle_[2]);
      vertIdx_++;
      if(vertIdx_ > 2)
        vertIdx_ = 0;
      break;
    }
    case GL_TRIANGLE_STRIP:
    {
      if(vertIdx_ == 2)
      {
        fragmentClip(*triangle_[0], *triangle_[1], *triangle_[2]);
        // Swap 3rd with 1st or 2nd vertex pointer
        if(bFlipFlop_ == true)
        {
          SVertexF * pTemp = triangle_[0];
          triangle_[0] = triangle_[2];
          triangle_[2] = pTemp;
        }
        else
        {
          SVertexF * pTemp = triangle_[1];
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
        fragmentClip(*triangle_[0], *triangle_[1], *triangle_[2]);
        // Swap 3rd with 2nd vertex pointer
        SVertexF * pTemp = triangle_[1];
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
        fragmentClip(*triangle_[0], *triangle_[1], *triangle_[2]);
        // Swap 3rd with 2nd vertex pointer
        SVertexF * pTemp = triangle_[1];
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
inline GLfloat                                     \
name(TVector4<GLfloat> & c, TVector4<GLfloat> & a, TVector4<GLfloat> & b) \
{                                                  \
  TVector4<GLfloat> delta;                         \
  GLfloat t, den;                                  \
                                                   \
  delta.x = b.x - a.x;                             \
  delta.y = b.y - a.y;                             \
  delta.z = b.z - a.z;                             \
  delta.w = b.w - a.w;                             \
                                                   \
  den = -(sign delta.dir) + delta.w;               \
  if(den == 0)                                     \
    t = 0;                                         \
  else                                             \
    t = (sign a.dir - a.w) / den;                  \
                                                   \
  c.dir1 = a.dir1 + t * delta.dir1;                \
  c.dir2 = a.dir2 + t * delta.dir2;                \
  c.w    = a.w    + t * delta.w;                   \
  c.dir  = sign c.w;                               \
                                                   \
  return t;                                        \
}

clip_func(clip_xmin,-,x,y,z)
clip_func(clip_xmax,+,x,y,z)
clip_func(clip_ymin,-,y,x,z)
clip_func(clip_ymax,+,y,x,z)
clip_func(clip_zmin,-,z,x,y)
clip_func(clip_zmax,+,z,x,y)

GLfloat (*f_clip_proc[6])(TVector4<GLfloat> &, TVector4<GLfloat> &, TVector4<GLfloat> &) =
{
  clip_xmin,
  clip_xmax,
  clip_ymin,
  clip_ymax,
  clip_zmin,
  clip_zmax
};

#define CLIP_FUNC(plane,c,a,b) f_clip_proc[plane](c,a,b)

//-----------------------------------------------------------------------------
void
CSoft3DRendererFloat::rasterTriangleClip(SVertexF & v0, SVertexF & v1, SVertexF & v2, uint32_t clipBit)
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
    fragmentCull(v0, v1, v2);
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
      SVertexF vNew1;  // Intersection of 1 line
      SVertexF vNew2;  // Intersection of 1 line
      GLfloat tt;

      // Rearrange vertices
      SVertexF * v[3];
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
      SVertexF vNew1;  // Intersection of 1 line
      SVertexF vNew2;  // Intersection of 1 line
      GLfloat tt;

      // Rearrange vertices
      SVertexF * v[3];
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
CSoft3DRendererFloat::interpolateVertex(SVertexF & c, SVertexF & a, SVertexF & b, GLfloat t)
{
  // Color
  if(state_.smoothShading == true)
    c.cl = mathlib_LERP(t, a.cl, b.cl);
  else
    c.cl = b.cl;

  // Texture coordinates
  if(state_.texturing.enabled == true)
  {
    c.t[0] = mathlib::lerp<GLfloat>(t, a.t[0], b.t[0]);
    c.t[1] = mathlib::lerp<GLfloat>(t, a.t[1], b.t[1]);
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
void
CSoft3DRendererFloat::rasterTriangle(SVertexF & v0, SVertexF & v1, SVertexF & v2)
{
  raster::SVertex vtx0, vtx1, vtx2;

  vtx0.x   = (int32_t)((xA_ * v0.vd.x) + xB_);
  vtx0.y   = (int32_t)((yA_ * v0.vd.y) + yB_);
  vtx0.z   = (int32_t)((zA_ * v0.vd.z) + zB_);
  vtx0.w   = v0.vd.w;
  vtx0.c.r = (int32_t)(v0.cl.r * (1<<SHIFT_COLOR));
  vtx0.c.g = (int32_t)(v0.cl.g * (1<<SHIFT_COLOR));
  vtx0.c.b = (int32_t)(v0.cl.b * (1<<SHIFT_COLOR));
  vtx0.c.a = (int32_t)(v0.cl.a * (1<<SHIFT_COLOR));
  vtx0.t.u = v0.t[0];
  vtx0.t.v = v0.t[1];

  vtx1.x   = (int32_t)((xA_ * v1.vd.x) + xB_);
  vtx1.y   = (int32_t)((yA_ * v1.vd.y) + yB_);
  vtx1.z   = (int32_t)((zA_ * v1.vd.z) + zB_);
  vtx1.w   = v1.vd.w;
  vtx1.c.r = (int32_t)(v1.cl.r * (1<<SHIFT_COLOR));
  vtx1.c.g = (int32_t)(v1.cl.g * (1<<SHIFT_COLOR));
  vtx1.c.b = (int32_t)(v1.cl.b * (1<<SHIFT_COLOR));
  vtx1.c.a = (int32_t)(v1.cl.a * (1<<SHIFT_COLOR));
  vtx1.t.u = v1.t[0];
  vtx1.t.v = v1.t[1];

  vtx2.x   = (int32_t)((xA_ * v2.vd.x) + xB_);
  vtx2.y   = (int32_t)((yA_ * v2.vd.y) + yB_);
  vtx2.z   = (int32_t)((zA_ * v2.vd.z) + zB_);
  vtx2.w   = v2.vd.w;
  vtx2.c.r = (int32_t)(v2.cl.r * (1<<SHIFT_COLOR));
  vtx2.c.g = (int32_t)(v2.cl.g * (1<<SHIFT_COLOR));
  vtx2.c.b = (int32_t)(v2.cl.b * (1<<SHIFT_COLOR));
  vtx2.c.a = (int32_t)(v2.cl.a * (1<<SHIFT_COLOR));
  vtx2.t.u = v2.t[0];
  vtx2.t.v = v2.t[1];

  pRaster_->rasterTriangle(vtx0, vtx1, vtx2);
}
