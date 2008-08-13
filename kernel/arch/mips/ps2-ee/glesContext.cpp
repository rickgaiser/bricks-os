#include "glesContext.h"
#include "bios.h"
#include "dma.h"
#include "gs.h"
#include "matrix.h"
#include "stdlib.h"


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CPS2GLESContext::CPS2GLESContext()
 : CASoftGLESFloat()
 , CAPS2Renderer()

 , pCurrentTex_(NULL)

 , ps2Shading_(SHADE_FLAT)
 , ps2Textures_(TEXTURES_OFF)
 , ps2Fog_(FOG_OFF)
 , ps2AlphaBlend_(ALPHABLEND_OFF)
 , ps2Aliasing_(ALIASING_OFF)
 , ps2DepthFunction_(ZTST_GREATER)
 , ps2DepthInvert_(true)
{
  for(GLuint idx(0); idx < MAX_TEXTURE_COUNT; idx++)
    textures_[idx].used = false;
}

//-----------------------------------------------------------------------------
CPS2GLESContext::~CPS2GLESContext()
{
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::glClear(GLbitfield mask)
{
  if(mask & GL_DEPTH_BUFFER_BIT)
  {
    packet_.gifAddPackedAD(GIF::REG::test_1, GS_TEST(0, 0, 0, 0, 0, 0, 1, ZTST_ALWAYS));
  }

  if(mask & GL_COLOR_BUFFER_BIT)
  {
    uint8_t r = (uint8_t)(clClear.r * 255);
    uint8_t g = (uint8_t)(clClear.g * 255);
    uint8_t b = (uint8_t)(clClear.b * 255);
    //uint8_t a = (uint8_t)(clClear.a * 255);

    packet_.gifAddPackedAD(GIF::REG::prim,  GS_PRIM(PRIM_SPRITE, 0, 0, 0, 0, 0, 0, 0, 0));
    packet_.gifAddPackedAD(GIF::REG::rgbaq, GS_RGBAQ(r, g, b, 0x80, 0));
    packet_.gifAddPackedAD(GIF::REG::xyz2,  GS_XYZ2((0+GS_X_BASE)<<4, (0+GS_Y_BASE)<<4, 0));
    packet_.gifAddPackedAD(GIF::REG::xyz2,  GS_XYZ2((viewportWidth+GS_X_BASE)<<4, (viewportHeight+GS_Y_BASE)<<4, 0));
  }

  if(mask & GL_DEPTH_BUFFER_BIT)
  {
    packet_.gifAddPackedAD(GIF::REG::test_1, GS_TEST(0, 0, 0, 0, 0, 0, depthTestEnabled_, ps2DepthFunction_));
  }
  bDataWaiting_ = true;
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::glDepthFunc(GLenum func)
{
  depthFunction_ = func;

  switch(func)
  {
    case GL_LESS:     ps2DepthFunction_ = ZTST_GREATER; ps2DepthInvert_ = true;  break;
//    case GL_EQUAL:    ps2DepthFunction_ = ??; break;
    case GL_LEQUAL:   ps2DepthFunction_ = ZTST_GEQUAL;  ps2DepthInvert_ = true;  break;
    case GL_GREATER:  ps2DepthFunction_ = ZTST_GREATER; ps2DepthInvert_ = false; break;
//    case GL_NOTEQUAL: ps2DepthFunction_ = ??; break;
    case GL_GEQUAL:   ps2DepthFunction_ = ZTST_GEQUAL;  ps2DepthInvert_ = false; break;
    case GL_NEVER:    ps2DepthFunction_ = ZTST_NEVER;   ps2DepthInvert_ = false; break;
    case GL_ALWAYS:
    default:          ps2DepthFunction_ = ZTST_ALWAYS;  ps2DepthInvert_ = false; break;
  }

  if(depthTestEnabled_ == true)
  {
    packet_.gifAddPackedAD(GIF::REG::test_1, GS_TEST(0, 0, 0, 0, 0, 0, depthTestEnabled_, ps2DepthFunction_));
  }
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::glDisable(GLenum cap)
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
/*
    case GL_DEPTH_TEST:
    {
      depthTestEnabled_ = false;
      // Z-Buffer
      packet_.gifAddPackedAD(GIF::REG::zbuf_1, GS_ZBUF(gs_mem_current >> 13, GRAPH_PSM_16, ZMSK_DISABLE));
      // Z-Buffer test
      packet_.gifAddPackedAD(GIF::REG::test_1, GS_TEST(0, 0, 0, 0, 0, 0, depthTestEnabled_, ps2DepthFunction_));
      break;
    }
*/
    case GL_CULL_FACE:  cullFaceEnabled_  = false; break;
    case GL_FOG:        fogEnabled_       = false; break;
    case GL_TEXTURE_2D: texturesEnabled_  = false; ps2Textures_ = TEXTURES_OFF; break;
    case GL_NORMALIZE:  normalizeEnabled_ = false; break;

    default:
      ; // Not supported
  };
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::glEnable(GLenum cap)
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
    case GL_DEPTH_TEST:
    {
      depthTestEnabled_ = true;
/*
      // 16bit
      packet_.gifAddPackedAD(GIF::REG::zbuf_1, GS_ZBUF((3*1024*1024) >> 13, GRAPH_PSM_16S, ZMSK_ENABLE));
      //ps2TexturesStart_ = gs_mem_current + (pSurface_->mode.width * pSurface_->mode.height * 2);
      zMax_ = 0xffff;

      // 24bit
      packet_.gifAddPackedAD(GIF::REG::zbuf_1, GS_ZBUF((3*1024*1024) >> 13, GRAPH_PSM_24, ZMSK_ENABLE));
      //ps2TexturesStart_ = gs_mem_current + (pSurface_->mode.width * pSurface_->mode.height * 3);
      zMax_ = 0xffffff;
*/
      // 32bit
      packet_.gifAddPackedAD(GIF::REG::zbuf_1, GS_ZBUF((3*1024*1024) >> 13, GRAPH_PSM_32, ZMSK_ENABLE));
      //ps2TexturesStart_ = gs_mem_current + (pSurface_->mode.width * pSurface_->mode.height * 4);
      zMax_ = 0xffffffff;

      // Z-Buffer test
      packet_.gifAddPackedAD(GIF::REG::test_1, GS_TEST(0, 0, 0, 0, 0, 0, depthTestEnabled_, ps2DepthFunction_));
      break;
    }
    case GL_CULL_FACE:  cullFaceEnabled_   = true; break;
    case GL_FOG:        fogEnabled_        = true; break;
    case GL_TEXTURE_2D: texturesEnabled_   = true; ps2Textures_ = TEXTURES_ON; break;
    case GL_NORMALIZE:  normalizeEnabled_  = true; break;

    default:
      ; // Not supported
  };
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::glShadeModel(GLenum mode)
{
  shadingModel_ = mode;

  switch(mode)
  {
    case GL_FLAT:   ps2Shading_ = SHADE_FLAT;    break;
    case GL_SMOOTH: ps2Shading_ = SHADE_GOURAUD; break;
  }
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::glBindTexture(GLenum target, GLuint texture)
{
  if(target == GL_TEXTURE_2D)
  {
    pCurrentTex_ = &textures_[texture];

    packet_.gifAddPackedAD(GIF::REG::tex0_1,
      GS_TEX0(
        ((uint32_t)pCurrentTex_->data)>>8,   // base pointer
        pCurrentTex_->width>>6,              // width
        0,                                   // 32bit RGBA
        getBitNr(pCurrentTex_->width),       // width
        getBitNr(pCurrentTex_->height),      // height
        1,                                   // RGBA
        TEX_DECAL,                           // just overwrite existing pixels
        0, 0, 0, 0, 0));

    packet_.gifAddPackedAD(GIF::REG::tex1_1,
      GS_TEX1(
        0, 0,
        pCurrentTex_->texMagFilter != GL_NEAREST,
        pCurrentTex_->texMinFilter != GL_NEAREST,
        0, 0, 0));
  }
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::glDeleteTextures(GLsizei n, const GLuint *textures)
{
  for(GLsizei i(0); i < n; i++)
    if(textures[i] < MAX_TEXTURE_COUNT)
      textures_[textures[i]].used = false;
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::glGenTextures(GLsizei n, GLuint *textures)
{
  for(GLsizei i(0); i < n; i++)
  {
    bool bFound(false);

    for(GLuint idx(0); idx < MAX_TEXTURE_COUNT; idx++)
    {
      if(textures_[idx].used == false)
      {
        textures_[idx].used = true;
        textures[i] = idx;
        bFound = true;
        break;
      }
    }

    if(bFound = false)
      break;
  }
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
{
/*
  if(target == GL_TEXTURE_2D)
  {
    if((pCurrentTex_ != 0) && (pCurrentTex_->used == true))
    {
      pCurrentTex_->width        = width;
      pCurrentTex_->height       = height;
      pCurrentTex_->texMinFilter = GL_LINEAR;
      pCurrentTex_->texMagFilter = GL_LINEAR;
      pCurrentTex_->texWrapS     = GL_REPEAT;
      pCurrentTex_->texWrapT     = GL_REPEAT;
      pCurrentTex_->data         = (void *)ps2TexturesStart_;

      // Copy to texture memory
      // Convert everything to cfA8B8G8R8
      switch(type)
      {
        case GL_UNSIGNED_BYTE:
        {
          ee_to_gsBitBlt(0, 0, width, height, (uint32_t)pixels, ps2TexturesStart_, width);
          break;
        }
        case GL_UNSIGNED_SHORT_5_6_5:
        {
          uint32_t * newTexture = new uint32_t[width*height];
          for(int i(0); i < (width*height); i++)
            newTexture[i] = BxColorFormat_Convert(cfB5G6R5, cfA8B8G8R8, ((uint16_t *)pixels)[i]);
          ee_to_gsBitBlt(0, 0, width, height, (uint32_t)newTexture, ps2TexturesStart_, width);
          delete newTexture;
          break;
        }
        case GL_UNSIGNED_SHORT_4_4_4_4:
        {
          uint32_t * newTexture = new uint32_t[width*height];
          for(int i(0); i < (width*height); i++)
            newTexture[i] = BxColorFormat_Convert(cfA4B4G4R4, cfA8B8G8R8, ((uint16_t *)pixels)[i]);
          ee_to_gsBitBlt(0, 0, width, height, (uint32_t)newTexture, ps2TexturesStart_, width);
          delete newTexture;
          break;
        }
        case GL_UNSIGNED_SHORT_5_5_5_1:
        {
          uint32_t * newTexture = new uint32_t[width*height];
          for(int i(0); i < (width*height); i++)
            newTexture[i] = BxColorFormat_Convert(cfA1B5G5R5, cfA8B8G8R8, ((uint16_t *)pixels)[i]);
          ee_to_gsBitBlt(0, 0, width, height, (uint32_t)newTexture, ps2TexturesStart_, width);
          delete newTexture;
          break;
        }
      };
    }
  }
*/
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::glTexParameterf(GLenum target, GLenum pname, GLfloat param)
{
  if((pCurrentTex_ != 0) && (target == GL_TEXTURE_2D))
  {
    switch(pname)
    {
      case GL_TEXTURE_MIN_FILTER: pCurrentTex_->texMinFilter = (GLint)param; break;
      case GL_TEXTURE_MAG_FILTER: pCurrentTex_->texMagFilter = (GLint)param; break;
      case GL_TEXTURE_WRAP_S:     pCurrentTex_->texWrapS     = (GLint)param; break;
      case GL_TEXTURE_WRAP_T:     pCurrentTex_->texWrapT     = (GLint)param; break;
    };
  }
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::glTexParameterx(GLenum target, GLenum pname, GLfixed param)
{
  glTexParameterf(target, pname, gl_fptof(param));
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::begin(GLenum mode)
{
  CASoftGLESFloat::begin(mode);

  switch(mode)
  {
    case GL_TRIANGLES:
      packet_.gifAddPackedAD(GIF::REG::prim, GS_PRIM(PRIM_TRI, ps2Shading_, ps2Textures_, ps2Fog_, ps2AlphaBlend_, ps2Aliasing_, TEXTURES_ST, 0, 0));
      break;
    case GL_TRIANGLE_STRIP:
      packet_.gifAddPackedAD(GIF::REG::prim, GS_PRIM(PRIM_TRI_STRIP, ps2Shading_, ps2Textures_, ps2Fog_, ps2AlphaBlend_, ps2Aliasing_, TEXTURES_ST, 0, 0));
      break;
    case GL_TRIANGLE_FAN:
      packet_.gifAddPackedAD(GIF::REG::prim, GS_PRIM(PRIM_TRI_FAN, ps2Shading_, ps2Textures_, ps2Fog_, ps2AlphaBlend_, ps2Aliasing_, TEXTURES_ST, 0, 0));
      break;
  };
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::end()
{
  CASoftGLESFloat::end();

  bDataWaiting_ = true;
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::rasterTriangle(STriangleF & tri)
{
  uint8_t alpha;

  for(int iVertex(0); iVertex < 3; iVertex++)
  {
    SVertexF & v = *tri.v[iVertex];

    if(ps2DepthInvert_ == true)
      v.sz = zMax_ - v.sz;

    // Determine alpha value (for aliasing and alpha blending)
    // Both off: solid colors
    if((ps2Aliasing_ == ALIASING_OFF) && (ps2AlphaBlend_ == ALPHABLEND_OFF))
      alpha = 255;
    // Only alpha blending: use alpha from color value
    else if((ps2Aliasing_ == ALIASING_OFF) && (ps2AlphaBlend_ == ALPHABLEND_ON))
      alpha = (uint8_t)(v.cl.a*255);
    // Only aliasing: use value 0x80
    else if((ps2Aliasing_ == ALIASING_ON)  && (ps2AlphaBlend_ == ALPHABLEND_OFF))
      alpha = 0x80;
    // Both: Can't do both at the same time!!! Prefer alpha blending then...
    else
    {
      alpha = (uint8_t)(v.cl.a*255);
      // Don't use 0x80, it's the 'magic' aliasing value
      if(alpha == 0x80)
        alpha = 0x81;
    }

    // Add to message
    if(texturesEnabled_ == true)
    {
      GLfloat tq = 1.0f / v.vc[3];
      v.t[0] *= tq;
      v.t[1] *= tq;

      packet_.gifAddPackedAD(GIF::REG::st, GS_ST(*(uint32_t *)(&v.t[0]), *(uint32_t *)(&v.t[1])));
      packet_.gifAddPackedAD(GIF::REG::rgbaq, GS_RGBAQ((uint8_t)(v.cl.r*255), (uint8_t)(v.cl.g*255), (uint8_t)(v.cl.b*255), alpha, *(uint32_t *)(&tq)));
    }
    else
    {
      packet_.gifAddPackedAD(GIF::REG::rgbaq, GS_RGBAQ((uint8_t)(v.cl.r*255), (uint8_t)(v.cl.g*255), (uint8_t)(v.cl.b*255), alpha, 0));
    }
    packet_.gifAddPackedAD(GIF::REG::xyz2, GS_XYZ2((GS_X_BASE+v.sx)<<4, (GS_Y_BASE+v.sy)<<4, v.sz));
  }
}
