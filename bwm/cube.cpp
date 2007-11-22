#include "GLES/gl.h"
#include "../gl/fixedPoint.h"


extern const unsigned short crate_Width;
extern const unsigned short crate_Height;
extern const unsigned short crate_Bitmap[];
GLuint textures[1];


// -----------------------------------------------------------------------------
const GLfloat cubeVertF[] =
{
  // Cube (6 triangle strips)
  // Top
  -1.0f, 1.0f, 1.0f,
   1.0f, 1.0f, 1.0f,
  -1.0f, 1.0f,-1.0f,
   1.0f, 1.0f,-1.0f,
  // Bottom
  -1.0f,-1.0f,-1.0f,
   1.0f,-1.0f,-1.0f,
  -1.0f,-1.0f, 1.0f,
   1.0f,-1.0f, 1.0f,
  // Front
  -1.0f,-1.0f, 1.0f,
   1.0f,-1.0f, 1.0f,
  -1.0f, 1.0f, 1.0f,
   1.0f, 1.0f, 1.0f,
  // Back
  -1.0f, 1.0f,-1.0f,
   1.0f, 1.0f,-1.0f,
  -1.0f,-1.0f,-1.0f,
   1.0f,-1.0f,-1.0f,
  // Left
  -1.0f, 1.0f, 1.0f,
  -1.0f, 1.0f,-1.0f,
  -1.0f,-1.0f, 1.0f,
  -1.0f,-1.0f,-1.0f,
  // Right
   1.0f, 1.0f,-1.0f,
   1.0f, 1.0f, 1.0f,
   1.0f,-1.0f,-1.0f,
   1.0f,-1.0f, 1.0f,
};

// -----------------------------------------------------------------------------
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

// -----------------------------------------------------------------------------
const GLfloat cubeTexF[] =
{
  // Cube
  // Top
  0.0f, 0.0f,
  1.0f, 0.0f,
  0.0f, 1.0f,
  1.0f, 1.0f,
  // Bottom
  0.0f, 0.0f,
  1.0f, 0.0f,
  0.0f, 1.0f,
  1.0f, 1.0f,
  // Front
  0.0f, 0.0f,
  1.0f, 0.0f,
  0.0f, 1.0f,
  1.0f, 1.0f,
  // Back
  0.0f, 0.0f,
  1.0f, 0.0f,
  0.0f, 1.0f,
  1.0f, 1.0f,
  // Left
  0.0f, 0.0f,
  1.0f, 0.0f,
  0.0f, 1.0f,
  1.0f, 1.0f,
  // Right
  0.0f, 0.0f,
  1.0f, 0.0f,
  0.0f, 1.0f,
  1.0f, 1.0f,
};

// -----------------------------------------------------------------------------
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

// -----------------------------------------------------------------------------
void
initCubeF()
{
  static bool bInitialized(false);

  if(bInitialized == false)
  {
    bInitialized = true;

    // Texture
    glGenTextures(1, &textures[0]);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, crate_Width, crate_Height, 0, GL_RGB, GL_UNSIGNED_SHORT_5_5_5_1, crate_Bitmap);
    //glMatrixMode(GL_TEXTURE);
    //glLoadIdentity();
  }
}

// -----------------------------------------------------------------------------
void
initCubeFx()
{
  initCubeF();
}

// -----------------------------------------------------------------------------
void
drawCubeF()
{
  glVertexPointer(3, GL_FLOAT, 0, cubeVertF);
  glTexCoordPointer(2, GL_FLOAT, 0, cubeTexF);

  glEnableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, textures[0]);
  glDrawArrays(GL_TRIANGLE_STRIP,  0, 4); // Top
  glDrawArrays(GL_TRIANGLE_STRIP,  4, 4); // Bottom
  glDrawArrays(GL_TRIANGLE_STRIP,  8, 4); // Front
  glDrawArrays(GL_TRIANGLE_STRIP, 12, 4); // Back
  glDrawArrays(GL_TRIANGLE_STRIP, 16, 4); // Left
  glDrawArrays(GL_TRIANGLE_STRIP, 20, 4); // Right
  glDisable(GL_TEXTURE_2D);
}

// -----------------------------------------------------------------------------
void
drawCubeFx()
{
  glVertexPointer(3, GL_FIXED, 0, cubeVertFx);
  glTexCoordPointer(2, GL_FIXED, 0, cubeTexFx);

  glEnableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, textures[0]);
  glDrawArrays(GL_TRIANGLE_STRIP,  0, 4); // Top
  glDrawArrays(GL_TRIANGLE_STRIP,  4, 4); // Bottom
  glDrawArrays(GL_TRIANGLE_STRIP,  8, 4); // Front
  glDrawArrays(GL_TRIANGLE_STRIP, 12, 4); // Back
  glDrawArrays(GL_TRIANGLE_STRIP, 16, 4); // Left
  glDrawArrays(GL_TRIANGLE_STRIP, 20, 4); // Right
  glDisable(GL_TEXTURE_2D);
}
