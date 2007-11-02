#ifndef NDS_TEXTURESNDS_H
#define NDS_TEXTURESNDS_H


#include "../../../../gl/context.h"


#define MAX_TEXTURE_COUNT 32


//-----------------------------------------------------------------------------
struct STextureNDS
{
  bool used;

  GLsizei width;
  GLsizei height;

  uint32_t format;

  const void * data;
};

//-----------------------------------------------------------------------------
class CAGLESTexturesNDS
 : public virtual IGLESContext
{
public:
  CAGLESTexturesNDS();
  virtual ~CAGLESTexturesNDS();

  virtual void glBindTexture(GLenum target, GLuint texture);
  virtual void glDeleteTextures(GLsizei n, const GLuint *textures);
  virtual void glGenTextures(GLsizei n, GLuint *textures);
  virtual void glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);

protected:
  STextureNDS * pCurrentTex_;

private:
  STextureNDS textures_[MAX_TEXTURE_COUNT];
};


#endif