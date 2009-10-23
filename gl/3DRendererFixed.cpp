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

#include "3DRendererFixed.h"
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
  CFixed inv_w; \
  inv_w.value = gl_fpinverse((v).vc.w.value); \
  (v).vd.x = (v).vc.x * inv_w; \
  (v).vd.y = (v).vc.y * inv_w; \
  (v).vd.z = (v).vc.z * inv_w; \
  (v).vd.w = inv_w

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


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CSoft3DRendererFixed::CSoft3DRendererFixed()
 : CAGLFloatToFixed()
 , CAGLErrorHandler()
 , CAGLBuffers()
 , CAGLMatrixFixed()
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
  state_.texturing.envColor = TColor<CFixed>(0, 0, 0, 0);

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
  state_.lighting.normal = TVector3<CFixed>(0.0f, 0.0f, 1.0f);

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
  state_.fog.color = TColor<CFixed>(0.0f, 0.0f, 0.0f, 0.0f);


  float zsize = (1 << (DEPTH_Z + SHIFT_Z));
  zA_ = ((zsize - 0.5) / 2.0);
  zB_ = ((zsize - 0.5) / 2.0) + (1 << (SHIFT_Z-1));

  beginValid_ = false;

  clipPlane_[0] = TVector4<CFixed>( 1.0,  0.0,  0.0,  1.0); // left
  clipPlane_[1] = TVector4<CFixed>(-1.0,  0.0,  0.0,  1.0); // right
  clipPlane_[2] = TVector4<CFixed>( 0.0,  1.0,  0.0,  1.0); // bottom
  clipPlane_[3] = TVector4<CFixed>( 0.0, -1.0,  0.0,  1.0); // top
  clipPlane_[4] = TVector4<CFixed>( 0.0,  0.0,  1.0,  1.0); // near
  clipPlane_[5] = TVector4<CFixed>( 0.0,  0.0, -1.0,  1.0); // far
}

//-----------------------------------------------------------------------------
CSoft3DRendererFixed::~CSoft3DRendererFixed()
{
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFixed::setRaster(raster::IRasterizer * rast)
{
  pRaster_ = rast;

  if(renderSurface != NULL)
    pRaster_->setSurface(renderSurface);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFixed::setSurface(CSurface * surface)
{
  IRenderer::setSurface(surface);

  if(pRaster_ != NULL)
    pRaster_->setSurface(renderSurface);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFixed::glAlphaFunc(GLenum func, GLclampf ref)
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

  state_.alphaTest.func = func;
  state_.alphaTest.value = mathlib::clamp<GLclampf>(ref, 0.0f, 1.0f);

  pRaster_->alphaFunc(state_.alphaTest.func, state_.alphaTest.value);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFixed::glClearColorx(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha)
{
  state_.clClear.r.value = mathlib::clamp<GLclampx>(red,   gl_fpfromi(0), gl_fpfromi(1));
  state_.clClear.g.value = mathlib::clamp<GLclampx>(green, gl_fpfromi(0), gl_fpfromi(1));
  state_.clClear.b.value = mathlib::clamp<GLclampx>(blue,  gl_fpfromi(0), gl_fpfromi(1));
  state_.clClear.a.value = mathlib::clamp<GLclampx>(alpha, gl_fpfromi(0), gl_fpfromi(1));

  pRaster_->clearColor(state_.clClear.r, state_.clClear.g, state_.clClear.b, state_.clClear.a);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFixed::glClearDepthx(GLclampx depth)
{
  state_.depthTest.clear.value = mathlib::clamp<GLclampx>(depth, gl_fpfromi(0), gl_fpfromi(1));

  pRaster_->clearDepthf(state_.depthTest.clear);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFixed::glColorMaterial(GLenum face, GLenum mode)
{
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
CSoft3DRendererFixed::glCullFace(GLenum mode)
{
  state_.culling.mode = mode;

  state_.culling.cullCW = (state_.culling.front == GL_CCW) == (state_.culling.mode == GL_BACK);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFixed::glDepthRangex(GLclampx zNear, GLclampx zFar)
{
  // FIXME: zA_ and zB_ need to be modified, this function is now useless

  state_.depthTest.rangeNear.value = mathlib::clamp<GLclampx>(zNear, gl_fpfromi(0), gl_fpfromi(1));
  state_.depthTest.rangeFar.value  = mathlib::clamp<GLclampx>(zFar,  gl_fpfromi(0), gl_fpfromi(1));
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFixed::glDepthFunc(GLenum func)
{
  state_.depthTest.function = func;

  pRaster_->depthFunc(state_.depthTest.function);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFixed::glDepthMask(GLboolean flag)
{
  state_.depthTest.mask = flag;

  //pRaster_->depthMask(state_.depthTest.mask);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFixed::glDisable(GLenum cap)
{
  switch(cap)
  {
    case GL_ALPHA_TEST: state_.alphaTest.enabled = false; pRaster_->enableAlphaTest(false); break;
    case GL_BLEND:      state_.blending.enabled  = false; pRaster_->enableBlending(false); break;
    case GL_COLOR_MATERIAL: state_.lighting.materialColorEnabled = false; break;
    case GL_LIGHTING:   state_.lighting.enabled  = false; break;
    case GL_LIGHT0:     state_.lighting.light[0].enabled = false; break;
    case GL_LIGHT1:     state_.lighting.light[1].enabled = false; break;
    case GL_LIGHT2:     state_.lighting.light[2].enabled = false; break;
    case GL_LIGHT3:     state_.lighting.light[3].enabled = false; break;
    case GL_LIGHT4:     state_.lighting.light[4].enabled = false; break;
    case GL_LIGHT5:     state_.lighting.light[5].enabled = false; break;
    case GL_LIGHT6:     state_.lighting.light[6].enabled = false; break;
    case GL_LIGHT7:     state_.lighting.light[7].enabled = false; break;
    case GL_DEPTH_TEST: state_.depthTest.enabled = false; pRaster_->enableDepthTest(false); break;
    case GL_CULL_FACE:  state_.culling.enabled   = false; break;
    case GL_FOG:        state_.fog.enabled       = false; break;
    case GL_TEXTURE_2D: state_.texturing.enabled = false; pRaster_->enableTextures(false); break;
    case GL_NORMALIZE:  state_.lighting.normalizeEnabled = false; break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFixed::glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
  _glDrawArrays(mode, first, count);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFixed::glEnable(GLenum cap)
{
  switch(cap)
  {
    case GL_ALPHA_TEST: state_.alphaTest.enabled = true; pRaster_->enableAlphaTest(true); break;
    case GL_BLEND:      state_.blending.enabled  = true; pRaster_->enableBlending(true); break;
    case GL_COLOR_MATERIAL: state_.lighting.materialColorEnabled = true; break;
    case GL_LIGHTING:   state_.lighting.enabled  = true; break;
    case GL_LIGHT0:     state_.lighting.light[0].enabled = true; break;
    case GL_LIGHT1:     state_.lighting.light[1].enabled = true; break;
    case GL_LIGHT2:     state_.lighting.light[2].enabled = true; break;
    case GL_LIGHT3:     state_.lighting.light[3].enabled = true; break;
    case GL_LIGHT4:     state_.lighting.light[4].enabled = true; break;
    case GL_LIGHT5:     state_.lighting.light[5].enabled = true; break;
    case GL_LIGHT6:     state_.lighting.light[6].enabled = true; break;
    case GL_LIGHT7:     state_.lighting.light[7].enabled = true; break;
    case GL_DEPTH_TEST: state_.depthTest.enabled = true; pRaster_->enableDepthTest(true); break;
    case GL_CULL_FACE:  state_.culling.enabled   = true; break;
    case GL_FOG:        state_.fog.enabled       = true; break;
    case GL_TEXTURE_2D: state_.texturing.enabled = true; pRaster_->enableTextures(true); break;
    case GL_NORMALIZE:  state_.lighting.normalizeEnabled = true; break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFixed::glFinish(void)
{
  //pRaster_->finish();
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFixed::glFlush(void)
{
  flush();

  pRaster_->flush();
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFixed::glFogx(GLenum pname, GLfixed param)
{
  bool bUpdateScale = false;

  switch(pname)
  {
    case GL_FOG_DENSITY: state_.fog.density.value = param; break;
    case GL_FOG_START:   state_.fog.start.value   = param; bUpdateScale = true; break;
    case GL_FOG_END:     state_.fog.end.value     = param; bUpdateScale = true; break;
    case GL_FOG_MODE:    state_.fog.mode          = param; break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };

  if(bUpdateScale == true)
  {
    if((state_.fog.end - state_.fog.start) != 0)
      state_.fog.linear_scale.value = gl_fpinverse((state_.fog.end - state_.fog.start).value);
  }
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFixed::glFogxv(GLenum pname, const GLfixed * params)
{
  bool bUpdateScale = false;

  switch(pname)
  {
    case GL_FOG_DENSITY: state_.fog.density.value = params[0]; break;
    case GL_FOG_START:   state_.fog.start.value   = params[0]; bUpdateScale = true; break;
    case GL_FOG_END:     state_.fog.end.value     = params[0]; bUpdateScale = true; break;
    case GL_FOG_MODE:    state_.fog.mode          = params[0]; break;
    case GL_FOG_COLOR:
      state_.fog.color.r.value = params[0];
      state_.fog.color.g.value = params[1];
      state_.fog.color.b.value = params[2];
      state_.fog.color.a.value = params[3];
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };

  if(bUpdateScale == true)
  {
    if((state_.fog.end - state_.fog.start) != 0)
      state_.fog.linear_scale.value = gl_fpinverse((state_.fog.end - state_.fog.start).value);
  }
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFixed::glFrontFace(GLenum mode)
{
  state_.culling.front = mode;

  state_.culling.cullCW = (state_.culling.front == GL_CCW) == (state_.culling.mode == GL_BACK);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFixed::glHint(GLenum target, GLenum mode)
{
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
    default:
      setError(GL_INVALID_ENUM);
      return;
  };

  switch(mode)
  {
    case GL_FASTEST:
      break;
    case GL_NICEST:
      break;
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
CSoft3DRendererFixed::glLightx(GLenum light, GLenum pname, GLfixed param)
{
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFixed::glLightxv(GLenum light, GLenum pname, const GLfixed * params)
{
  TLight<CFixed> * pLight = 0;
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

  TColor<CFixed> * pColor = 0;
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
      pLight->positionNormal = pLight->position.getNormalized();
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
CSoft3DRendererFixed::glMaterialx(GLenum face, GLenum pname, GLfixed param)
{
  TMaterial<CFixed> * pMaterial;

  switch(face)
  {
    case GL_FRONT:
      pMaterial = &state_.materialFront;
      break;
    case GL_BACK:
      pMaterial = &state_.materialBack;
      break;
    case GL_FRONT_AND_BACK:
      glMaterialx(GL_FRONT, pname, param);
      glMaterialx(GL_BACK,  pname, param);
      return;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };

  switch(pname)
  {
    case GL_SHININESS:
      pMaterial->shininess.value = param;
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  }
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFixed::glMaterialxv(GLenum face, GLenum pname, const GLfixed *params)
{
  TMaterial<CFixed> * pMaterial;

  switch(face)
  {
    case GL_FRONT:
      pMaterial = &state_.materialFront;
      break;
    case GL_BACK:
      pMaterial = &state_.materialBack;
      break;
    case GL_FRONT_AND_BACK:
      glMaterialxv(GL_FRONT, pname, params);
      glMaterialxv(GL_BACK,  pname, params);
      return;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };

  switch(pname)
  {
    case GL_AMBIENT:
      pMaterial->ambient.r.value = params[0];
      pMaterial->ambient.g.value = params[1];
      pMaterial->ambient.b.value = params[2];
      pMaterial->ambient.a.value = params[3];
      break;
    case GL_DIFFUSE:
      pMaterial->diffuse.r.value = params[0];
      pMaterial->diffuse.g.value = params[1];
      pMaterial->diffuse.b.value = params[2];
      pMaterial->diffuse.a.value = params[3];
      break;
    case GL_SPECULAR:
      pMaterial->specular.r.value = params[0];
      pMaterial->specular.g.value = params[1];
      pMaterial->specular.b.value = params[2];
      pMaterial->specular.a.value = params[3];
      break;
    case GL_EMISSION:
      pMaterial->emission.r.value = params[0];
      pMaterial->emission.g.value = params[1];
      pMaterial->emission.b.value = params[2];
      pMaterial->emission.a.value = params[3];
      break;
    case GL_SHININESS:
      pMaterial->shininess.value = params[0];
      break;
    case GL_AMBIENT_AND_DIFFUSE:
      pMaterial->ambient.r.value = params[0];
      pMaterial->ambient.g.value = params[1];
      pMaterial->ambient.b.value = params[2];
      pMaterial->ambient.a.value = params[3];
      pMaterial->diffuse.r.value = params[0];
      pMaterial->diffuse.g.value = params[1];
      pMaterial->diffuse.b.value = params[2];
      pMaterial->diffuse.a.value = params[3];
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  }
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFixed::glShadeModel(GLenum mode)
{
  state_.shadingModel = mode;
  state_.smoothShading = (state_.shadingModel == GL_SMOOTH);

  pRaster_->enableSmoothShading(state_.smoothShading);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFixed::glBegin(GLenum mode)
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

  pRaster_->begin(GL_TRIANGLES);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFixed::glEnd()
{
  if(beginValid_ == false)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  beginValid_ = false;

  pRaster_->end();
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFixed::glVertex4x(GLfixed x, GLfixed y, GLfixed z, GLfixed w)
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
CSoft3DRendererFixed::glColor4x(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha)
{
  state_.clCurrent.r.value = red;
  state_.clCurrent.g.value = green;
  state_.clCurrent.b.value = blue;
  state_.clCurrent.a.value = alpha;
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFixed::glTexCoord4x(GLfixed s, GLfixed t, GLfixed r, GLfixed q)
{
  state_.texturing.coordCurrent[0].value = s;
  state_.texturing.coordCurrent[1].value = t;
  state_.texturing.coordCurrent[2].value = r;
  state_.texturing.coordCurrent[3].value = q;
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFixed::glNormal3x(GLfixed nx, GLfixed ny, GLfixed nz)
{
  state_.lighting.normal.x.value = nx;
  state_.lighting.normal.y.value = ny;
  state_.lighting.normal.z.value = nz;

  if(state_.lighting.normalizeEnabled == true)
    state_.lighting.normal.normalize();
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFixed::glGetFloatv(GLenum pname, GLfloat * params)
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
CSoft3DRendererFixed::glBlendFunc(GLenum sfactor, GLenum dfactor)
{
  state_.blending.sourceFactor = sfactor;
  state_.blending.destFactor   = dfactor;

  pRaster_->blendFunc(state_.blending.sourceFactor, state_.blending.destFactor);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFixed::glTexEnvf(GLenum target, GLenum pname, GLfloat param)
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
      state_.texturing.envMode = (GLenum)param;
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFixed::glTexEnvfv(GLenum target, GLenum pname, const GLfloat * params)
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
          state_.texturing.envMode = (GLenum)params[0];
          break;
        default:
          setError(GL_INVALID_ENUM);
          return;
      };
      break;
    case GL_TEXTURE_ENV_COLOR:
      state_.texturing.envColor.r = params[0];
      state_.texturing.envColor.g = params[1];
      state_.texturing.envColor.b = params[2];
      state_.texturing.envColor.a = params[3];
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFixed::vertexShaderTransform(SVertexFx & v)
{
  _vertexShaderTransform(v);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFixed::vertexShaderLight(SVertexFx & v)
{
  _vertexShaderLight(v);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFixed::fragmentCull(SVertexFx & v0, SVertexFx & v1, SVertexFx & v2)
{
  _fragmentCull(v0, v1, v2);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFixed::fragmentClip(SVertexFx & v0, SVertexFx & v1, SVertexFx & v2)
{
  _fragmentClip(v0, v1, v2);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFixed::primitiveAssembly(SVertexFx & v)
{
  _primitiveAssembly(v);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFixed::_glDrawArrays(GLenum mode, GLint first, GLsizei count)
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
        v.vo.x = ((GLfloat *)bufVertex_.pointer)[idxVertex++];
        v.vo.y = ((GLfloat *)bufVertex_.pointer)[idxVertex++];
        v.vo.z = ((GLfloat *)bufVertex_.pointer)[idxVertex++];
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
          v.n.x = ((GLfloat *)bufNormal_.pointer)[idxNormal++];
          v.n.y = ((GLfloat *)bufNormal_.pointer)[idxNormal++];
          v.n.z = ((GLfloat *)bufNormal_.pointer)[idxNormal++];
          break;
        case GL_FIXED:
          v.n.x.value = ((GLfixed *)bufNormal_.pointer)[idxNormal++];
          v.n.y.value = ((GLfixed *)bufNormal_.pointer)[idxNormal++];
          v.n.z.value = ((GLfixed *)bufNormal_.pointer)[idxNormal++];
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
            v.cl.r.value = ((GLfixed *)bufColor_.pointer)[idxColor++];
            v.cl.g.value = ((GLfixed *)bufColor_.pointer)[idxColor++];
            v.cl.b.value = ((GLfixed *)bufColor_.pointer)[idxColor++];
            v.cl.a.value = ((GLfixed *)bufColor_.pointer)[idxColor++];
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
          v.t[0].value = ((GLfixed *)bufTexCoord_.pointer)[idxTexCoord++];
          v.t[1].value = ((GLfixed *)bufTexCoord_.pointer)[idxTexCoord++];
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
CSoft3DRendererFixed::_vertexShaderTransform(SVertexFx & v)
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
CSoft3DRendererFixed::_vertexShaderLight(SVertexFx & v)
{
  // --------
  // Lighting
  // --------
  if(state_.lighting.enabled == true)
  {
    TMaterial<CFixed> * pMaterial;
    TColor<CFixed> cAmbient (0, 0, 0, 0);
    TColor<CFixed> cDiffuse (0, 0, 0, 0);
    TColor<CFixed> cSpecular(0, 0, 0, 0);

    // Normalized vertex normal
    TVector3<CFixed> vVertexNormal;
    pCurrentModelView_->transform3(v.n, vVertexNormal);

    // Normalized vector from vertex to eye
    TVector3<CFixed> vVertexToEye = v.ve.getInverted();
    vVertexToEye.normalize();

#if 0
    // Front or backfacing vertex
    // FIXME: We need the fragment front/back, not the normal's direction
    if(vVertexNormal.dotProduct(vVertexToEye) > 0)
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
        TVector3<CFixed> vVertexToLightNormal = TVector3<CFixed>(state_.lighting.light[iLight].position) - TVector3<CFixed>(v.ve);
        vVertexToLightNormal.normalize();
#endif

        // Ambient light (it's everywhere!)
        cAmbient += state_.lighting.light[iLight].ambient * pMaterial->ambient;

        // Diffuse light
#ifndef INFINITE_LIGHT
        CFixed diffuse = vVertexToLightNormal.dotProduct(vVertexNormal);
#else
        CFixed diffuse = state_.lighting.light[iLight].positionNormal.dotProduct(vVertexNormal);
#endif
        if(diffuse > 0.0f)
        {
          cDiffuse += state_.lighting.light[iLight].diffuse * pMaterial->diffuse * diffuse;
        }

        if(pMaterial->shininess >= 0.5f)
        {
          // Specular light
#ifndef INFINITE_LIGHT
          CFixed specular = vVertexToLightNormal.getReflection(vVertexNormal).dotProduct(vVertexToEye);
#else
          CFixed specular = state_.lighting.light[iLight].positionNormal.getReflection(vVertexNormal).dotProduct(vVertexToEye);
#endif
          if(specular > 0.0f)
          {
            specular = mathlib::fast_int_pow<CFixed>(specular, (int)(pMaterial->shininess + 0.5f));
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
    CFixed f;

    switch(state_.fog.mode)
    {
      case GL_LINEAR:
        //f = (state_.fog.end - (0-v.ve.z)) * state_.fog.linear_scale;
        f = (state_.fog.end + v.ve.z) * state_.fog.linear_scale;
        break;
      case GL_EXP2:
        //f = mathlib::fast_exp2(0-state_.fog.density * (0-v.ve.z));
        f = mathlib::fast_exp2(state_.fog.density * v.ve.z);
        break;
      case GL_EXP:
      default:
        //f = mathlib::fast_exp (0-state_.fog.density * (0-v.ve.z));
        f = mathlib::fast_exp (state_.fog.density * v.ve.z);
    };

    f = mathlib::clamp<CFixed>(f, 0, 1);

    v.cl = mathlib_LERP(f, state_.fog.color, v.cl);
  }
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFixed::_fragmentCull(SVertexFx & v0, SVertexFx & v1, SVertexFx & v2)
{
  CFixed vnz =
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
CSoft3DRendererFixed::_fragmentClip(SVertexFx & v0, SVertexFx & v1, SVertexFx & v2)
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
CSoft3DRendererFixed::_primitiveAssembly(SVertexFx & v)
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
        fragmentClip(*triangle_[0], *triangle_[1], *triangle_[2]);
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
        fragmentClip(*triangle_[0], *triangle_[1], *triangle_[2]);
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
  TVector4<CFixed> delta;                          \
  CFixed t, den;                                   \
                                                   \
  delta.x = b.x - a.x;                             \
  delta.y = b.y - a.y;                             \
  delta.z = b.z - a.z;                             \
  delta.w = b.w - a.w;                             \
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

CFixed (*fx_clip_proc[6])(TVector4<CFixed> &, TVector4<CFixed> &, TVector4<CFixed> &) =
{
  clip_xmin,
  clip_xmax,
  clip_ymin,
  clip_ymax,
  clip_zmin,
  clip_zmax
};

#define CLIP_FUNC(plane,c,a,b) fx_clip_proc[plane](c,a,b)

//-----------------------------------------------------------------------------
void
CSoft3DRendererFixed::rasterTriangleClip(SVertexFx & v0, SVertexFx & v1, SVertexFx & v2, uint32_t clipBit)
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
CSoft3DRendererFixed::interpolateVertex(SVertexFx & c, SVertexFx & a, SVertexFx & b, CFixed t)
{
  // Color
  if(state_.smoothShading == true)
    c.cl = mathlib_LERP(t, a.cl, b.cl);
  else
    c.cl = b.cl;

  // Texture coordinates
  if(state_.texturing.enabled == true)
  {
    c.t[0] = mathlib::lerp<CFixed>(t, a.t[0], b.t[0]);
    c.t[1] = mathlib::lerp<CFixed>(t, a.t[1], b.t[1]);
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
CSoft3DRendererFixed::rasterTriangle(SVertexFx & v0, SVertexFx & v1, SVertexFx & v2)
{
  raster::SVertex vtx0, vtx1, vtx2;

  vtx0.x   = ((xA_ * v0.vd.x) + xB_).value >> (16 - SHIFT_XY);
  vtx0.y   = ((yA_ * v0.vd.y) + yB_).value >> (16 - SHIFT_XY);
  vtx0.z   = 0;//((zA_ * v0.vd.z) + zB_).value >> (16 - SHIFT_XY);
  vtx0.w   = v0.vd.w;
  vtx0.c.r = v0.cl.r.value >> (16 - SHIFT_COLOR);
  vtx0.c.g = v0.cl.g.value >> (16 - SHIFT_COLOR);
  vtx0.c.b = v0.cl.b.value >> (16 - SHIFT_COLOR);
  vtx0.c.a = v0.cl.a.value >> (16 - SHIFT_COLOR);
  //vtx0.t.u = v0.t[0];
  //vtx0.t.v = v0.t[1];

  vtx1.x   = ((xA_ * v1.vd.x) + xB_).value >> (16 - SHIFT_XY);
  vtx1.y   = ((yA_ * v1.vd.y) + yB_).value >> (16 - SHIFT_XY);
  vtx1.z   = 0;//((zA_ * v1.vd.z) + zB_).value >> (16 - SHIFT_XY);
  vtx1.w   = v1.vd.w;
  vtx1.c.r = v1.cl.r.value >> (16 - SHIFT_COLOR);
  vtx1.c.g = v1.cl.g.value >> (16 - SHIFT_COLOR);
  vtx1.c.b = v1.cl.b.value >> (16 - SHIFT_COLOR);
  vtx1.c.a = v1.cl.a.value >> (16 - SHIFT_COLOR);
  //vtx1.t.u = v1.t[0];
  //vtx1.t.v = v1.t[1];

  vtx2.x   = ((xA_ * v2.vd.x) + xB_).value >> (16 - SHIFT_XY);
  vtx2.y   = ((yA_ * v2.vd.y) + yB_).value >> (16 - SHIFT_XY);
  vtx2.z   = 0;//((zA_ * v2.vd.z) + zB_).value >> (16 - SHIFT_XY);
  vtx2.w   = v2.vd.w;
  vtx2.c.r = v2.cl.r.value >> (16 - SHIFT_COLOR);
  vtx2.c.g = v2.cl.g.value >> (16 - SHIFT_COLOR);
  vtx2.c.b = v2.cl.b.value >> (16 - SHIFT_COLOR);
  vtx2.c.a = v2.cl.a.value >> (16 - SHIFT_COLOR);
  //vtx2.t.u = v2.t[0];
  //vtx2.t.v = v2.t[1];

  pRaster_->rasterTriangle(vtx0, vtx1, vtx2);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFixed::glBindTexture(GLenum target, GLuint texture)
{
  pRaster_->bindTexture(target, texture);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFixed::glDeleteTextures(GLsizei n, const GLuint *textures)
{
  pRaster_->deleteTextures(n, textures);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFixed::glGenTextures(GLsizei n, GLuint *textures)
{
  pRaster_->genTextures(n, textures);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFixed::glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
{
  pRaster_->texImage2D(target, level, internalformat, width, height, border, format, type, pixels);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFixed::glTexParameterf(GLenum target, GLenum pname, GLfloat param)
{
  pRaster_->texParameterf(target, pname, param);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFixed::glTexParameterx(GLenum target, GLenum pname, GLfixed param)
{
  pRaster_->texParameterf(target, pname, gl_fptof(param));
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFixed::glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels)
{
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFixed::glClear(GLbitfield mask)
{
  pRaster_->clear(mask);
}

//-----------------------------------------------------------------------------
void
CSoft3DRendererFixed::glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
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
  yA_  = 0-yA_;
  xB_ -= x;
  yB_ -= y;

  // Use fixed point format for xy
//  xA_ *= (1<<SHIFT_XY);
//  xB_ *= (1<<SHIFT_XY);
//  yA_ *= (1<<SHIFT_XY);
//  yB_ *= (1<<SHIFT_XY);
}
