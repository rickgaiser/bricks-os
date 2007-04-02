#ifndef OPENGL_H
#define OPENGL_H


#include "kernel/videoManager.h"


// void glBegin(GLenum mode);
#define GL_POINTS              0x00000001
#define GL_LINES               0x00000002
#define GL_LINE_STRIP          0x00000004
#define GL_LINE_LOOP           0x00000008
#define GL_TRIANGLES           0x00000010
#define GL_TRIANGLE_STRIP      0x00000020
#define GL_TRIANGLE_FAN        0x00000040
#define GL_QUADS               0x00000080
#define GL_QUAD_STRIP          0x00000100
#define GL_POLYGON             0x00000200
// void glShadeModel(GLenum mode);
#define GL_FLAT                0x00000010
#define GL_SMOOTH              0x00000020
// void glMatrixMode(GLenum mode);
#define GL_MODELVIEW           0x00000001
#define GL_PROJECTION          0x00000002
#define GL_TEXTURE             0x00000004
#define GL_COLOR               0x00000008
// void glEnable(GLenum cap);
#define GL_DEPTH_TEST          0x00000001
#define GL_LIGHTING            0x00000002
#define GL_COLOR_MATERIAL      0x00000004
#define GL_TEXTURE_2D          0x00000008
#define GL_CULL_FACE           0x00000010
// void glLightModelfv(GLenum pname, const GLfloat * params);
#define GL_LIGHT_MODEL_AMBIENT 0x00000001
#define GL_AMBIENT             0x00000002
#define GL_DIFFUSE             0x00000004
#define GL_SPECULAR            0x00000008
// void glLightfv(GLenum light, GLenum pname, const GLfloat * params);
#define GL_LIGHT0              0x00000001
#define GL_LIGHT1              0x00000002
#define GL_LIGHT2              0x00000004
#define GL_LIGHT3              0x00000008
#define GL_LIGHT4              0x00000010
#define GL_LIGHT5              0x00000020
#define GL_LIGHT6              0x00000040
#define GL_LIGHT7              0x00000080

// GLbitfield bits
#define GL_COLOR_BUFFER_BIT    0x00000001
#define GL_DEPTH_BUFFER_BIT    0x00000002

typedef char           GLbyte;
typedef short          GLshort;
typedef long           GLint;
typedef long           GLsizei;
typedef float          GLfloat;
typedef float          GLclampf;
typedef double         GLdouble;
typedef double         GLclampd;
typedef unsigned char  GLubyte;
typedef unsigned char  GLboolean;
typedef unsigned short GLushort;
typedef unsigned long  GLuint;
typedef unsigned long  GLenum;
typedef unsigned long  GLbitfield;
typedef void           GLvoid;


void glSetSurface(CSurface * surface);

// ------
// GL API
// ------
void glBegin(GLenum mode);
void glEnd();

void glVertex3i(GLint x, GLint y, GLint z);
void glVertex3f(GLfloat x, GLfloat y, GLfloat z);
void glVertex3iv(const GLint * v);
void glVertex3fv(const GLfloat * v);

void glColor3ub(GLubyte red, GLubyte green, GLubyte blue);
void glColor3f(GLfloat red, GLfloat green, GLfloat blue);

//void glTexCoord2d(GLdouble s, GLdouble t);
//void glTexCoord2f(GLfloat s, GLfloat t);
//void glTexCoord2i(GLint s, GLint t);
//void glTexCoord2s(GLshort s, GLshort t);

//void glTexImage2D(GLenum target, GLint level, GLint components, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid * pixels);

//void glNormal3b(GLbyte nx, GLbyte ny, GLbyte nz);
//void glNormal3d(GLdouble nx, GLdouble ny, GLdouble nz);
//void glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz);
void glNormal3i(GLint nx, GLint ny, GLint nz);
//void glNormal3s(GLshort nx, GLshort ny, GLshort nz);
//void glNormal3bv(const GLbyte * v);
//void glNormal3dv(const GLdouble * v);
//void glNormal3fv(const GLfloat * v);
void glNormal3iv(const GLint * v);
//void glNormal3sv(const GLshort * v);

// Matrix operations
void glMatrixMode(GLenum mode);
void glLoadIdentity();
void glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
void glScalef(GLfloat x, GLfloat y, GLfloat z);
void glTranslatef(GLfloat x, GLfloat y, GLfloat z);

void glClear(GLbitfield mask);
void glViewport(GLint x, GLint y, GLsizei width, GLsizei height);

void glShadeModel(GLenum mode);
void glEnable(GLenum cap);
void glDisable(GLenum cap);

void glLightModeliv(GLenum pname, const GLint * params);
void glLightModelfv(GLenum pname, const GLfloat * params);
void glLightfv(GLenum light, GLenum pname, const GLfloat * params);
void glColorMaterial(GLenum face, GLenum mode);
void glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
void glClearColor3ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);

// -------
// GLU API
// -------
void gluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);


#endif
