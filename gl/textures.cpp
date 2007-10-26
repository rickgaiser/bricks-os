#include "textures.h"


//-----------------------------------------------------------------------------
CAGLESTextures::CAGLESTextures()
 : pCurrentTex_(0)
{
  for(GLuint idx(0); idx < MAX_TEXTURE_COUNT; idx++)
    textures_[idx].used = false;
}

//-----------------------------------------------------------------------------
CAGLESTextures::~CAGLESTextures()
{
}

//-----------------------------------------------------------------------------
void
CAGLESTextures::glBindTexture(GLenum target, GLuint texture)
{
  if(target == GL_TEXTURE_2D)
  {
    pCurrentTex_ = &textures_[texture];
  }
}

//-----------------------------------------------------------------------------
void
CAGLESTextures::glDeleteTextures(GLsizei n, const GLuint *textures)
{
  for(GLsizei i(0); i < n; i++)
    if(textures[i] < MAX_TEXTURE_COUNT)
      textures_[textures[i]].used = false;
}

//-----------------------------------------------------------------------------
void
CAGLESTextures::glGenTextures(GLsizei n, GLuint *textures)
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
CAGLESTextures::glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
{
  if(target == GL_TEXTURE_2D)
  {
    if((pCurrentTex_ != 0) && (pCurrentTex_->used == true))
    {
      pCurrentTex_->width  = width;
      pCurrentTex_->height = height;
      pCurrentTex_->data   = pixels;
    }
  }
}
