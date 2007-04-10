#ifndef OPENGL_H
#define OPENGL_H


#include "GLES/gl.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef double         GLdouble;


// -------
// GLU API
// -------
void gluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);


#ifdef __cplusplus
}
#endif


#endif
