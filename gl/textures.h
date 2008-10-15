#ifndef TEXTURES_H
#define TEXTURES_H


#include "context.h"


#define MAX_TEXTURE_COUNT 32


//-----------------------------------------------------------------------------
class CTexture
{
public:
  CTexture();
  virtual ~CTexture();

  virtual void init();
  virtual void free();
  virtual void bind();

public:
  GLsizei width;
  GLsizei height;

  GLint minFilter;
  GLint magFilter;
  GLint wrapS;
  GLint wrapT;
};

//-----------------------------------------------------------------------------
class CSoftTexture
 : public CTexture
{
public:
  CSoftTexture();
  virtual ~CSoftTexture();

  virtual void init();
  virtual void free();

public:
  uint32_t maskWidth;
  uint32_t maskHeight;
  void * data;
};

//-----------------------------------------------------------------------------
class CAGLESTextures
 : public virtual I3DRenderer
{
public:
  CAGLESTextures();
  virtual ~CAGLESTextures();

  virtual void glBindTexture(GLenum target, GLuint texture);
  virtual void glDeleteTextures(GLsizei n, const GLuint *textures);
  virtual void glGenTextures(GLsizei n, GLuint *textures);
  virtual void glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
  virtual void glTexParameterf(GLenum target, GLenum pname, GLfloat param);
  virtual void glTexParameterx(GLenum target, GLenum pname, GLfixed param);

protected:
  virtual CTexture * getTexture();

protected:
  CTexture * pCurrentTex_;

private:
  CTexture * textures_[MAX_TEXTURE_COUNT];
};

//-----------------------------------------------------------------------------
int convertImageFormat(void * dst, EColorFormat dstFmt, const void * src, EColorFormat srcFmt, int width, int height);
EColorFormat convertGLToBxColorFormat(GLenum format, GLenum type);
uint8_t getBitNr(uint32_t value);


#endif
