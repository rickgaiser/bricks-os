#ifndef OPENGL_H
#define OPENGL_H


#include "kernel/videoManager.h"
#include "GLES/gl.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef double         GLdouble;


void glSetSurface(CSurface * surface);

// ------
// GL API
// ------
GL_API void GL_APIENTRY glBegin(GLenum mode);
GL_API void GL_APIENTRY glEnd();
GL_API void GL_APIENTRY glVertex3f(GLfloat x, GLfloat y, GLfloat z);
GL_API void GL_APIENTRY glColor3f(GLfloat red, GLfloat green, GLfloat blue);

// -------
// GLU API
// -------
void gluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);


#ifdef __cplusplus
}
#endif


#endif
