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


#include "vu0.h"


#define ROW_MAJOR


namespace VU0
{


//---------------------------------------------------------------------------
CMatrix4x4::CMatrix4x4(const TMatrix4x4<GLfloat> & m)
 : TMatrix4x4<GLfloat>(m)
{
}

//---------------------------------------------------------------------------
CMatrix4x4::CMatrix4x4(const GLfloat * m)
 : TMatrix4x4<GLfloat>(m)
{
}

//---------------------------------------------------------------------------
CMatrix4x4::CMatrix4x4(GLfloat _m00, GLfloat _m01, GLfloat _m02, GLfloat _m03,
                       GLfloat _m10, GLfloat _m11, GLfloat _m12, GLfloat _m13,
                       GLfloat _m20, GLfloat _m21, GLfloat _m22, GLfloat _m23,
                       GLfloat _m30, GLfloat _m31, GLfloat _m32, GLfloat _m33)
 : TMatrix4x4<GLfloat>(
    _m00, _m01, _m02, _m03,
    _m10, _m11, _m12, _m13,
    _m20, _m21, _m22, _m23,
    _m30, _m31, _m32, _m33)
{
}

//-----------------------------------------------------------------------------
TMatrix4x4<GLfloat> &
CMatrix4x4::operator*=(const GLfloat * m)
{
  asm __volatile__
  (
    // Load m
    "lqc2         vf4, 0x00(%1)\n" // load m[0][xyzw] to vf4
    "lqc2         vf5, 0x10(%1)\n" // load m[1][xyzw] to vf5
    "lqc2         vf6, 0x20(%1)\n" // load m[2][xyzw] to vf6
    "lqc2         vf7, 0x30(%1)\n" // load m[3][xyzw] to vf7

    // m_[0][xyzw] = m_[0][xyzw] * m
    "lqc2         vf8, 0x00(%0)\n" // load m_[0][xyzw] to vf8
    "vmulax.xyzw  ACC, vf4, vf8\n" // acc  = vf4.xyzw * vf8.x
    "vmadday.xyzw ACC, vf5, vf8\n" // acc += vf5.xyzw * vf8.y
    "vmaddaz.xyzw ACC, vf6, vf8\n" // acc += vf6.xyzw * vf8.z
    "vmaddw.xyzw  vf9, vf7, vf8\n" // vf9 += vf7.xyzw * vf8.w
    "sqc2         vf9, 0x00(%0)\n" // store vf9 to m_[0][xyzw]

    // m_[1][xyzw] = m_[1][xyzw] * m
    "lqc2         vf8, 0x10(%0)\n" // load m_[1][xyzw] to vf8
    "vmulax.xyzw  ACC, vf4, vf8\n" // acc  = vf4.xyzw * vf8.x
    "vmadday.xyzw ACC, vf5, vf8\n" // acc += vf5.xyzw * vf8.y
    "vmaddaz.xyzw ACC, vf6, vf8\n" // acc += vf6.xyzw * vf8.z
    "vmaddw.xyzw  vf9, vf7, vf8\n" // vf9 += vf7.xyzw * vf8.w
    "sqc2         vf9, 0x10(%0)\n" // store vf9 to m_[1][xyzw]

    // m_[2][xyzw] = m_[2][xyzw] * m
    "lqc2         vf8, 0x20(%0)\n" // load m_[2][xyzw] to vf8
    "vmulax.xyzw  ACC, vf4, vf8\n" // acc  = vf4.xyzw * vf8.x
    "vmadday.xyzw ACC, vf5, vf8\n" // acc += vf5.xyzw * vf8.y
    "vmaddaz.xyzw ACC, vf6, vf8\n" // acc += vf6.xyzw * vf8.z
    "vmaddw.xyzw  vf9, vf7, vf8\n" // vf9 += vf7.xyzw * vf8.w
    "sqc2         vf9, 0x20(%0)\n" // store vf9 to m_[2][xyzw]

    // m_[3][xyzw] = m_[3][xyzw] * m
    "lqc2         vf8, 0x30(%0)\n" // load m_[3][xyzw] to vf8
    "vmulax.xyzw  ACC, vf4, vf8\n" // acc  = vf4.xyzw * vf8.x
    "vmadday.xyzw ACC, vf5, vf8\n" // acc += vf5.xyzw * vf8.y
    "vmaddaz.xyzw ACC, vf6, vf8\n" // acc += vf6.xyzw * vf8.z
    "vmaddw.xyzw  vf9, vf7, vf8\n" // vf9 += vf7.xyzw * vf8.w
    "sqc2         vf9, 0x30(%0)\n" // store vf9 to m_[3][xyzw]

    :
    : "r" (m_), "r" (m)
  );

  return *this;
}

//-----------------------------------------------------------------------------
void
CMatrix4x4::transform4(const GLfloat * from, GLfloat * to)
{
  asm __volatile__
  (
    "lqc2         vf4, 0x00(%0)\n" // load m_[0][xyzw] to vf4
    "lqc2         vf5, 0x10(%0)\n" // load m_[1][xyzw] to vf5
    "lqc2         vf6, 0x20(%0)\n" // load m_[2][xyzw] to vf6
    "lqc2         vf7, 0x30(%0)\n" // load m_[3][xyzw] to vf7
    "lqc2         vf8, 0x00(%1)\n" // load "from" to vf8
#ifdef ROW_MAJOR
    "vmula.xyzw   ACC, vf4, vf8\n" // acc  = vf4.xyzw * vf8
    "vmadda.xyzw  ACC, vf5, vf8\n" // acc += vf5.xyzw * vf8
    "vmadda.xyzw  ACC, vf6, vf8\n" // acc += vf6.xyzw * vf8
    "vmadd.xyzw   vf9, vf7, vf8\n" // vf9 += vf7.xyzw * vf8
#else
    "vmulax.xyzw  ACC, vf4, vf8\n" // acc  = vf4.xyzw * vf8.x
    "vmadday.xyzw ACC, vf5, vf8\n" // acc += vf5.xyzw * vf8.y
    "vmaddaz.xyzw ACC, vf6, vf8\n" // acc += vf6.xyzw * vf8.z
    "vmaddw.xyzw  vf9, vf7, vf8\n" // vf9 += vf7.xyzw * vf8.w
#endif
    "sqc2         vf9, 0x00(%2)\n" // store vf9 to "to"

    :
    : "r" (m_), "r" (from), "r" (to)
  );
}

//-----------------------------------------------------------------------------
void
CMatrix4x4::transform4list(const GLfloat * from, int fromInc, GLfloat * to, int toInc, int count)
{
  // Load m_ to vu0
  asm __volatile__
  (
    "lqc2         vf4, 0x00(%0)\n" // load m_[0][xyzw] to vf4
    "lqc2         vf5, 0x10(%0)\n" // load m_[1][xyzw] to vf5
    "lqc2         vf6, 0x20(%0)\n" // load m_[2][xyzw] to vf6
    "lqc2         vf7, 0x30(%0)\n" // load m_[3][xyzw] to vf7

    :
    : "r" (m_)
  );

  // Loop through all vertices
  while(count--)
  {
    // Convert vertex
    asm __volatile__
    (
      "lqc2         vf8, 0x00(%1)\n" // load "from" to vf8

#ifdef ROW_MAJOR
      "vmula.xyzw   ACC, vf4, vf8\n" // acc  = vf4.xyzw * vf8
      "vmadda.xyzw  ACC, vf5, vf8\n" // acc += vf5.xyzw * vf8
      "vmadda.xyzw  ACC, vf6, vf8\n" // acc += vf6.xyzw * vf8
      "vmadd.xyzw   vf9, vf7, vf8\n" // vf9 += vf7.xyzw * vf8
#else
      "vmulax.xyzw  ACC, vf4, vf8\n" // acc  = vf4.xyzw * vf8.x
      "vmadday.xyzw ACC, vf5, vf8\n" // acc += vf5.xyzw * vf8.y
      "vmaddaz.xyzw ACC, vf6, vf8\n" // acc += vf6.xyzw * vf8.z
      "vmaddw.xyzw  vf9, vf7, vf8\n" // vf9 += vf7.xyzw * vf8.w
#endif
      "sqc2         vf9, 0x00(%2)\n" // store vf9 to "to"

      :
      : "r" (m_), "r" (from), "r" (to)
    );

    // Advance to next vertex
    from += 4 * fromInc;
    to   += 4 * toInc;
  }
}


};
