#ifndef GLU_H
#define GLU_H


#include "GL/gl.h"


#ifdef __cplusplus
extern "C" {
#endif


int  gluBuild2DMipmaps(GLenum target, GLint components, GLint width, GLint height, GLenum format, GLenum type, const void * data);
void gluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);
int  gluScaleImage(GLenum format, GLint widthin, GLint heightin, GLenum typein, const void * datain, GLint widthout, GLint heightout, GLenum typeout, void * dataout);


#ifdef __cplusplus
}
#endif


#endif
