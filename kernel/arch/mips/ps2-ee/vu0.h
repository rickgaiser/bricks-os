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


#ifndef PS2_VU0_H
#define PS2_VU0_H


#include "GL/gl.h"
#include "vhl/matrix.h"


namespace VU0
{


  class CMatrix4x4
   : public TMatrix4x4<GLfloat>
  {
  public:
    // Constructors
    CMatrix4x4(){}
    CMatrix4x4(const TMatrix4x4<GLfloat> & m);
    CMatrix4x4(const GLfloat * m);
    CMatrix4x4(GLfloat _m00, GLfloat _m01, GLfloat _m02, GLfloat _m03,
               GLfloat _m10, GLfloat _m11, GLfloat _m12, GLfloat _m13,
               GLfloat _m20, GLfloat _m21, GLfloat _m22, GLfloat _m23,
               GLfloat _m30, GLfloat _m31, GLfloat _m32, GLfloat _m33);

    virtual TMatrix4x4<GLfloat> & operator*=(const GLfloat * m);

    virtual void transform4(const GLfloat * from, GLfloat * to);
    virtual void transform4list(const GLfloat * from, int fromInc, GLfloat * to, int toInc, int count);
  };


};


#endif
