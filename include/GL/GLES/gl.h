/*
 * Bricks-OS, Operating System for Game Consoles
 * Copyright (C) 2008 Maximus32 <Maximus32@bricks-os.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA
 */


#ifndef GLES_GL_H
#define GLES_GL_H


#include "GL/gl.h"


#ifdef __cplusplus
extern "C" {
#endif


GLAPI void APIENTRY glClearColorx (GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha);
GLAPI void APIENTRY glClearDepthx (GLclampx depth);
GLAPI void APIENTRY glDepthRangex (GLclampx zNear, GLclampx zFar);
GLAPI void APIENTRY glOrthox (GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar);
GLAPI void APIENTRY glFrustumx (GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar);
GLAPI void APIENTRY glLoadMatrixx (const GLfixed *m);
GLAPI void APIENTRY glMultMatrixx (const GLfixed *m);
GLAPI void APIENTRY glRotatex (GLfixed angle, GLfixed x, GLfixed y, GLfixed z);
GLAPI void APIENTRY glScalex (GLfixed x, GLfixed y, GLfixed z);
GLAPI void APIENTRY glTranslatex (GLfixed x, GLfixed y, GLfixed z);
GLAPI void APIENTRY glNormal3x (GLfixed nx, GLfixed ny, GLfixed nz);
GLAPI void APIENTRY glColor4x (GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha);
GLAPI void APIENTRY glLightx (GLenum light, GLenum pname, GLfixed param);
GLAPI void APIENTRY glLightxv (GLenum light, GLenum pname, const GLfixed *params);
GLAPI void APIENTRY glMaterialx (GLenum face, GLenum pname, GLfixed param);
GLAPI void APIENTRY glMaterialxv (GLenum face, GLenum pname, const GLfixed *params);
GLAPI void APIENTRY glTexParameterx (GLenum target, GLenum pname, GLfixed param);
GLAPI void APIENTRY glFogx (GLenum pname, GLfixed param);
GLAPI void APIENTRY glFogxv (GLenum pname, const GLfixed *params);


#ifdef __cplusplus
}
#endif


#endif
