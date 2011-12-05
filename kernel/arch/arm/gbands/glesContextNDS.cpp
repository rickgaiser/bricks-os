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


#include "glesContextNDS.h"
#include "../../../../gl/mathlib.h"
#include "registers.h"
#include "macros.h"
#include "string.h"
#include "math.h"


#define FIXME_TEX_ADDR (0x6840000)

#define fpgl_to_ndsRGB555(r,g,b) \
  ((((b*31) >>  6) & 0x00007c00) | \
   (((g*31) >> 11) & 0x000003e0) | \
   (((r*31) >> 16) & 0x0000001f))
#define fpgl_to_ndsRGBA5555(r,g,b,a) \
  ((((a*31)      ) & 0x001f0000) | \
   (((b*31) >>  6) & 0x00007c00) | \
   (((g*31) >> 11) & 0x000003e0) | \
   (((r*31) >> 16) & 0x0000001f))
#define fp_to_ndsRGB555(r,g,b) \
  ((((b.value*31) >>  6) & 0x00007c00) | \
   (((g.value*31) >> 11) & 0x000003e0) | \
   (((r.value*31) >> 16) & 0x0000001f))
#define fp_to_ndsRGBA5555(r,g,b,a) \
  ((((a.value*31)      ) & 0x001f0000) | \
   (((b.value*31) >>  6) & 0x00007c00) | \
   (((g.value*31) >> 11) & 0x000003e0) | \
   (((r.value*31) >> 16) & 0x0000001f))
#define fp_to_ndsNormal(x,y,z) \
  (((gl_to_ndsn(z.value) & 0x3ff) << 20) | \
   ((gl_to_ndsn(y.value) & 0x3ff) << 10) | \
   ((gl_to_ndsn(x.value) & 0x3ff)      ))
#define fp_to_ndsTexCoord(s,t) \
  (((gl_to_ndst(s.value) & 0xffff) << 16) | \
   ((gl_to_ndst(t.value) & 0xffff)      ))

// mode bits
#define NDS_RGB32_A3   1 // 32 color palette, 3 bits of alpha
#define NDS_RGB4       2 // 4 color palette
#define NDS_RGB16      3 // 16 color palette
#define NDS_RGB256     4 // 256 color palette
#define NDS_COMPRESSED 5 // compressed texture
#define NDS_RGB8_A5    6 // 8 color palette, 5 bits of alpha
#define NDS_RGBA       7 // 15 bit direct color, 1 bit of alpha
#define NDS_RGB        8 // 15 bit direct color, alpha bit autoset to 1

#define NDS_REPEAT_S    (1<<16)
#define NDS_REPEAT_T    (1<<17)
#define NDS_FLIP_S      (1<<18)
#define NDS_FLIP_T      (1<<19)

#define TEXGEN_OFF      (0<<30)  //unmodified texcoord
#define TEXGEN_TEXCOORD (1<<30)  //texcoord * texture-matrix
#define TEXGEN_NORMAL   (2<<30)  //normal * texture-matrix
#define TEXGEN_POSITION (3<<30)  //vertex * texture-matrix


bool      CNDSGLESContext::bInitialized_(false);
NDSfixed  CNDSGLESContext::fpSin_[360];
NDSfixed  CNDSGLESContext::fpCos_[360];


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CNDSTexture::CNDSTexture()
 : CTexture()
{
}

//-----------------------------------------------------------------------------
CNDSTexture::~CNDSTexture()
{
}

//-----------------------------------------------------------------------------
void
CNDSTexture::bind()
{
  uint8_t idxWidth  = getBitNr(width)  - 3;
  uint8_t idxHeight = getBitNr(height) - 3;
  format = TEXGEN_OFF | NDS_REPEAT_S | NDS_REPEAT_T | (idxWidth << 20) | (idxHeight << 23) | (((uint32_t)FIXME_TEX_ADDR >> 3) & 0xffff) | (NDS_RGBA << 26);

  GFX_TEX_FORMAT = format;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CNDSGLESContext::CNDSGLESContext()
 : I3DRenderer()
 , CAGLFloatToFixed()
 , CAGLErrorHandler()
 , CAGLBuffers()

 , matrixMode_(GL_MODELVIEW)
 , ndsCurrentMatrixId_(NDS_MODELVIEW)
 , pCurrentTex_(NULL)
 , zClearValue_(0x0000ffff)
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

  bInBeginEnd_ = false;

  // Power control
  REG_POWCNT |= POWER_LCD | POWER_2D_TOP | POWER_2D_BOTTOM | POWER_3D_CORE | POWER_3D_MATRIX;
  // Display control
  REG_DISPCNT = MODE_0 | BG0_ENABLE | ENABLE_BG03D | DISP_SOURCE_ENGINE;

  iNDSGFXControl_ = 0;//NDS_SHADING_HIGHLIGHT;
  iNDSCullMode_   = NDS_CULL_BACK;
  iNDSPolyFormat_ = NDS_POLY_ALPHA(31) | iNDSCullMode_;// | NDS_PM_MODULATION | NDS_POLY_FOG;

  GFX_CONTROL     = iNDSGFXControl_;
  GFX_POLY_FORMAT = iNDSPolyFormat_;
  GFX_CLEAR_DEPTH = 0x7fff;

  // Setup shinyness table
  uint8_t * shiny8 = (uint8_t *)ndsMatShinyness_;
  for(int i(0); i < (128 * 2); i += 2)
    shiny8[i>>1] = i;
  for(int i(0); i < (128 / 4); i++)
    GFX_SHININESS = ndsMatShinyness_[i];

  // Set the fog density table
  for(int i(0); i < 32; i++)
    GFX_FOG_TABLE[i] = (i << 2);

  if(bInitialized_ == false)
  {
    bInitialized_ = true;
    for(int i(0); i < 360; i++)
    {
      fpSin_[i] = nds_fpfromf(sin(static_cast<float>(i) * M_PI / 180.0f));
      fpCos_[i] = nds_fpfromf(cos(static_cast<float>(i) * M_PI / 180.0f));
    }
  }
}

//-----------------------------------------------------------------------------
CNDSGLESContext::~CNDSGLESContext()
{
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::setSurface(CSurface * surface)
{
  IRenderer::setSurface(surface);
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::flush()
{
  IRenderer::flush();
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glAlphaFunc(GLenum func, GLclampf ref)
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

  GLclampf clampedRef = mathlib::clamp<GLclampf>(ref, 0.0f, 1.0f);
  state_.alphaTest.func = func;
  state_.alphaTest.value = clampedRef;

  // NOTE: NDS only supports GL_GEQUAL function
  GFX_ALPHA_TEST = (uint32_t)(clampedRef * 31);
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glBindTexture(GLenum target, GLuint texture)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glBlendFunc(GLenum sfactor, GLenum dfactor)
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
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glClear(GLbitfield mask)
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
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glClearColorx(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  state_.clClear.r.value = mathlib::clamp<GLclampx>(red,   gl_fpfromi(0), gl_fpfromi(1));
  state_.clClear.g.value = mathlib::clamp<GLclampx>(green, gl_fpfromi(0), gl_fpfromi(1));
  state_.clClear.b.value = mathlib::clamp<GLclampx>(blue,  gl_fpfromi(0), gl_fpfromi(1));
  state_.clClear.a.value = mathlib::clamp<GLclampx>(alpha, gl_fpfromi(0), gl_fpfromi(1));

  GFX_CLEAR_COLOR = fpgl_to_ndsRGBA5555(state_.clClear.r.value,
                                        state_.clClear.g.value,
                                        state_.clClear.b.value,
                                        state_.clClear.a.value);
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glClearDepthx(GLclampx depth)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  state_.depthTest.clear.value = mathlib::clamp<GLclampx>(depth, gl_fpfromi(0), gl_fpfromi(1));

  // Convert to 16bit depth value
  if(state_.depthTest.clear.value >= 0x00008000)
    zClearValue_ = state_.depthTest.clear.value - 1;
  else
    zClearValue_ = state_.depthTest.clear.value;

  GFX_CLEAR_DEPTH = zClearValue_;
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glColorMaterial(GLenum face, GLenum mode)
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
CNDSGLESContext::glColorTable(GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid * table)
{
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glCullFace(GLenum mode)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  switch(mode)
  {
    case GL_FRONT:          iNDSCullMode_ = NDS_CULL_FRONT;          break;
    case GL_BACK:           iNDSCullMode_ = NDS_CULL_BACK;           break;
    case GL_FRONT_AND_BACK: iNDSCullMode_ = NDS_CULL_BACK_AND_FRONT; break;
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };

  state_.culling.mode = mode;
  state_.culling.cullCW = (state_.culling.front == GL_CCW) == (state_.culling.mode == GL_BACK);

  if(state_.culling.enabled == true)
  {
    iNDSPolyFormat_ = (iNDSPolyFormat_ & ~NDS_CULL_MASK) | iNDSCullMode_;
    GFX_POLY_FORMAT = iNDSPolyFormat_;
  }
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glDeleteTextures(GLsizei n, const GLuint *textures)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glDepthRangex(GLclampx zNear, GLclampx zFar)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  // FIXME: zA_ and zB_ need to be modified, this function is now useless
  state_.depthTest.rangeNear.value = mathlib::clamp<GLclampx>(zNear, gl_fpfromi(0), gl_fpfromi(1));
  state_.depthTest.rangeFar.value  = mathlib::clamp<GLclampx>(zFar,  gl_fpfromi(0), gl_fpfromi(1));
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glDepthFunc(GLenum func)
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
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glDepthMask(GLboolean flag)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  state_.depthTest.mask = flag;
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glDisable(GLenum cap)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  switch(cap)
  {
  case GL_ALPHA_TEST:
    state_.alphaTest.enabled = false;
    iNDSGFXControl_ &= ~NDS_ALPHA_TEST;
    GFX_CONTROL = iNDSGFXControl_;
    break;
  //case GL_AUTO_NORMAL:
  case GL_BLEND:
    state_.blending.enabled = false;
    iNDSGFXControl_ &= ~NDS_BLEND;
    GFX_CONTROL = iNDSGFXControl_;
    break;
  //case GL_CLIP_PLANEi:
  //case GL_COLOR_ARRAY_EXT:
  //case GL_COLOR_LOGIC_OP:
  case GL_COLOR_MATERIAL: state_.lighting.materialColorEnabled = false; break;
  //case GL_COLOR_SUM_EXT:
  case GL_CULL_FACE:
    state_.culling.enabled = false;
    iNDSPolyFormat_ &= ~NDS_CULL_MASK;
    GFX_POLY_FORMAT = iNDSPolyFormat_;
    break;
  case GL_DEPTH_TEST: state_.depthTest.enabled = false; break;
  //case GL_DITHER:
  //case GL_EDGE_FLAG_ARRAY_EXT:
  case GL_FOG:
    state_.fog.enabled = false;
    iNDSGFXControl_ &= ~NDS_FOG;
    GFX_CONTROL = iNDSGFXControl_;
    break;
  //case GL_INDEX_ARRAY_EXT:
  case GL_LIGHT0:     state_.lighting.light[0].enabled = false; updateLights(); break;
  case GL_LIGHT1:     state_.lighting.light[1].enabled = false; updateLights(); break;
  case GL_LIGHT2:     state_.lighting.light[2].enabled = false; updateLights(); break;
  case GL_LIGHT3:     state_.lighting.light[3].enabled = false; updateLights(); break;
  //case GL_LIGHT4:     state_.lighting.light[4].enabled = false; updateLights(); break;
  //case GL_LIGHT5:     state_.lighting.light[5].enabled = false; updateLights(); break;
  //case GL_LIGHT6:     state_.lighting.light[6].enabled = false; updateLights(); break;
  //case GL_LIGHT7:     state_.lighting.light[7].enabled = false; updateLights(); break;
  case GL_LIGHTING:   state_.lighting.enabled  = false;         updateLights(); break;
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
  case GL_TEXTURE_2D:
    state_.texturing.enabled = false;
    iNDSGFXControl_ &= ~NDS_TEXTURE_2D;
    GFX_CONTROL = iNDSGFXControl_;
    break;
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
CNDSGLESContext::glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glEnable(GLenum cap)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  switch(cap)
  {
    case GL_ALPHA_TEST:
      state_.alphaTest.enabled = true;
      // NDS only supports GL_GEQUAL function, so only enable alpha testing if it's usefull
      if((state_.alphaTest.func == GL_GEQUAL) || (state_.alphaTest.func == GL_GREATER))
      {
        iNDSGFXControl_ |= NDS_ALPHA_TEST;
        GFX_CONTROL = iNDSGFXControl_;
      }
      break;
    //case GL_AUTO_NORMAL:
    case GL_BLEND:
      state_.blending.enabled = true;
      iNDSGFXControl_ |= NDS_BLEND;
      GFX_CONTROL = iNDSGFXControl_;
      break;
    //case GL_CLIP_PLANEi:
    //case GL_COLOR_ARRAY_EXT:
    //case GL_COLOR_LOGIC_OP:
    case GL_COLOR_MATERIAL: state_.lighting.materialColorEnabled = true; break;
    //case GL_COLOR_SUM_EXT:
    case GL_CULL_FACE:
      state_.culling.enabled = true;
      iNDSPolyFormat_ = (iNDSPolyFormat_ & ~NDS_CULL_MASK) | iNDSCullMode_;
      GFX_POLY_FORMAT = iNDSPolyFormat_;
      break;
    case GL_DEPTH_TEST: state_.depthTest.enabled = true; break;
    //case GL_DITHER:
    //case GL_EDGE_FLAG_ARRAY_EXT:
    case GL_FOG:
      state_.fog.enabled = true;
      iNDSGFXControl_ |= NDS_FOG;
      GFX_CONTROL = iNDSGFXControl_;
      break;
    //case GL_INDEX_ARRAY_EXT:
    case GL_LIGHT0:     state_.lighting.light[0].enabled = true; updateLights(); break;
    case GL_LIGHT1:     state_.lighting.light[1].enabled = true; updateLights(); break;
    case GL_LIGHT2:     state_.lighting.light[2].enabled = true; updateLights(); break;
    case GL_LIGHT3:     state_.lighting.light[3].enabled = true; updateLights(); break;
    //case GL_LIGHT4:     state_.lighting.light[4].enabled = true; updateLights(); break;
    //case GL_LIGHT5:     state_.lighting.light[5].enabled = true; updateLights(); break;
    //case GL_LIGHT6:     state_.lighting.light[6].enabled = true; updateLights(); break;
    //case GL_LIGHT7:     state_.lighting.light[7].enabled = true; updateLights(); break;
    case GL_LIGHTING:   state_.lighting.enabled  = true;         updateLights(); break;
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
    case GL_NORMALIZE:  state_.lighting.normalizeEnabled = true; break;
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
    case GL_TEXTURE_2D:
      state_.texturing.enabled = true;
      iNDSGFXControl_ |= NDS_TEXTURE_2D;
      GFX_CONTROL = iNDSGFXControl_;
      break;
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
CNDSGLESContext::glFinish()
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glFlush()
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  flush();

  GFX_FLUSH = 2;
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glFogx(GLenum pname, GLfixed param)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

/*
  updateFog();
*/
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glFogxv(GLenum pname, const GLfixed *params)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

/*
  switch(pname)
  {
    case GL_FOG_COLOR:
      GFX_FOG_COLOR = fp_to_ndsRGBA5555(fogColor_.r, fogColor_.g, fogColor_.b, fogColor_.a);
      break;
  };
*/
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glFrontFace(GLenum mode)
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

  // FIXME: NDS Front Face support
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glGenTextures(GLsizei n, GLuint *textures)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glGetFloatv(GLenum pname, GLfloat * params)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glHint(GLenum target, GLenum mode)
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
CNDSGLESContext::glLightx(GLenum light, GLenum pname, GLfixed param)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

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

  updateLights();
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glLightxv(GLenum light, GLenum pname, const GLfixed * params)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

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

  pColor->r.value = params[0];
  pColor->g.value = params[1];
  pColor->b.value = params[2];
  pColor->a.value = params[3];

  updateLights();
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glMaterialx(GLenum face, GLenum pname, GLfixed param)
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
      if((param < 0) || (param > 128))
      {
        setError(GL_INVALID_VALUE);
        return;
      }
      pMaterial->shininess.value = param;
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  }

  // NDS uses shininess table, initialized in constructor
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glMaterialxv(GLenum face, GLenum pname, const GLfixed *params)
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
      if((params[0] < 0) || (params[0] > 128))
      {
        setError(GL_INVALID_VALUE);
        return;
      }
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
    //case GL_COLOR_INDEXES:
    default:
      setError(GL_INVALID_ENUM);
      return;
  }

  // NDS Only lights the front face
  if(face != GL_BACK)
  {
    switch(pname)
    {
      case GL_AMBIENT:
        ndsMatColorAmbient_ = fpgl_to_ndsRGB555(params[0], params[1], params[2]);
        GFX_DIFFUSE_AMBIENT = (ndsMatColorDiffuse_  << 16) | ndsMatColorAmbient_;
        break;
      case GL_DIFFUSE:
        ndsMatColorDiffuse_ = fpgl_to_ndsRGB555(params[0], params[1], params[2]);
        GFX_DIFFUSE_AMBIENT = (ndsMatColorDiffuse_  << 16) | ndsMatColorAmbient_;
        break;
      case GL_SPECULAR:
        ndsMatColorSpecular_  = 0x8000 | fpgl_to_ndsRGB555(params[0], params[1], params[2]);
        GFX_SPECULAR_EMISSION = (ndsMatColorSpecular_ << 16) | ndsMatColorEmission_;
        break;
      case GL_EMISSION:
        ndsMatColorEmission_  = fpgl_to_ndsRGB555(params[0], params[1], params[2]);
        GFX_SPECULAR_EMISSION = (ndsMatColorSpecular_ << 16) | ndsMatColorEmission_;
        break;
      case GL_SHININESS:
        // NDS uses shininess table, initialized in constructor
        break;
      case GL_AMBIENT_AND_DIFFUSE:
        ndsMatColorAmbient_ = ndsMatColorDiffuse_ = fpgl_to_ndsRGB555(params[0], params[1], params[2]);
        GFX_DIFFUSE_AMBIENT = (ndsMatColorDiffuse_  << 16) | ndsMatColorAmbient_;
        break;
    };
  }
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glShadeModel(GLenum mode)
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
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glTexEnvf(GLenum target, GLenum pname, GLfloat param)
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
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glTexEnvfv(GLenum target, GLenum pname, const GLfloat *params)
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
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

/*
  // Unlock texture memory
  REG_VRAM_C_CR = VRAM_ENABLE | VRAM_TYPE_LCD;

  memcpy((void *)FIXME_TEX_ADDR, pCurrentTex_->data, width * height * (colorFormatOps[cfA1B5G5R5].bitsPerPixel >> 3));

  // Restore texture memory
  REG_VRAM_C_CR = VRAM_ENABLE | VRAM_TYPE_TEXTURE;
*/
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glTexParameterf(GLenum target, GLenum pname, GLfloat param)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  if(target != GL_TEXTURE_2D)
  {
    setError(GL_INVALID_ENUM);
    return;
  }

  if(pCurrentTex_ != 0)
  {
    switch(pname)
    {
      case GL_TEXTURE_MIN_FILTER:
      {
        GLint value = (GLint)param;
        switch(value)
        {
          case GL_NEAREST: break;
          case GL_LINEAR:  break;
          default:
            setError(GL_INVALID_ENUM);
            return;
        };
        pCurrentTex_->minFilter = value;
        break;
      }
      case GL_TEXTURE_MAG_FILTER:
      {
        GLint value = (GLint)param;
        switch(value)
        {
          case GL_NEAREST:                break;
          case GL_LINEAR:                 break;
          case GL_NEAREST_MIPMAP_NEAREST: break;
          case GL_LINEAR_MIPMAP_NEAREST:  break;
          case GL_NEAREST_MIPMAP_LINEAR:  break;
          case GL_LINEAR_MIPMAP_LINEAR:   break;
          default:
            setError(GL_INVALID_ENUM);
            return;
        };
        pCurrentTex_->magFilter = value;
        break;
      }
      case GL_TEXTURE_WRAP_S:
        switch((GLint)param)
        {
          case GL_CLAMP:  pCurrentTex_->format &= ~NDS_REPEAT_S; break;
          case GL_REPEAT: pCurrentTex_->format |=  NDS_REPEAT_S; break;
          default:
            setError(GL_INVALID_ENUM);
            return;
        };
        break;
      case GL_TEXTURE_WRAP_T:
        switch((GLint)param)
        {
          case GL_CLAMP:  pCurrentTex_->format &= ~NDS_REPEAT_T; break;
          case GL_REPEAT: pCurrentTex_->format |=  NDS_REPEAT_T; break;
          default:
            setError(GL_INVALID_ENUM);
            return;
        };
        break;
      default:
        setError(GL_INVALID_ENUM);
        return;
    };
  }
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glTexParameterx(GLenum target, GLenum pname, GLfixed param)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels)
{
  if(bInBeginEnd_ == true)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
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

  GFX_VIEWPORT = x + (y << 8) + ((width-1) << 16) + ((height-1) << 24);
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glBegin(GLenum mode)
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
    case GL_QUAD_STRIP:
    case GL_POLYGON:
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  }

  bInBeginEnd_ = true;
  rasterMode_ = mode;

  switch(mode)
  {
    case GL_TRIANGLES:
      GFX_BEGIN = NDS_TRIANGLES;
      break;
    case GL_TRIANGLE_STRIP:
      GFX_BEGIN = NDS_TRIANGLE_STRIP;
      break;
    case GL_QUADS:
      GFX_BEGIN = NDS_QUADS;
      break;
    case GL_QUAD_STRIP:
      GFX_BEGIN = NDS_QUAD_STRIP;
      break;
    // Simulate polygon and triangle fan using triangles
    case GL_POLYGON:
    case GL_TRIANGLE_FAN:
      GFX_BEGIN = NDS_TRIANGLES;
      break;
  };
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glEnd()
{
  if(bInBeginEnd_ == false)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  bInBeginEnd_ = false;

  GFX_END = 0;
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glVertex4x(GLfixed x, GLfixed y, GLfixed z, GLfixed w)
{
  if(bInBeginEnd_ == false)
  {
    // No error, but undefined behaviour
    //setError(GL_INVALID_OPERATION);
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
  v.c = state_.clCurrent;
  // Set texture
  v.t[0] = state_.texturing.coordCurrent[0];
  v.t[1] = state_.texturing.coordCurrent[1];
  // Vertex not processed yet
  v.processed = false;

  vertexShaderTransform(v);
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glColor4x(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha)
{
  state_.clCurrent.r.value = red;
  state_.clCurrent.g.value = green;
  state_.clCurrent.b.value = blue;
  state_.clCurrent.a.value = alpha;
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glTexCoord4x(GLfixed s, GLfixed t, GLfixed r, GLfixed q)
{
  state_.texturing.coordCurrent[0].value = s;
  state_.texturing.coordCurrent[1].value = t;
  state_.texturing.coordCurrent[2].value = r;
  state_.texturing.coordCurrent[3].value = q;
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glNormal3x(GLfixed nx, GLfixed ny, GLfixed nz)
{
  state_.lighting.normal.x.value = nx;
  state_.lighting.normal.y.value = ny;
  state_.lighting.normal.z.value = nz;

  if(state_.lighting.normalizeEnabled == true)
    state_.lighting.normal.normalize();
}

//---------------------------------------------------------------------------
void
CNDSGLESContext::glFrustumx(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
  MATRIX_MULT4x4 = gl_to_ndsv(gl_fpdiv((zNear << 1), (right - left)));
  MATRIX_MULT4x4 = nds_fpfromi(0);
  MATRIX_MULT4x4 = gl_to_ndsv(gl_fpdiv((right + left), (right - left)));
  MATRIX_MULT4x4 = nds_fpfromi(0);

  MATRIX_MULT4x4 = nds_fpfromi(0);
  MATRIX_MULT4x4 = gl_to_ndsv(gl_fpdiv((zNear << 1), (top - bottom)));
  MATRIX_MULT4x4 = gl_to_ndsv(gl_fpdiv((top + bottom), (top - bottom)));
  MATRIX_MULT4x4 = nds_fpfromi(0);

  MATRIX_MULT4x4 = nds_fpfromi(0);
  MATRIX_MULT4x4 = nds_fpfromi(0);
  MATRIX_MULT4x4 = gl_to_ndsv(-gl_fpdiv((zFar + zNear), (zFar - zNear)));
  MATRIX_MULT4x4 = nds_fpfromi(-1);

  MATRIX_MULT4x4 = nds_fpfromi(0);
  MATRIX_MULT4x4 = nds_fpfromi(0);
  MATRIX_MULT4x4 = gl_to_ndsv(-gl_fpdiv((gl_fpmul(zFar, zNear) << 1), (zFar - zNear)));
  MATRIX_MULT4x4 = nds_fpfromi(0);

  //MATRIX_STORE = ndsCurrentMatrixId_;
  //MATRIX_STORE = 0;
}

//---------------------------------------------------------------------------
void
CNDSGLESContext::glLoadIdentity(void)
{
  MATRIX_IDENTITY = 0;
}

//---------------------------------------------------------------------------
void
CNDSGLESContext::glLoadMatrixx(const GLfixed *m)
{
  MATRIX_LOAD4x4 = gl_to_ndsv(m[0]);
  MATRIX_LOAD4x4 = gl_to_ndsv(m[1]);
  MATRIX_LOAD4x4 = gl_to_ndsv(m[2]);
  MATRIX_LOAD4x4 = gl_to_ndsv(m[3]);

  MATRIX_LOAD4x4 = gl_to_ndsv(m[4]);
  MATRIX_LOAD4x4 = gl_to_ndsv(m[5]);
  MATRIX_LOAD4x4 = gl_to_ndsv(m[6]);
  MATRIX_LOAD4x4 = gl_to_ndsv(m[7]);

  MATRIX_LOAD4x4 = gl_to_ndsv(m[8]);
  MATRIX_LOAD4x4 = gl_to_ndsv(m[9]);
  MATRIX_LOAD4x4 = gl_to_ndsv(m[10]);
  MATRIX_LOAD4x4 = gl_to_ndsv(m[11]);

  MATRIX_LOAD4x4 = gl_to_ndsv(m[12]);
  MATRIX_LOAD4x4 = gl_to_ndsv(m[13]);
  MATRIX_LOAD4x4 = gl_to_ndsv(m[14]);
  MATRIX_LOAD4x4 = gl_to_ndsv(m[15]);
}

//---------------------------------------------------------------------------
void
CNDSGLESContext::glMatrixMode(GLenum mode)
{
  matrixMode_ = mode;

  switch(mode)
  {
    case GL_PROJECTION: ndsCurrentMatrixId_ = NDS_PROJECTION; break;
    //case GL_???:        ndsCurrentMatrixId_ = NDS_POSITION;   break;
    case GL_MODELVIEW:  ndsCurrentMatrixId_ = NDS_MODELVIEW;  break;
    case GL_TEXTURE:    ndsCurrentMatrixId_ = NDS_TEXTURE;    break;
  };

  MATRIX_CONTROL = ndsCurrentMatrixId_;
}

//---------------------------------------------------------------------------
void
CNDSGLESContext::glMultMatrixx(const GLfixed *m)
{
  MATRIX_MULT4x4 = gl_to_ndsv(m[0]);
  MATRIX_MULT4x4 = gl_to_ndsv(m[1]);
  MATRIX_MULT4x4 = gl_to_ndsv(m[2]);
  MATRIX_MULT4x4 = gl_to_ndsv(m[3]);

  MATRIX_MULT4x4 = gl_to_ndsv(m[4]);
  MATRIX_MULT4x4 = gl_to_ndsv(m[5]);
  MATRIX_MULT4x4 = gl_to_ndsv(m[6]);
  MATRIX_MULT4x4 = gl_to_ndsv(m[7]);

  MATRIX_MULT4x4 = gl_to_ndsv(m[8]);
  MATRIX_MULT4x4 = gl_to_ndsv(m[9]);
  MATRIX_MULT4x4 = gl_to_ndsv(m[10]);
  MATRIX_MULT4x4 = gl_to_ndsv(m[11]);

  MATRIX_MULT4x4 = gl_to_ndsv(m[12]);
  MATRIX_MULT4x4 = gl_to_ndsv(m[13]);
  MATRIX_MULT4x4 = gl_to_ndsv(m[14]);
  MATRIX_MULT4x4 = gl_to_ndsv(m[15]);
}

//---------------------------------------------------------------------------
void
CNDSGLESContext::glOrthox(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
  MATRIX_MULT4x4 = gl_to_ndsv(gl_fpdiv(gl_fpfromi(2), (right - left)));
  MATRIX_MULT4x4 = nds_fpfromi(0);
  MATRIX_MULT4x4 = nds_fpfromi(0);
  MATRIX_MULT4x4 = gl_to_ndsv(-gl_fpdiv((right + left), (right - left)));

  MATRIX_MULT4x4 = nds_fpfromi(0);
  MATRIX_MULT4x4 = gl_to_ndsv(gl_fpdiv(gl_fpfromi(2), (top - bottom)));
  MATRIX_MULT4x4 = nds_fpfromi(0);
  MATRIX_MULT4x4 = gl_to_ndsv(-gl_fpdiv((top + bottom), (top - bottom)));

  MATRIX_MULT4x4 = nds_fpfromi(0);
  MATRIX_MULT4x4 = nds_fpfromi(0);
  MATRIX_MULT4x4 = gl_to_ndsv(-gl_fpdiv((zFar + zNear), (zFar - zNear)));
  MATRIX_MULT4x4 = nds_fpfromi(0);

  MATRIX_MULT4x4 = nds_fpfromi(0);
  MATRIX_MULT4x4 = nds_fpfromi(0);
  MATRIX_MULT4x4 = gl_to_ndsv(-gl_fpdiv((gl_fpmul(zFar, zNear) << 1), (zFar - zNear)));
  MATRIX_MULT4x4 = nds_fpfromi(1);

  //MATRIX_STORE = ndsCurrentMatrixId_;
  //MATRIX_STORE = 0;
}

//---------------------------------------------------------------------------
void
CNDSGLESContext::glPopMatrix(void)
{
  MATRIX_POP = 1;
}

//---------------------------------------------------------------------------
void
CNDSGLESContext::glPushMatrix(void)
{
  MATRIX_PUSH = 0;
}

//---------------------------------------------------------------------------
void
CNDSGLESContext::glRotatex(GLfixed angle, GLfixed x, GLfixed y, GLfixed z)
{
  // Normalize the angle
  angle = angle - gl_fpfromi((gl_fpfloor(angle) / 360) * 360);
  if(angle < 0)
    angle += gl_fpfromi(360);

  // Get sin and cos from lookup table
  NDSfixed iSin = fpSin_[gl_fpfloor(angle) % 360];
  NDSfixed iCos = fpCos_[gl_fpfloor(angle) % 360];

  // Convert from gl fixed to nds fixed
  iSin = -iSin; // Why???
  x = gl_to_ndsv(x);
  y = gl_to_ndsv(y);
  z = gl_to_ndsv(z);

  long flags(((z != 0) << 2) | ((y != 0) << 1) | (x != 0));
  switch(flags)
  {
    case 0x00:
    {
      break;
    }
    case 0x01:
    {
      // X Rotation only
      MATRIX_MULT3x3 = nds_fpfromi(1);
      MATRIX_MULT3x3 = nds_fpfromi(0);
      MATRIX_MULT3x3 = nds_fpfromi(0);

      MATRIX_MULT3x3 = nds_fpfromi(0);
      MATRIX_MULT3x3 = iCos;
      MATRIX_MULT3x3 = -iSin;

      MATRIX_MULT3x3 = nds_fpfromi(0);
      MATRIX_MULT3x3 = iSin;
      MATRIX_MULT3x3 = iCos;
      break;
    }
    case 0x02:
    {
      // Y Rotation only
      MATRIX_MULT3x3 = iCos;
      MATRIX_MULT3x3 = nds_fpfromi(0);
      MATRIX_MULT3x3 = iSin;

      MATRIX_MULT3x3 = nds_fpfromi(0);
      MATRIX_MULT3x3 = nds_fpfromi(1);
      MATRIX_MULT3x3 = nds_fpfromi(0);

      MATRIX_MULT3x3 = -iSin;
      MATRIX_MULT3x3 = nds_fpfromi(0);
      MATRIX_MULT3x3 = iCos;
      break;
    }
    case 0x04:
    {
      // Z Rotation only
      MATRIX_MULT3x3 = iCos;
      MATRIX_MULT3x3 = iSin;
      MATRIX_MULT3x3 = nds_fpfromi(0);

      MATRIX_MULT3x3 = -iSin;
      MATRIX_MULT3x3 = iCos;
      MATRIX_MULT3x3 = nds_fpfromi(0);

      MATRIX_MULT3x3 = nds_fpfromi(0);
      MATRIX_MULT3x3 = nds_fpfromi(0);
      MATRIX_MULT3x3 = nds_fpfromi(1);
      break;
    }
    default:
    {
      // Mixed Rotation
      NDSfixed iMinCos = nds_fpfromi(1) - iCos;
      MATRIX_MULT3x3 = nds_fpmul(nds_fpmul(x, x), iMinCos) + iCos;
      MATRIX_MULT3x3 = nds_fpmul(nds_fpmul(x, y), iMinCos) - nds_fpmul(z, iSin);
      MATRIX_MULT3x3 = nds_fpmul(nds_fpmul(x, z), iMinCos) + nds_fpmul(y, iSin);

      MATRIX_MULT3x3 = nds_fpmul(nds_fpmul(y, x), iMinCos) + nds_fpmul(z, iSin);
      MATRIX_MULT3x3 = nds_fpmul(nds_fpmul(y, y), iMinCos) + iCos;
      MATRIX_MULT3x3 = nds_fpmul(nds_fpmul(y, z), iMinCos) - nds_fpmul(x, iSin);

      MATRIX_MULT3x3 = nds_fpmul(nds_fpmul(z, x), iMinCos) - nds_fpmul(y, iSin);
      MATRIX_MULT3x3 = nds_fpmul(nds_fpmul(z, y), iMinCos) + nds_fpmul(x, iSin);
      MATRIX_MULT3x3 = nds_fpmul(nds_fpmul(z, z), iMinCos) + iCos;
    }
  };
}

//---------------------------------------------------------------------------
void
CNDSGLESContext::glScalex(GLfixed x, GLfixed y, GLfixed z)
{
  MATRIX_SCALE = gl_to_ndsv(x);
  MATRIX_SCALE = gl_to_ndsv(y);
  MATRIX_SCALE = gl_to_ndsv(z);
}

//---------------------------------------------------------------------------
void
CNDSGLESContext::glTranslatex(GLfixed x, GLfixed y, GLfixed z)
{
  MATRIX_TRANSLATE = gl_to_ndsv(x);
  MATRIX_TRANSLATE = gl_to_ndsv(y);
  MATRIX_TRANSLATE = gl_to_ndsv(z);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void
CNDSGLESContext::vertexShaderTransform(SVertexFx & v)
{
  // Color or normal
  if(state_.lighting.enabled == true)
    GFX_NORMAL = fp_to_ndsNormal(v.n.x,  v.n.y,  v.n.z);
  else
    GFX_COLOR  = fp_to_ndsRGB555(v.c.r, v.c.g, v.c.b);

  // Textures
  if(state_.texturing.enabled == true)
    GFX_TEX_COORD = fp_to_ndsTexCoord(v.t[0], v.t[1]);

  // Vertex
  GFX_VERTEX16 = ((gl_to_ndsv(v.vo[1].value) << 16) & 0xffff0000) | (gl_to_ndsv(v.vo[0].value) & 0xffff);
  GFX_VERTEX16 = gl_to_ndsv(v.vo[2].value) & 0xffff;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void
CNDSGLESContext::updateLights()
{
  iNDSPolyFormat_ &= ~(NDS_LIGHT0 | NDS_LIGHT1 | NDS_LIGHT2 | NDS_LIGHT3);
  if(state_.lighting.enabled == true)
  {
    // Set color to white
    GFX_COLOR = 0x7fff;

    for(int iLight = 0; iLight < 4; iLight++)
    {
      if(state_.lighting.light[iLight].enabled == true)
      {
        iNDSPolyFormat_ |= 1 << iLight;

        // We need to set the light vector with an identity mapped model-view matrix
        // If we don't, the light vector will be rotated
        MATRIX_CONTROL = NDS_MODELVIEW;
        glPushMatrix();
        glLoadIdentity();

        GFX_LIGHT_VECTOR = (iLight << 30) | fp_to_ndsNormal(state_.lighting.light[iLight].positionNormal.x, state_.lighting.light[iLight].positionNormal.y, state_.lighting.light[iLight].positionNormal.z);
        GFX_LIGHT_COLOR  = (iLight << 30) | fp_to_ndsRGB555(state_.lighting.light[iLight].ambient.r,        state_.lighting.light[iLight].ambient.g,        state_.lighting.light[iLight].ambient.b);

        glPopMatrix();
        MATRIX_CONTROL = ndsCurrentMatrixId_;
      }
    }
  }
  GFX_POLY_FORMAT = iNDSPolyFormat_;
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::updateFog()
{
//  GFX_FOG_OFFSET = fogStart_ --> 0..0x7fff;

//  fogDensity_
//  fogEnd_
}
