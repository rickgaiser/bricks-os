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
CPS2Matrix::CPS2Matrix(const CPS2Matrix & m)
{
  // FIXME: This can go faster
  matrix[ 0] = m.matrix[ 0];
  matrix[ 1] = m.matrix[ 1];
  matrix[ 2] = m.matrix[ 2];
  matrix[ 3] = m.matrix[ 3];

  matrix[ 4] = m.matrix[ 4];
  matrix[ 5] = m.matrix[ 5];
  matrix[ 6] = m.matrix[ 6];
  matrix[ 7] = m.matrix[ 7];

  matrix[ 8] = m.matrix[ 8];
  matrix[ 9] = m.matrix[ 9];
  matrix[10] = m.matrix[10];
  matrix[11] = m.matrix[11];

  matrix[12] = m.matrix[12];
  matrix[13] = m.matrix[13];
  matrix[14] = m.matrix[14];
  matrix[15] = m.matrix[15];
}

//-----------------------------------------------------------------------------
CPS2Matrix::CPS2Matrix(const TMatrix4x4<GLfloat> & m)
{
  // FIXME: We need to use the right format from the start,
  //        This conversion makes it slow
  matrix[0*4+0] = m.matrix[0*4+0];
  matrix[0*4+1] = m.matrix[1*4+0];
  matrix[0*4+2] = m.matrix[2*4+0];
  matrix[0*4+3] = m.matrix[3*4+0];

  matrix[1*4+0] = m.matrix[0*4+1];
  matrix[1*4+1] = m.matrix[1*4+1];
  matrix[1*4+2] = m.matrix[2*4+1];
  matrix[1*4+3] = m.matrix[3*4+1];

  matrix[2*4+0] = m.matrix[0*4+2];
  matrix[2*4+1] = m.matrix[1*4+2];
  matrix[2*4+2] = m.matrix[2*4+2];
  matrix[2*4+3] = m.matrix[3*4+2];

  matrix[3*4+0] = m.matrix[0*4+3];
  matrix[3*4+1] = m.matrix[1*4+3];
  matrix[3*4+2] = m.matrix[2*4+3];
  matrix[3*4+3] = m.matrix[3*4+3];
}
