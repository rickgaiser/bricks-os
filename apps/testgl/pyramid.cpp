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


#include "GL/gl.h"
#include "GLES/gl.h"
#include "vhl/fixedPoint.h"
#include "vhl/vector.h"
#include "glconfig.h"


#define TRIANGLE_COUNT 4


// -----------------------------------------------------------------------------
const GLfloat pyramidVertF[TRIANGLE_COUNT*3*3] =
{
  // Pyramid (triangles)
   0.0f,  1.0f,  0.0f,
  -1.0f, -1.0f,  1.0f,
   1.0f, -1.0f,  1.0f,

   0.0f,  1.0f,  0.0f,
   1.0f, -1.0f,  1.0f,
   1.0f, -1.0f, -1.0f,

   0.0f,  1.0f,  0.0f,
   1.0f, -1.0f, -1.0f,
  -1.0f, -1.0f, -1.0f,

   0.0f,  1.0f,  0.0f,
  -1.0f, -1.0f, -1.0f,
  -1.0f, -1.0f,  1.0f,
};

// -----------------------------------------------------------------------------
const GLfloat pyramidColF[TRIANGLE_COUNT*3*4] =
{
  // Pyramid
  1.0f, 0.0f, 0.0f, 1.0f,
  0.0f, 1.0f, 0.0f, 1.0f,
  0.0f, 0.0f, 1.0f, 1.0f,

  1.0f, 0.0f, 0.0f, 1.0f,
  0.0f, 0.0f, 1.0f, 1.0f,
  0.0f, 1.0f, 0.0f, 1.0f,

  1.0f, 0.0f, 0.0f, 1.0f,
  0.0f, 1.0f, 0.0f, 1.0f,
  0.0f, 0.0f, 1.0f, 1.0f,

  1.0f, 0.0f, 0.0f, 1.0f,
  0.0f, 0.0f, 1.0f, 1.0f,
  0.0f, 1.0f, 0.0f, 1.0f,
};

// -----------------------------------------------------------------------------
const GLfixed pyramidVertFx[TRIANGLE_COUNT*3*3] =
{
  // Pyramid (triangles)
  gl_fpfromf( 0.0f), gl_fpfromf( 1.0f), gl_fpfromf( 0.0f),
  gl_fpfromf(-1.0f), gl_fpfromf(-1.0f), gl_fpfromf( 1.0f),
  gl_fpfromf( 1.0f), gl_fpfromf(-1.0f), gl_fpfromf( 1.0f),

  gl_fpfromf( 0.0f), gl_fpfromf( 1.0f), gl_fpfromf( 0.0f),
  gl_fpfromf( 1.0f), gl_fpfromf(-1.0f), gl_fpfromf( 1.0f),
  gl_fpfromf( 1.0f), gl_fpfromf(-1.0f), gl_fpfromf(-1.0f),

  gl_fpfromf( 0.0f), gl_fpfromf( 1.0f), gl_fpfromf( 0.0f),
  gl_fpfromf( 1.0f), gl_fpfromf(-1.0f), gl_fpfromf(-1.0f),
  gl_fpfromf(-1.0f), gl_fpfromf(-1.0f), gl_fpfromf(-1.0f),

  gl_fpfromf( 0.0f), gl_fpfromf( 1.0f), gl_fpfromf( 0.0f),
  gl_fpfromf(-1.0f), gl_fpfromf(-1.0f), gl_fpfromf(-1.0f),
  gl_fpfromf(-1.0f), gl_fpfromf(-1.0f), gl_fpfromf( 1.0f)
};

// -----------------------------------------------------------------------------
const GLfixed pyramidColFx[TRIANGLE_COUNT*3*4] =
{
  // Pyramid
  gl_fpfromf(1.0f), gl_fpfromf(0.0f), gl_fpfromf(0.0f), gl_fpfromf(1.0f),
  gl_fpfromf(0.0f), gl_fpfromf(1.0f), gl_fpfromf(0.0f), gl_fpfromf(1.0f),
  gl_fpfromf(0.0f), gl_fpfromf(0.0f), gl_fpfromf(1.0f), gl_fpfromf(1.0f),

  gl_fpfromf(1.0f), gl_fpfromf(0.0f), gl_fpfromf(0.0f), gl_fpfromf(1.0f),
  gl_fpfromf(0.0f), gl_fpfromf(0.0f), gl_fpfromf(1.0f), gl_fpfromf(1.0f),
  gl_fpfromf(0.0f), gl_fpfromf(1.0f), gl_fpfromf(0.0f), gl_fpfromf(1.0f),

  gl_fpfromf(1.0f), gl_fpfromf(0.0f), gl_fpfromf(0.0f), gl_fpfromf(1.0f),
  gl_fpfromf(0.0f), gl_fpfromf(1.0f), gl_fpfromf(0.0f), gl_fpfromf(1.0f),
  gl_fpfromf(0.0f), gl_fpfromf(0.0f), gl_fpfromf(1.0f), gl_fpfromf(1.0f),

  gl_fpfromf(1.0f), gl_fpfromf(0.0f), gl_fpfromf(0.0f), gl_fpfromf(1.0f),
  gl_fpfromf(0.0f), gl_fpfromf(0.0f), gl_fpfromf(1.0f), gl_fpfromf(1.0f),
  gl_fpfromf(0.0f), gl_fpfromf(1.0f), gl_fpfromf(0.0f), gl_fpfromf(1.0f),
};


#ifdef ENABLE_LIGHTING
GLfloat pyramidNormalF[TRIANGLE_COUNT*3*3];
#endif
// -----------------------------------------------------------------------------
void
initPyramidF()
{
#ifdef ENABLE_LIGHTING
  // Precalculated normals
  TVector3<GLfloat> V[3];
  TVector3<GLfloat> normal;
  for(int i(0); i < TRIANGLE_COUNT; i++)
  {
    // Load vetices V0, V1 and V2
    for(int v(0); v < 3; v++)
    {
      V[v].x = pyramidVertF[i*3*3+(v*3)+0];
      V[v].y = pyramidVertF[i*3*3+(v*3)+1];
      V[v].z = pyramidVertF[i*3*3+(v*3)+2];
    }

    // Calculate normal
    normal = (V[0] - V[1]).getCrossProduct(V[2] - V[1]);
    normal.normalize();

    // Store normal for V0, V1 and V2
    for(int v(0); v < 3; v++)
    {
      pyramidNormalF[i*3*3+(v*3)+0] = normal.x;
      pyramidNormalF[i*3*3+(v*3)+1] = normal.y;
      pyramidNormalF[i*3*3+(v*3)+2] = normal.z;
    }
  }
#endif
}

// -----------------------------------------------------------------------------
void
drawPyramidF()
{
  glVertexPointer(3, GL_FLOAT, 0, pyramidVertF);
  glColorPointer (4, GL_FLOAT, 0, pyramidColF);
#ifdef ENABLE_LIGHTING
  glNormalPointer(   GL_FLOAT, 0, pyramidNormalF);
#endif

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
#ifdef ENABLE_LIGHTING
  glEnableClientState(GL_NORMAL_ARRAY);
#else
  glDisableClientState(GL_NORMAL_ARRAY);
#endif

  glDrawArrays(GL_TRIANGLES, 0, TRIANGLE_COUNT*3); // Pyramid
}

#ifdef ENABLE_LIGHTING
GLfixed pyramidNormalFx[TRIANGLE_COUNT*3*3];
#endif
// -----------------------------------------------------------------------------
void
initPyramidFx()
{
#ifdef ENABLE_LIGHTING
  TVector3<CFixed> V0;
  TVector3<CFixed> V1;
  TVector3<CFixed> V2;
  TVector3<CFixed> normal;

  for(int i(0); i < TRIANGLE_COUNT; i++)
  {
    int iBase = i * 3 * 3;

    // Load vetices V0, V1 and V2
    // V0
    V0.x.value = pyramidVertFx[iBase+0];
    V0.y.value = pyramidVertFx[iBase+1];
    V0.z.value = pyramidVertFx[iBase+2];
    // V1
    V1.x.value = pyramidVertFx[iBase+3];
    V1.y.value = pyramidVertFx[iBase+4];
    V1.z.value = pyramidVertFx[iBase+5];
    // V2
    V2.x.value = pyramidVertFx[iBase+6];
    V2.y.value = pyramidVertFx[iBase+7];
    V2.z.value = pyramidVertFx[iBase+8];

    // Calculate normal
    normal = (V0 - V1).getCrossProduct(V2 - V1);
    normal.normalize();

    // Store normals to V0, V1 and V2
    // V0
    pyramidNormalFx[iBase+0] = normal.x.value;
    pyramidNormalFx[iBase+1] = normal.y.value;
    pyramidNormalFx[iBase+2] = normal.z.value;
    // V1
    pyramidNormalFx[iBase+3] = normal.x.value;
    pyramidNormalFx[iBase+4] = normal.y.value;
    pyramidNormalFx[iBase+5] = normal.z.value;
    // V2
    pyramidNormalFx[iBase+6] = normal.x.value;
    pyramidNormalFx[iBase+7] = normal.y.value;
    pyramidNormalFx[iBase+8] = normal.z.value;
  }
#endif
}

// -----------------------------------------------------------------------------
void
drawPyramidFx()
{
  glVertexPointer(3, GL_FIXED, 0, pyramidVertFx);
  glColorPointer (4, GL_FIXED, 0, pyramidColFx);
#ifdef ENABLE_LIGHTING
  glNormalPointer(   GL_FIXED, 0, pyramidNormalFx);
#endif

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
#ifdef ENABLE_LIGHTING
  glEnableClientState(GL_NORMAL_ARRAY);
#else
  glDisableClientState(GL_NORMAL_ARRAY);
#endif

  glDrawArrays(GL_TRIANGLES, 0, TRIANGLE_COUNT*3); // Pyramid
}
