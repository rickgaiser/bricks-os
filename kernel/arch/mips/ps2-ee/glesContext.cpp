#include "glesContext.h"
#include "bios.h"
#include "dma.h"
#include "gs.h"
#include "matrix.h"
#include "stdlib.h"


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CPS2GLESContext::CPS2GLESContext(CPS2VideoDevice & device)
 : CASoftGLESFloat()
 , CAPS2Renderer()

 , device_(device)
 , pCurrentTex_(NULL)

 , ps2ZPSM_(GRAPH_PSM_16S)
 , ps2ZBufferAddr_(0)
 , ps2Shading_(SHADE_FLAT)
 , ps2Textures_(TEXTURES_OFF)
 , ps2Fog_(FOG_OFF)
 , ps2AlphaBlend_(ALPHABLEND_OFF)
 , ps2Aliasing_(ALIASING_OFF)
 , ps2DepthFunction_(ZTST_GREATER)
 , ps2DepthInvert_(true)
{
  // Select MAX z-value
  switch(ps2ZPSM_)
  {
    case GRAPH_PSM_16:
    case GRAPH_PSM_16S: zMax_ = 0x0000ffff; break;
    case GRAPH_PSM_24:  zMax_ = 0x00ffffff; break;
    case GRAPH_PSM_32:  zMax_ = 0xffffffff; break;
  };

  // Initialize textures
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

    packet_.gifAddPackedAD(GIF::REG::prim,  GIF::REG::PRIM(PRIM_SPRITE, 0, 0, 0, 0, 0, 0, 0, 0));
    packet_.gifAddPackedAD(GIF::REG::rgbaq, GIF::REG::RGBAQ(r, g, b, 0x80, 0));
    packet_.gifAddPackedAD(GIF::REG::xyz2,  GIF::REG::XYZ2((0+GS_X_BASE)<<4, (0+GS_Y_BASE)<<4, 0));
    packet_.gifAddPackedAD(GIF::REG::xyz2,  GIF::REG::XYZ2((viewportWidth+GS_X_BASE)<<4, (viewportHeight+GS_Y_BASE)<<4, 0));
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
    case GL_DEPTH_TEST:
      depthTestEnabled_ = false;
      zbuffer(false);
      break;
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
      depthTestEnabled_ = true;
      zbuffer(true);
      break;
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
  if(target != GL_TEXTURE_2D)
  {
    setError(GL_INVALID_ENUM);
    return;
  }
  if(texture >= MAX_TEXTURE_COUNT)
  {
    setError(GL_INVALID_VALUE);
    return;
  }

  pCurrentTex_ = &textures_[texture];

  if(pCurrentTex_->level_[0].initialized == true)
  {
    packet_.gifAddPackedAD(GIF::REG::tex0_1,
      GS_TEX0(
        ((uint32_t)pCurrentTex_->level_[0].data)>>8,   // base pointer
        pCurrentTex_->level_[0].width>>6,              // width
        pCurrentTex_->psm_,                            // pixel store mode
        pCurrentTex_->level_[0].widthBitNr,            // width
        pCurrentTex_->level_[0].heightBitNr,           // height
        pCurrentTex_->rgba_,                           // 0=RGB, 1=RGBA
        TEX_MODULATE,                                  // Texture Function
        0, 0, 0, 0, 0));                               // CLUT (currently not used)

    packet_.gifAddPackedAD(GIF::REG::tex1_1,
      GS_TEX1(
        0,
        pCurrentTex_->maxLevel_,                 // Max MipMap level
        pCurrentTex_->texMagFilter == GL_LINEAR, // GL_LINEAR or GL_NEAREST
        4,//pCurrentTex_->texMinFilter == GL_LINEAR, // GL_LINEAR or GL_NEAREST
        0,
        0,                                       // LOD: L parameter (Shift LOD left 0..3)
        -20));                                   // LOD: K parameter (Add to LOD -128..127)

    if(pCurrentTex_->maxLevel_ > 0)
    {
      packet_.gifAddPackedAD(GIF::REG::miptbp1_1,
        GS_MIPTBP(
          ((uint32_t)pCurrentTex_->level_[1].data) >> 8,
          pCurrentTex_->level_[1].width>>6,
          ((uint32_t)pCurrentTex_->level_[2].data) >> 8,
          pCurrentTex_->level_[2].width>>6,
          ((uint32_t)pCurrentTex_->level_[3].data) >> 8,
          pCurrentTex_->level_[3].width>>6));
    }

    if(pCurrentTex_->maxLevel_ > 3)
    {
      packet_.gifAddPackedAD(GIF::REG::miptbp2_1,
        GS_MIPTBP(
          ((uint32_t)pCurrentTex_->level_[4].data) >> 8,
          pCurrentTex_->level_[4].width>>6,
          ((uint32_t)pCurrentTex_->level_[5].data) >> 8,
          pCurrentTex_->level_[5].width>>6,
          ((uint32_t)pCurrentTex_->level_[6].data) >> 8,
          pCurrentTex_->level_[6].width>>6));
    }
  }
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::glDeleteTextures(GLsizei n, const GLuint *textures)
{
  if(n < 0)
  {
    setError(GL_INVALID_VALUE);
    return;
  }

  for(GLsizei i(0); i < n; i++)
    if(textures[i] < MAX_TEXTURE_COUNT)
      textures_[textures[i]].used = false;
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::glGenTextures(GLsizei n, GLuint *textures)
{
  if(n < 0)
  {
    setError(GL_INVALID_VALUE);
    return;
  }

  for(GLsizei idxNr(0); idxNr < n; idxNr++)
  {
    bool bFound(false);

    for(GLuint idxTex(0); idxTex < MAX_TEXTURE_COUNT; idxTex++)
    {
      if(textures_[idxTex].used == false)
      {
        textures_[idxTex].used = true;
        textures_[idxTex].initialized = false;

        textures[idxNr] = idxTex;
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
  //level = 0;
  internalformat = GL_RGBA;
  format = GL_RGBA;
  type = GL_UNSIGNED_SHORT_1_5_5_5_REV;

  if(target != GL_TEXTURE_2D)
  {
    setError(GL_INVALID_ENUM);
    return;
  }
  if(level < 0)
  {
    setError(GL_INVALID_VALUE);
    return;
  }
  if((GLint)format != internalformat)
  {
    setError(GL_INVALID_OPERATION);
    return;
  }
  if(((format != GL_RGB) && (format != GL_BGR)) &&
     ((type == GL_UNSIGNED_SHORT_5_6_5) ||
      (type == GL_UNSIGNED_SHORT_5_6_5_REV)))
  {
    setError(GL_INVALID_OPERATION);
    return;
  }
  if(((format != GL_RGBA) && (format != GL_BGRA)) &&
     ((type == GL_UNSIGNED_SHORT_4_4_4_4) ||
      (type == GL_UNSIGNED_SHORT_4_4_4_4_REV) ||
      (type == GL_UNSIGNED_SHORT_5_5_5_1) ||
      (type == GL_UNSIGNED_SHORT_1_5_5_5_REV)))
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  if(level > 6)
  {
//    setError(GL_INVALID_OPERATION);
    return;
  }

  if((pCurrentTex_ == 0) || (pCurrentTex_->used == false))
  {
    setError(GL_INVALID_OPERATION);
    return;
  }

  if(pCurrentTex_->initialized == false)
  {
    // Initialize the texture with MipMap level 0 first
    if(level != 0)
    {
      setError(GL_INVALID_OPERATION);
      return;
    }

    pCurrentTex_->texMinFilter = GL_LINEAR;
    pCurrentTex_->texMagFilter = GL_LINEAR;
    pCurrentTex_->texWrapS     = GL_REPEAT;
    pCurrentTex_->texWrapT     = GL_REPEAT;
    for(int iLevel(0); iLevel < 7; iLevel++)
      pCurrentTex_->level_[iLevel].initialized = false;
    pCurrentTex_->initialized  = true;
  }

  if(level == 0)
  {
    GLint widthBit;
    GLint heightBit;

    // Clear every level
    for(int iLevel(0); iLevel < 7; iLevel++)
      pCurrentTex_->level_[iLevel].initialized = false;

    switch(width)
    {
      case   64: widthBit =  6; break;
      case  128: widthBit =  7; break;
      case  256: widthBit =  8; break;
      case  512: widthBit =  9; break;
      case 1024: widthBit = 10; break;
      default:
        setError(GL_INVALID_VALUE);
        return;
    };
    switch(height)
    {
      case   64: heightBit =  6; break;
      case  128: heightBit =  7; break;
      case  256: heightBit =  8; break;
      case  512: heightBit =  9; break;
      case 1024: heightBit = 10; break;
      default:
        setError(GL_INVALID_VALUE);
        return;
    };

    for(int iLevel(0); iLevel < 7; iLevel++)
    {
      pCurrentTex_->level_[iLevel].width       = width    >> iLevel;
      pCurrentTex_->level_[iLevel].height      = height   >> iLevel;
      pCurrentTex_->level_[iLevel].widthBitNr  = widthBit  - iLevel;
      pCurrentTex_->level_[iLevel].heightBitNr = heightBit - iLevel;
    }

    pCurrentTex_->rgba_ = ((format == GL_RGBA) || (format == GL_BGRA));
  }
  else
  {
    // MipMap level > 0

    // Previous level must be initialized
    if(pCurrentTex_->level_[level-1].initialized == false)
    {
      setError(GL_INVALID_OPERATION);
      return;
    }
    // Width and height must be >= 8
    if((width < 8) || (height < 8))
    {
//      setError(GL_INVALID_OPERATION);
      return;
    }
    // Width and height must match the selected level
    if((pCurrentTex_->level_[level].width != width) || (pCurrentTex_->level_[level].height != height))
    {
      setError(GL_INVALID_OPERATION);
      return;
    }
    // RGB/RGBA must match
    if(pCurrentTex_->rgba_ != ((format == GL_RGBA) || (format == GL_BGRA)))
    {
      setError(GL_INVALID_OPERATION);
      return;
    }
  }

  EColorFormat fmtTo;
  EColorFormat fmtFrom = convertGLToBxColorFormat(format, type);
  if(fmtFrom == cfUNKNOWN)
  {
    setError(GL_INVALID_ENUM);
    return;
  }

  if(colorFormatOps[fmtFrom].bitsPerPixel <= 16)
  {
    pCurrentTex_->psm_ = GRAPH_PSM_16;
    fmtTo = cfA1B5G5R5;
  }
  else
  {
    pCurrentTex_->psm_ = GRAPH_PSM_32;
    fmtTo = cfA8B8G8R8;
  }

  uint32_t texAddr(0);
  if(device_.allocTexture(texAddr, width, height, pCurrentTex_->psm_) == false)
  {
    setError(GL_OUT_OF_MEMORY);
    return;
  }

  if(fmtTo == fmtFrom)
  {
    ee_to_gsBitBlt(texAddr, width, pCurrentTex_->psm_, 0, 0, width, height, (uint32_t)pixels);
  }
  else if(pCurrentTex_->psm_ == GRAPH_PSM_16)
  {
    uint16_t * newTexture = new uint16_t[width*height];
    convertImageFormat(newTexture, fmtTo, pixels, fmtFrom, width, height);
    ee_to_gsBitBlt(texAddr, width, pCurrentTex_->psm_, 0, 0, width, height, (uint32_t)newTexture);
    delete newTexture;
  }
  else // 32bit
  {
    uint32_t * newTexture = new uint32_t[width*height];
    convertImageFormat(newTexture, fmtTo, pixels, fmtFrom, width, height);
    ee_to_gsBitBlt(texAddr, width, pCurrentTex_->psm_, 0, 0, width, height, (uint32_t)newTexture);
    delete newTexture;
  }

  pCurrentTex_->level_[level].data = (void *)texAddr;
  if(level > pCurrentTex_->maxLevel_)
    pCurrentTex_->maxLevel_ = level;
  pCurrentTex_->level_[level].initialized = true;
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::glTexParameterf(GLenum target, GLenum pname, GLfloat param)
{
  if(target != GL_TEXTURE_2D)
  {
    setError(GL_INVALID_ENUM);
    return;
  }

  if(pCurrentTex_ != 0)
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
CPS2GLESContext::glBegin(GLenum mode)
{
  CASoftGLESFloat::glBegin(mode);

//  switch(mode)
//  {
//    case GL_TRIANGLES:
      packet_.gifAddPackedAD(GIF::REG::prim, GS_PRIM(PRIM_TRI,       ps2Shading_, ps2Textures_, ps2Fog_, ps2AlphaBlend_, ps2Aliasing_, TEXTURES_ST, 0, 0));
//      break;
//    case GL_TRIANGLE_STRIP:
//      packet_.gifAddPackedAD(GIF::REG::prim, GS_PRIM(PRIM_TRI_STRIP, ps2Shading_, ps2Textures_, ps2Fog_, ps2AlphaBlend_, ps2Aliasing_, TEXTURES_ST, 0, 0));
//      break;
//    case GL_TRIANGLE_FAN:
//      packet_.gifAddPackedAD(GIF::REG::prim, GS_PRIM(PRIM_TRI_FAN,   ps2Shading_, ps2Textures_, ps2Fog_, ps2AlphaBlend_, ps2Aliasing_, TEXTURES_ST, 0, 0));
//      break;
//  };
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::glEnd()
{
  CASoftGLESFloat::glEnd();

  bDataWaiting_ = true;
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::rasterTriangleClip(SVertexF & v0, SVertexF & v1, SVertexF & v2, uint32_t clipBit)
{
  SVertexF * va[3] = {&v0, &v1, &v2};
  uint8_t alpha;

  for(int iVertex(0); iVertex < 3; iVertex++)
  {
    SVertexF & v = *va[iVertex];

    uint32_t z = ps2DepthInvert_ ? (zMax_ - v.sz) : v.sz;

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
      GLfloat tq = 1.0f / v.vc.w;
      GLfloat ts = v.t[0] * tq;
      GLfloat tt = v.t[1] * tq;

      packet_.gifAddPackedAD(GIF::REG::st,    GIF::REG::ST(ts, tt));
      packet_.gifAddPackedAD(GIF::REG::rgbaq, GIF::REG::RGBAQ((uint8_t)(v.cl.r*255), (uint8_t)(v.cl.g*255), (uint8_t)(v.cl.b*255), alpha, tq));
    }
    else
    {
      packet_.gifAddPackedAD(GIF::REG::rgbaq, GIF::REG::RGBAQ((uint8_t)(v.cl.r*255), (uint8_t)(v.cl.g*255), (uint8_t)(v.cl.b*255), alpha, 0));
    }
    packet_.gifAddPackedAD(GIF::REG::xyz2, GIF::REG::XYZ2((GS_X_BASE+v.sx)<<4, (GS_Y_BASE+v.sy)<<4, z));
  }
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::rasterTriangle(SVertexF & v0, SVertexF & v1, SVertexF & v2)
{
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::zbuffer(bool enable)
{
  if(enable == true)
  {
    if(ps2ZBufferAddr_ == 0)
    {
      // Allocate z-buffer
      device_.allocFramebuffer(ps2ZBufferAddr_, pSurface_->mode.xpitch, pSurface_->mode.height, ps2ZPSM_);
    }
    // Register buffer location and pixel mode
    packet_.gifAddPackedAD(GIF::REG::zbuf_1, GS_ZBUF(ps2ZBufferAddr_ >> 13, ps2ZPSM_, ZMSK_ENABLE));
  }
  else
  {
    // Z-Buffer
    packet_.gifAddPackedAD(GIF::REG::zbuf_1, GS_ZBUF(0, ps2ZPSM_, ZMSK_DISABLE));
  }

  // Z-Buffer test
  packet_.gifAddPackedAD(GIF::REG::test_1, GS_TEST(0, 0, 0, 0, 0, 0, enable, ps2DepthFunction_));
}
