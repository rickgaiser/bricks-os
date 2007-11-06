#include "textures.h"
#include "dma.h"
#include "gs.h"
#include "gif.h"
#include "string.h"


DECLARE_GS_PACKET(gs_tex_buf,50);
extern uint32_t   gs_mem_current;


//-----------------------------------------------------------------------------
CAGLESTexturesPS2::CAGLESTexturesPS2()
 : pCurrentTex_(0)
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
        ((uint32_t)pCurrentTex_->data)/256,            // base pointer
        pCurrentTex_->width/64,              // width
        0,                              // 32bit RGBA
        gs_texture_wh(pCurrentTex_->width),    // width
        gs_texture_wh(pCurrentTex_->height),    // height
        1,                              // RGBA
        TEX_DECAL,                      // just overwrite existing pixels
        0,0,0,0,0));

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
      pCurrentTex_->width  = width;
      pCurrentTex_->height = height;
      pCurrentTex_->data   = (void *)gs_mem_current;

      // Copy to texture memory
      gs_load_texture(0, 0, width, height, (uint32_t)pixels, gs_mem_current, width);
    }
  }
}
