#include "textures.h"
#include "dma.h"
#include "gs.h"
#include "gif.h"
#include "string.h"


DECLARE_GS_PACKET(gs_tex_buf,50);
extern uint32_t   ps2TexturesStart_;


//-----------------------------------------------------------------------------
CAGLESTexturesPS2::CAGLESTexturesPS2()
 : texturesEnabled_(false)
 , pCurrentTex_(NULL)
{
  for(GLuint idx(0); idx < MAX_TEXTURE_COUNT; idx++)
    textures_[idx].used = false;
}

//-----------------------------------------------------------------------------
CAGLESTexturesPS2::~CAGLESTexturesPS2()
{
}

//-----------------------------------------------------------------------------
void
CAGLESTexturesPS2::glBindTexture(GLenum target, GLuint texture)
{
  if(target == GL_TEXTURE_2D)
  {
    pCurrentTex_ = &textures_[texture];

    BEGIN_GS_PACKET(gs_tex_buf);
    GIF_TAG_AD(gs_tex_buf, 1, 0, 0, 0);

    GIF_DATA_AD(gs_tex_buf, tex0_1,
      GS_TEX0(
        ((uint32_t)pCurrentTex_->data)>>8,   // base pointer
        pCurrentTex_->width>>6,              // width
        0,                                   // 32bit RGBA
        gs_texture_wh(pCurrentTex_->width),  // width
        gs_texture_wh(pCurrentTex_->height), // height
        1,                                   // RGBA
        TEX_DECAL,                           // just overwrite existing pixels
        0, 0, 0, 0, 0));

    GIF_DATA_AD(gs_tex_buf, tex1_1,
      GS_TEX1(
        0, 0,
        pCurrentTex_->texMagFilter != GL_NEAREST,
        pCurrentTex_->texMinFilter != GL_NEAREST,
        0, 0, 0));

    SEND_GS_PACKET(gs_tex_buf);
  }
}

//-----------------------------------------------------------------------------
void
CAGLESTexturesPS2::glDeleteTextures(GLsizei n, const GLuint *textures)
{
  for(GLsizei i(0); i < n; i++)
    if(textures[i] < MAX_TEXTURE_COUNT)
      textures_[textures[i]].used = false;
}

//-----------------------------------------------------------------------------
void
CAGLESTexturesPS2::glGenTextures(GLsizei n, GLuint *textures)
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
CAGLESTexturesPS2::glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
{
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
          gs_load_texture(0, 0, width, height, (uint32_t)pixels, ps2TexturesStart_, width);
          break;
        }
        case GL_UNSIGNED_SHORT_5_6_5:
        {
          uint32_t * newTexture = new uint32_t[width*height];
          for(int i(0); i < (width*height); i++)
            newTexture[i] = BxColorFormat_Convert(cfB5G6R5, cfA8B8G8R8, ((uint16_t *)pixels)[i]);
          gs_load_texture(0, 0, width, height, (uint32_t)newTexture, ps2TexturesStart_, width);
          delete newTexture;
          break;
        }
        case GL_UNSIGNED_SHORT_4_4_4_4:
        {
          uint32_t * newTexture = new uint32_t[width*height];
          for(int i(0); i < (width*height); i++)
            newTexture[i] = BxColorFormat_Convert(cfA4B4G4R4, cfA8B8G8R8, ((uint16_t *)pixels)[i]);
          gs_load_texture(0, 0, width, height, (uint32_t)newTexture, ps2TexturesStart_, width);
          delete newTexture;
          break;
        }
        case GL_UNSIGNED_SHORT_5_5_5_1:
        {
          uint32_t * newTexture = new uint32_t[width*height];
          for(int i(0); i < (width*height); i++)
            newTexture[i] = BxColorFormat_Convert(cfA1B5G5R5, cfA8B8G8R8, ((uint16_t *)pixels)[i]);
          gs_load_texture(0, 0, width, height, (uint32_t)newTexture, ps2TexturesStart_, width);
          delete newTexture;
          break;
        }
      };
    }
  }
}

//-----------------------------------------------------------------------------
void
CAGLESTexturesPS2::glTexParameterf(GLenum target, GLenum pname, GLfloat param)
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
CAGLESTexturesPS2::glTexParameterx(GLenum target, GLenum pname, GLfixed param)
{
  glTexParameterf(target, pname, gl_fptof(param));
}
