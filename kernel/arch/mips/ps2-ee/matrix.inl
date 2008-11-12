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


#include "matrix.h"


//-----------------------------------------------------------------------------
void
CPS2Matrix::transform(const GLfloat * v1, GLfloat * v2)
{
  asm __volatile__
  (
    "lqc2 vf4,0x0(%0)\n"
    "lqc2 vf5,0x10(%0)\n"
    "lqc2 vf6,0x20(%0)\n"
    "lqc2 vf7,0x30(%0)\n"
    "lqc2 vf8,0x0(%2)\n"
    "vmulax.xyzw ACC, vf4,vf8\n"
    "vmadday.xyzw ACC, vf5,vf8\n"
    "vmaddaz.xyzw ACC, vf6,vf8\n"
    "vmaddw.xyzw vf9,vf7,vf8\n"
    "sqc2 vf9,0x0(%1)\n"
    :
    : "r" (matrix), "r" (v1), "r" (v2)
  );
}
