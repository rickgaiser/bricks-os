#include "glesContext.h"
#include "bios.h"
#include "dma.h"
#include "gs.h"
#include "matrix.h"
#include "stdlib.h"


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CPS2GLESContext::CPS2GLESContext()
 : CAGLESFxToFloatContext()
 , CAGLESBuffers()
 , CAGLESCull()
 , CAGLESMatrixF()
 , CAPS2Renderer()

 , depthTestEnabled_(false)
 , depthFunction_(GL_LESS)
 , depthClear_(1.0f)
 , zClearValue_(0xffffffff)
 , zbuffer(0)
 , zNear_(0.0f)
 , zFar_(1.0f)
 , zMax_(0xffffffff) // 32bit z-buffer

 , shadingModel_(GL_FLAT)
 , lightingEnabled_(false)
 , normalizeEnabled_(false)
 , fogEnabled_(false)
 , texturesEnabled_(false)
 , pCurrentTex_(NULL)

 , viewportXOffset(0)
 , viewportYOffset(0)
 , viewportPixelCount(0)
 , viewportWidth(0)
 , viewportHeight(0)

 , ps2Shading_(SHADE_FLAT)
 , ps2Textures_(TEXTURES_OFF)
 , ps2Fog_(FOG_OFF)
 , ps2AlphaBlend_(ALPHABLEND_OFF)
 , ps2Aliasing_(ALIASING_ON)
 , ps2DepthFunction_(ZTST_GREATER)
 , ps2DepthInvert_(true)
{
  clCurrent.r = 1.0f;
  clCurrent.g = 1.0f;
  clCurrent.b = 1.0f;
  clCurrent.a = 1.0f;

  clClear.r = 0.0f;
  clClear.g = 0.0f;
  clClear.b = 0.0f;
  clClear.a = 0.0f;

  // Light properties
  for(int iLight(0); iLight < 8; iLight++)
  {
    lights_[iLight].ambient.r = 0.0f;
    lights_[iLight].ambient.g = 0.0f;
    lights_[iLight].ambient.b = 0.0f;
    lights_[iLight].ambient.a = 1.0f;

    if(iLight == 0)
    {
      lights_[iLight].diffuse.r = 1.0f;
      lights_[iLight].diffuse.g = 1.0f;
      lights_[iLight].diffuse.b = 1.0f;
      lights_[iLight].diffuse.a = 1.0f;

      lights_[iLight].specular.r = 1.0f;
      lights_[iLight].specular.g = 1.0f;
      lights_[iLight].specular.b = 1.0f;
      lights_[iLight].specular.a = 1.0f;
    }
    else
    {
      lights_[iLight].diffuse.r = 0.0f;
      lights_[iLight].diffuse.g = 0.0f;
      lights_[iLight].diffuse.b = 0.0f;
      lights_[iLight].diffuse.a = 0.0f;

      lights_[iLight].specular.r = 0.0f;
      lights_[iLight].specular.g = 0.0f;
      lights_[iLight].specular.b = 0.0f;
      lights_[iLight].specular.a = 0.0f;
    }

    lights_[iLight].position.x = 0.0f;
    lights_[iLight].position.y = 0.0f;
    lights_[iLight].position.z = 1.0f;
    lights_[iLight].position.w = 0.0f;
    lights_[iLight].direction  = lights_[iLight].position;
    lights_[iLight].direction.invert();

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

  zA_ = (zFar_ - zNear_) / 2;
  zB_ = (zFar_ + zNear_) / 2;

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
    packet_.data(test_1, GS_TEST(0, 0, 0, 0, 0, 0, 1, ZTST_ALWAYS));
  }

  if(mask & GL_COLOR_BUFFER_BIT)
  {
    uint8_t r = (uint8_t)(clClear.r * 255);
    uint8_t g = (uint8_t)(clClear.g * 255);
    uint8_t b = (uint8_t)(clClear.b * 255);
    //uint8_t a = (uint8_t)(clClear.a * 255);

    packet_.data(prim, GS_PRIM(PRIM_SPRITE, 0, 0, 0, 0, 0, 0, 0, 0));
    packet_.data(rgbaq, GS_RGBAQ(r, g, b, 0x80, 0));
    packet_.data(xyz2, GS_XYZ2((0+GS_X_BASE)<<4, (0+GS_Y_BASE)<<4, 0));
    packet_.data(xyz2, GS_XYZ2((viewportWidth+GS_X_BASE)<<4, (viewportHeight+GS_Y_BASE)<<4, 0));
  }

  if(mask & GL_DEPTH_BUFFER_BIT)
  {
    packet_.data(test_1, GS_TEST(0, 0, 0, 0, 0, 0, depthTestEnabled_, ps2DepthFunction_));
  }
  bDataWaiting_ = true;
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
  clClear.r = red;
  clClear.g = green;
  clClear.b = blue;
  clClear.a = alpha;
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::glClearDepthf(GLclampf depth)
{
  depthClear_ = depth;
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::glColor4ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha)
{
  clCurrent.r = (GLfloat)red   / 255.0f;
  clCurrent.g = (GLfloat)green / 255.0f;
  clCurrent.b = (GLfloat)blue  / 255.0f;
  clCurrent.a = (GLfloat)alpha / 255.0f;
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
  clCurrent.r = red;
  clCurrent.g = green;
  clCurrent.b = blue;
  clCurrent.a = alpha;
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::glDepthRangef(GLclampf zNear, GLclampf zFar)
{
  zNear_ = clampf(zNear);
  zFar_  = clampf(zFar);

  zA_ = (zFar_ - zNear_) / 2;
  zB_ = (zFar_ + zNear_) / 2;
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz)
{
  normal_.x = nx;
  normal_.y = ny;
  normal_.z = nz;

  if(normalizeEnabled_  == true)
    normal_.normalize();
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
    packet_.data(test_1, GS_TEST(0, 0, 0, 0, 0, 0, depthTestEnabled_, ps2DepthFunction_));
  }
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::glDisable(GLenum cap)
{
  switch(cap)
  {
    case GL_LIGHTING:   lightingEnabled_   = false; break;
    case GL_LIGHT0:     lights_[0].enabled = false; break;
    case GL_LIGHT1:     lights_[1].enabled = false; break;
    case GL_LIGHT2:     lights_[2].enabled = false; break;
    case GL_LIGHT3:     lights_[3].enabled = false; break;
    case GL_LIGHT4:     lights_[4].enabled = false; break;
    case GL_LIGHT5:     lights_[5].enabled = false; break;
    case GL_LIGHT6:     lights_[6].enabled = false; break;
    case GL_LIGHT7:     lights_[7].enabled = false; break;
/*
    case GL_DEPTH_TEST:
    {
      depthTestEnabled_ = false;
      // Z-Buffer
      packet_.data(zbuf_1, GS_ZBUF(gs_mem_current >> 13, GRAPH_PSM_16, ZMSK_DISABLE));
      // Z-Buffer test
      packet_.data(test_1, GS_TEST(0, 0, 0, 0, 0, 0, depthTestEnabled_, ps2DepthFunction_));
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
CPS2GLESContext::glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
  if(bBufVertexEnabled_ == false)
    return;

  GLint idxVertex  (first * bufVertex_.size);
  GLint idxColor   (first * bufColor_.size);
  GLint idxNormal  (first * bufNormal_.size);
  GLint idxTexCoord(first * bufTexCoord_.size);
  SVertexF v;

  begin(mode);

  // Process all vertices
  for(GLint i(0); i < count; i++)
  {
    // Vertex
    switch(bufVertex_.type)
    {
      case GL_FLOAT:
        v.v[0] = ((GLfloat *)bufVertex_.pointer)[idxVertex++];
        v.v[1] = ((GLfloat *)bufVertex_.pointer)[idxVertex++];
        v.v[2] = ((GLfloat *)bufVertex_.pointer)[idxVertex++];
        v.v[3] = 1.0f;
        break;
      case GL_FIXED:
        v.v[0] = gl_fptof(((GLfixed *)bufVertex_.pointer)[idxVertex++]);
        v.v[1] = gl_fptof(((GLfixed *)bufVertex_.pointer)[idxVertex++]);
        v.v[2] = gl_fptof(((GLfixed *)bufVertex_.pointer)[idxVertex++]);
        v.v[3] = 1.0f;
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
      v.n = normal_;

    // Textures
    if(texturesEnabled_ == true)
    {
      // Textures
      if(bBufTexCoordEnabled_ == true)
      {
        switch(bufTexCoord_.type)
        {
          case GL_FLOAT:
            v.t[0] =         ((GLfloat *)bufTexCoord_.pointer)[idxTexCoord++];
            v.t[1] = (1.0f - ((GLfloat *)bufTexCoord_.pointer)[idxTexCoord++]);
            break;
          case GL_FIXED:
            v.t[0] =         gl_fptof(((GLfixed *)bufTexCoord_.pointer)[idxTexCoord++]);
            v.t[1] = (1.0f - gl_fptof(((GLfixed *)bufTexCoord_.pointer)[idxTexCoord++]));
            break;
        };
      }
    }

    // Color
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
      v.cl = clCurrent;

    // -------------
    // Vertex shader
    // -------------
    vertexShader(v);

    // ------------
    // Raterization
    // ------------
    rasterize(v);
  }

  end();
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
/*
    case GL_DEPTH_TEST:
    {
      depthTestEnabled_ = true;
      // Z-Buffer
      switch(renderSurface->mode.bpp)
      {
        case 16:
        {
          packet_.data(zbuf_1, GS_ZBUF(gs_mem_current >> 13, GRAPH_PSM_16, ZMSK_ENABLE));
          ps2TexturesStart_ = gs_mem_current + (renderSurface->mode.width * renderSurface->mode.height * 2);
          zMax_ = 0xffff;
          break;
        }
        case 24:
        {
          packet_.data(zbuf_1, GS_ZBUF(gs_mem_current >> 13, GRAPH_PSM_24, ZMSK_ENABLE));
          ps2TexturesStart_ = gs_mem_current + (renderSurface->mode.width * renderSurface->mode.height * 3);
          zMax_ = 0xffffff;
          break;
        }
        case 32:
        {
          packet_.data(zbuf_1, GS_ZBUF(gs_mem_current >> 13, GRAPH_PSM_32, ZMSK_ENABLE));
          ps2TexturesStart_ = gs_mem_current + (renderSurface->mode.width * renderSurface->mode.height * 4);
          zMax_ = 0xffffffff;
          break;
        }
      };
      // Z-Buffer test
      packet_.data(test_1, GS_TEST(0, 0, 0, 0, 0, 0, depthTestEnabled_, ps2DepthFunction_));
      break;
    }
*/
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
CPS2GLESContext::glFinish(void)
{
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::glFlush(void)
{
  flush();
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::glFogf(GLenum pname, GLfloat param)
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
CPS2GLESContext::glFogfv(GLenum pname, const GLfloat * params)
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
CPS2GLESContext::glLightf(GLenum light, GLenum pname, GLfloat param)
{
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::glLightfv(GLenum light, GLenum pname, const GLfloat * params)
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
    case GL_AMBIENT:  pColor = &pLight->ambient;  break;
    case GL_DIFFUSE:  pColor = &pLight->diffuse;  break;
    case GL_SPECULAR: pColor = &pLight->specular; break;
    case GL_POSITION:
      pLight->position.x = params[0];
      pLight->position.y = params[1];
      pLight->position.z = params[2];
      pLight->position.w = params[3];
      // Invert and normalize
      pLight->direction = pLight->position;
      pLight->direction.invert();
      pLight->direction.normalize();
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
CPS2GLESContext::glMaterialf(GLenum face, GLenum pname, GLfloat param)
{
  switch(pname)
  {
    case GL_SHININESS:
      matShininess_ = param;
      break;
    default:
      return;
  }
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::glMaterialfv(GLenum face, GLenum pname, const GLfloat * params)
{
  switch(pname)
  {
    case GL_AMBIENT:
      matColorAmbient_.r = params[0];
      matColorAmbient_.g = params[1];
      matColorAmbient_.b = params[2];
      matColorAmbient_.a = params[3];
      break;
    case GL_DIFFUSE:
      matColorDiffuse_.r = params[0];
      matColorDiffuse_.g = params[1];
      matColorDiffuse_.b = params[2];
      matColorDiffuse_.a = params[3];
      break;
    case GL_SPECULAR:
      matColorSpecular_.r = params[0];
      matColorSpecular_.g = params[1];
      matColorSpecular_.b = params[2];
      matColorSpecular_.a = params[3];
      break;
    case GL_EMISSION:
      matColorEmission_.r = params[0];
      matColorEmission_.g = params[1];
      matColorEmission_.b = params[2];
      matColorEmission_.a = params[3];
      break;
    case GL_SHININESS:
      matShininess_ = params[0];
      break;
    case GL_AMBIENT_AND_DIFFUSE:
      matColorAmbient_.r = params[0];
      matColorAmbient_.g = params[1];
      matColorAmbient_.b = params[2];
      matColorAmbient_.a = params[3];
      matColorDiffuse_.r = params[0];
      matColorDiffuse_.g = params[1];
      matColorDiffuse_.b = params[2];
      matColorDiffuse_.a = params[3];
      break;
    default:
      return;
  }
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
CPS2GLESContext::glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
  viewportXOffset    = x;
  viewportYOffset    = y;
  viewportWidth      = width;
  viewportHeight     = height;
  viewportPixelCount = width * height;

  xA_ = (viewportWidth  >> 1);
  xB_ = (GLfloat)(viewportWidth  >> 1) + 0.5f;
  yA_ = -(viewportHeight >> 1);
  yB_ = (GLfloat)(viewportHeight >> 1) + 0.5f;
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::glBindTexture(GLenum target, GLuint texture)
{
  if(target == GL_TEXTURE_2D)
  {
    pCurrentTex_ = &textures_[texture];

    packet_.data(tex0_1,
      GS_TEX0(
        ((uint32_t)pCurrentTex_->data)>>8,   // base pointer
        pCurrentTex_->width>>6,              // width
        0,                                   // 32bit RGBA
        getBitNr(pCurrentTex_->width),       // width
        getBitNr(pCurrentTex_->height),      // height
        1,                                   // RGBA
        TEX_DECAL,                           // just overwrite existing pixels
        0, 0, 0, 0, 0));

    packet_.data(tex1_1,
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
inline GLfloat
my_pow(GLfloat x, int y)
{
  GLfloat rv(x);
  for(int i(1); i < y; i++)
    rv *= x;
  return rv;
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::vertexShader(SVertexF & v)
{
  // --------------
  // Transformation
  // --------------
  // Model-View matrix
  //   from 'object coordinates' to 'eye coordinates'
  matrixModelView.transform4(v.v, v.v);
  // Projection matrix
  //   from 'eye coordinates' to 'clip coordinates'
  matrixProjection.transform4(v.v, v.v);
  // Perspective division
  //   from 'clip coordinates' to 'normalized device coordinates'
  GLfloat iw = 1.0f / v.v[3];
  v.v[0] *= iw;
  v.v[1] *= iw;
  v.v[2] *= iw;
  // Viewport transformation
  //   from 'normalized device coordinates' to 'window coordinates'
  v.sx = (GLint)    ((xA_ * v.v[0]) + xB_);
  v.sy = (GLint)    ((yA_ * v.v[1]) + yB_);
  v.sz = (uint32_t)(((zA_ * v.v[2]) + zB_) * zMax_);
//  if(ps2DepthInvert_ == true)
//    v.sz = (zMax_ - v.sz);

  // --------
  // Lighting
  // --------
  if(lightingEnabled_ == true)
  {
    SColorF c(0, 0, 0, 0);

    // Normal Rotation
    matrixNormal.transform3(v.n, v.n);

    for(int iLight(0); iLight < 8; iLight++)
    {
      if(lights_[iLight].enabled == true)
      {
        // Ambient light (it's everywhere!)
        c += lights_[iLight].ambient * matColorAmbient_;

        // Diffuse light
        GLfloat diffuse = -lights_[iLight].direction.dotProduct(v.n);
        if(diffuse >= 0.0f)
        {
          c += lights_[iLight].diffuse * matColorDiffuse_ * diffuse;
        }

        if(matShininess_ >= 0.5f)
        {
          // Specular light
          TVector3<GLfloat> eye(0, 0, 1);
          GLfloat specular = lights_[iLight].direction.getCrossProduct(v.n).dotProduct(eye);
          if(specular >= 0.0f)
          {
            specular = my_pow(specular, (int)(matShininess_ + 0.5f));
            c += lights_[iLight].specular * matColorSpecular_ * specular;
          }
        }
      }
    }

    // Multiply vertex color by calculated color
    v.cl *= c;
    // Clamp to 0..1
    v.cl.clamp();
  }

  // Fog
  if(fogEnabled_ == true)
  {
    GLfloat partFog, partColor;
    partFog = clampf((abs(v.v[2]) - fogStart_) / (fogEnd_ - fogStart_));
    partColor = 1.0f - partFog;
    v.cl = ((v.cl * partColor) + (fogColor_ * partFog)).getClamped();
  }
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::rasterize(SVertexF & v)
{
  // Add to message
  if(texturesEnabled_ == true)
  {
    float tq = 1.0f / v.v[3];
    v.t[0] *= tq;
    v.t[1] *= tq;

    packet_.data(st, GS_ST(*(uint32_t *)(&v.t[0]), *(uint32_t *)(&v.t[1])));
    packet_.data(rgbaq, GS_RGBAQ((uint8_t)(v.cl.r*255), (uint8_t)(v.cl.g*255), (uint8_t)(v.cl.b*255), 100, *(uint32_t *)(&tq)));
  }
  else
  {
    packet_.data(rgbaq, GS_RGBAQ((uint8_t)(v.cl.r*255), (uint8_t)(v.cl.g*255), (uint8_t)(v.cl.b*255), 100, 0));
  }
  packet_.data(xyz2, GS_XYZ2((GS_X_BASE+v.sx)<<4, (GS_Y_BASE+v.sy)<<4, v.sz));
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::begin(GLenum mode)
{
  rasterMode_ = mode;

  switch(mode)
  {
    case GL_TRIANGLES:
      packet_.data(prim, GS_PRIM(PRIM_TRI, ps2Shading_, ps2Textures_, ps2Fog_, ps2AlphaBlend_, ps2Aliasing_, TEXTURES_ST, 0, 0));
      break;
    case GL_TRIANGLE_STRIP:
      packet_.data(prim, GS_PRIM(PRIM_TRI_STRIP, ps2Shading_, ps2Textures_, ps2Fog_, ps2AlphaBlend_, ps2Aliasing_, TEXTURES_ST, 0, 0));
      break;
    case GL_TRIANGLE_FAN:
      packet_.data(prim, GS_PRIM(PRIM_TRI_FAN, ps2Shading_, ps2Textures_, ps2Fog_, ps2AlphaBlend_, ps2Aliasing_, TEXTURES_ST, 0, 0));
      break;
  };
}

//-----------------------------------------------------------------------------
void
CPS2GLESContext::end()
{
  bDataWaiting_ = true;
}
