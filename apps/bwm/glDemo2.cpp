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


#include "glDemo2.h"
#include "GLES/gl.h"
#include "GL/glu.h"
#include "../gl/context.h"


extern const unsigned short crate_Width;
extern const unsigned short crate_Height;
extern const unsigned short crate_Bitmap[];
GLuint textures[1];
const GLfixed lightAmbient[]  = {gl_fpfromf(0.5f), gl_fpfromf(0.5f), gl_fpfromf(0.5f), gl_fpfromf(1.0f)};
const GLfixed lightDiffuse[]  = {gl_fpfromf(1.0f), gl_fpfromf(1.0f), gl_fpfromf(1.0f), gl_fpfromf(1.0f)};
const GLfixed lightPosition[] = {gl_fpfromf(0.0f), gl_fpfromf(0.0f), gl_fpfromf(2.0f), gl_fpfromf(1.0f)};
const GLfixed fogColor[]      = {gl_fpfromf(0.5f), gl_fpfromf(0.5f), gl_fpfromf(0.5f), gl_fpfromf(1.0f)};

const GLfixed cubeVertFx[] =
{
  // Cube (6 triangle strips)
  // Top
  gl_fpfromf(-1.0f), gl_fpfromf( 1.0f), gl_fpfromf( 1.0f),
  gl_fpfromf( 1.0f), gl_fpfromf( 1.0f), gl_fpfromf( 1.0f),
  gl_fpfromf(-1.0f), gl_fpfromf( 1.0f), gl_fpfromf(-1.0f),
  gl_fpfromf( 1.0f), gl_fpfromf( 1.0f), gl_fpfromf(-1.0f),
  // Bottom
  gl_fpfromf(-1.0f), gl_fpfromf(-1.0f), gl_fpfromf(-1.0f),
  gl_fpfromf( 1.0f), gl_fpfromf(-1.0f), gl_fpfromf(-1.0f),
  gl_fpfromf(-1.0f), gl_fpfromf(-1.0f), gl_fpfromf( 1.0f),
  gl_fpfromf( 1.0f), gl_fpfromf(-1.0f), gl_fpfromf( 1.0f),
  // Front
  gl_fpfromf(-1.0f), gl_fpfromf(-1.0f), gl_fpfromf( 1.0f),
  gl_fpfromf( 1.0f), gl_fpfromf(-1.0f), gl_fpfromf( 1.0f),
  gl_fpfromf(-1.0f), gl_fpfromf( 1.0f), gl_fpfromf( 1.0f),
  gl_fpfromf( 1.0f), gl_fpfromf( 1.0f), gl_fpfromf( 1.0f),
  // Back
  gl_fpfromf(-1.0f), gl_fpfromf( 1.0f), gl_fpfromf(-1.0f),
  gl_fpfromf( 1.0f), gl_fpfromf( 1.0f), gl_fpfromf(-1.0f),
  gl_fpfromf(-1.0f), gl_fpfromf(-1.0f), gl_fpfromf(-1.0f),
  gl_fpfromf( 1.0f), gl_fpfromf(-1.0f), gl_fpfromf(-1.0f),
  // Left
  gl_fpfromf(-1.0f), gl_fpfromf( 1.0f), gl_fpfromf( 1.0f),
  gl_fpfromf(-1.0f), gl_fpfromf( 1.0f), gl_fpfromf(-1.0f),
  gl_fpfromf(-1.0f), gl_fpfromf(-1.0f), gl_fpfromf( 1.0f),
  gl_fpfromf(-1.0f), gl_fpfromf(-1.0f), gl_fpfromf(-1.0f),
  // Right
  gl_fpfromf( 1.0f), gl_fpfromf( 1.0f), gl_fpfromf(-1.0f),
  gl_fpfromf( 1.0f), gl_fpfromf( 1.0f), gl_fpfromf( 1.0f),
  gl_fpfromf( 1.0f), gl_fpfromf(-1.0f), gl_fpfromf(-1.0f),
  gl_fpfromf( 1.0f), gl_fpfromf(-1.0f), gl_fpfromf( 1.0f),
};

const GLfixed cubeTexFx[] =
{
  // Cube
  // Top
  gl_fpfromf(0.0f), gl_fpfromf(0.0f),
  gl_fpfromf(1.0f), gl_fpfromf(0.0f),
  gl_fpfromf(0.0f), gl_fpfromf(1.0f),
  gl_fpfromf(1.0f), gl_fpfromf(1.0f),
  // Bottom
  gl_fpfromf(0.0f), gl_fpfromf(0.0f),
  gl_fpfromf(1.0f), gl_fpfromf(0.0f),
  gl_fpfromf(0.0f), gl_fpfromf(1.0f),
  gl_fpfromf(1.0f), gl_fpfromf(1.0f),
  // Front
  gl_fpfromf(0.0f), gl_fpfromf(0.0f),
  gl_fpfromf(1.0f), gl_fpfromf(0.0f),
  gl_fpfromf(0.0f), gl_fpfromf(1.0f),
  gl_fpfromf(1.0f), gl_fpfromf(1.0f),
  // Back
  gl_fpfromf(0.0f), gl_fpfromf(0.0f),
  gl_fpfromf(1.0f), gl_fpfromf(0.0f),
  gl_fpfromf(0.0f), gl_fpfromf(1.0f),
  gl_fpfromf(1.0f), gl_fpfromf(1.0f),
  // Left
  gl_fpfromf(0.0f), gl_fpfromf(0.0f),
  gl_fpfromf(1.0f), gl_fpfromf(0.0f),
  gl_fpfromf(0.0f), gl_fpfromf(1.0f),
  gl_fpfromf(1.0f), gl_fpfromf(1.0f),
  // Right
  gl_fpfromf(0.0f), gl_fpfromf(0.0f),
  gl_fpfromf(1.0f), gl_fpfromf(0.0f),
  gl_fpfromf(0.0f), gl_fpfromf(1.0f),
  gl_fpfromf(1.0f), gl_fpfromf(1.0f),
};


//---------------------------------------------------------------------------
void
createMipMap(uint16_t * dest, uint16_t * source, uint16_t width, uint16_t height)
{
  uint16_t destWidth (width  >> 1);
  uint16_t destHeight(height >> 1);

  uint16_t p00, p01, p10, p11;
  uint8_t  r, g, b;

  for(uint16_t y(0); y < destHeight; y++)
  {
    for(uint16_t x(0); x < destWidth; x++)
    {
      p00 = source[((y << 1)    ) * width + (x << 1)    ];
      p01 = source[((y << 1)    ) * width + (x << 1) + 1];
      p10 = source[((y << 1) + 1) * width + (x << 1)    ];
      p11 = source[((y << 1) + 1) * width + (x << 1) + 1];

      r   = (BxColorFormat_GetR(cfX1R5G5B5, p00) +
             BxColorFormat_GetR(cfX1R5G5B5, p01) +
             BxColorFormat_GetR(cfX1R5G5B5, p10) +
             BxColorFormat_GetR(cfX1R5G5B5, p11)) >> 2;

      g   = (BxColorFormat_GetG(cfX1R5G5B5, p00) +
             BxColorFormat_GetG(cfX1R5G5B5, p01) +
             BxColorFormat_GetG(cfX1R5G5B5, p10) +
             BxColorFormat_GetG(cfX1R5G5B5, p11)) >> 2;

      b   = (BxColorFormat_GetB(cfX1R5G5B5, p00) +
             BxColorFormat_GetB(cfX1R5G5B5, p01) +
             BxColorFormat_GetB(cfX1R5G5B5, p10) +
             BxColorFormat_GetB(cfX1R5G5B5, p11)) >> 2;

      dest[y * destWidth + x] = BxColorFormat_FromRGB(cfX1R5G5B5, r, g, b);
    }
  }
}

//---------------------------------------------------------------------------
CGLDemo2::CGLDemo2(CWidget * parent)
 : CGLWidget(parent)
 , yrot_(gl_fpfromi(0))
{
}

//---------------------------------------------------------------------------
CGLDemo2::~CGLDemo2()
{
}

//---------------------------------------------------------------------------
void
CGLDemo2::initializeGL()
{
  // Background color
  glClearColorx(fogColor[0], fogColor[1], fogColor[2], fogColor[3]);

  // Depth test
  glClearDepthx(gl_fpfromi(1));
  glDepthFunc(GL_LEQUAL);
  //glEnable(GL_DEPTH_TEST);

  // Backface culling
  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);

  // Shade model
  glShadeModel(/*GL_FLAT*/GL_SMOOTH);

  // Lighting
  //glLightxv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
  //glLightxv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
  //glLightxv(GL_LIGHT0, GL_POSITION, lightPosition);
  //glEnable(GL_LIGHT0);
  //glEnable(GL_LIGHTING);

  // Fog
  glFogxv(GL_FOG_COLOR, fogColor);
  glFogx(GL_FOG_DENSITY, gl_fpfromf(0.35f));
  glFogx(GL_FOG_START, gl_fpfromi(1));
  glFogx(GL_FOG_END, gl_fpfromi(10));
  //glEnable(GL_FOG);

  // Texture
  glGenTextures(1, &textures[0]);
  glBindTexture(GL_TEXTURE_2D, textures[0]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, crate_Width, crate_Height, 0, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, crate_Bitmap);
  //glMatrixMode(GL_TEXTURE);
  //glLoadIdentity();

  glMatrixMode(GL_MODELVIEW);

  glVertexPointer(3, GL_FIXED, 0, cubeVertFx);
  glTexCoordPointer(2, GL_FIXED, 0, cubeTexFx);

  glEnableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
}

//---------------------------------------------------------------------------
void
CGLDemo2::resizeGL(int w, int h)
{
  // Viewport & Perspective
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0f, (float)w / (float)h, 0.1f, 100.0f);

  // Move up a little
  glTranslatex(gl_fpfromi(0), gl_fpfromi(-2), gl_fpfromi(0));
  // Look down a little
  glRotatex(gl_fpfromi(23), gl_fpfromi(1), gl_fpfromi(0), gl_fpfromi(0));

  glMatrixMode(GL_MODELVIEW);
}

//---------------------------------------------------------------------------
void
CGLDemo2::drawGL()
{
  glClear(GL_COLOR_BUFFER_BIT);

  glLoadIdentity();
  glTranslatex(gl_fpfromi(0), gl_fpfromi(0), gl_fpfromi(-6));
  glRotatex(yrot_, gl_fpfromi(0), gl_fpfromi(1), gl_fpfromi(0));

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, textures[0]);
  glDrawArrays(GL_TRIANGLE_STRIP,  0, 4); // Top
  glDrawArrays(GL_TRIANGLE_STRIP,  4, 4); // Bottom
  glDrawArrays(GL_TRIANGLE_STRIP,  8, 4); // Front
  glDrawArrays(GL_TRIANGLE_STRIP, 12, 4); // Back
  glDrawArrays(GL_TRIANGLE_STRIP, 16, 4); // Left
  glDrawArrays(GL_TRIANGLE_STRIP, 20, 4); // Right
  glDisable(GL_TEXTURE_2D);

  // Flush everything to surface
  glFlush();

  yrot_ += gl_fpfromi(2);
  if(yrot_ >= gl_fpfromi(360))
    yrot_ -= gl_fpfromi(360);
}
