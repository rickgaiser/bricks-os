#include "GLES/gl.h"
#include "GLES/gl_extra.h"
#include "GL/glu.h"
#include "kernel/videoManager.h"


const GLfloat lightAmbient[] = {0.1f, 0.1f, 0.1f, 1.0f};
const GLfloat lightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
const GLfloat fogColor[]     = {0.5f, 0.5f, 0.5f, 0.5f};

const GLfloat triangle[] =
{
  // Square (strip)
  -1.0f, -1.5f,  1.0f,
   1.0f, -1.5f,  1.0f,
  -1.0f, -1.5f, -1.0f,
   1.0f, -1.5f, -1.0f,

  // Pyramid (fan)
   0.0f,  1.0f,  0.0f,
  -1.0f, -1.0f,  1.0f,
   1.0f, -1.0f,  1.0f,
   1.0f, -1.0f, -1.0f,
  -1.0f, -1.0f, -1.0f,
  -1.0f, -1.0f,  1.0f
};
const GLint square_vcount(4);
const GLint pyramid_vcount(6);

const GLfloat colors[] =
{
  // Square
  0.4f, 0.4f, 0.4f, 1.0f,
  0.4f, 0.4f, 0.4f, 1.0f,
  0.4f, 0.4f, 0.4f, 1.0f,
  0.4f, 0.4f, 0.4f, 1.0f,

  // Pyramid
  1.0f, 0.0f, 0.0f, 1.0f,
  0.0f, 1.0f, 0.0f, 1.0f,
  0.0f, 0.0f, 1.0f, 1.0f,
  0.0f, 1.0f, 0.0f, 1.0f,
  0.0f, 0.0f, 1.0f, 1.0f,
  0.0f, 1.0f, 0.0f, 1.0f
};

const GLfloat normals[] =
{
  // Square
   0.0f, 1.0f, 0.0f,
   0.0f, 1.0f, 0.0f,
   0.0f, 1.0f, 0.0f,
   0.0f, 1.0f, 0.0f,

  // Pyramid
   0.0f, 1.0f, 0.0f,
  -1.0f, 0.0f, 1.0f,
   1.0f, 0.0f, 1.0f,
   1.0f, 0.0f,-1.0f,
  -1.0f, 0.0f,-1.0f,
  -1.0f, 0.0f, 1.0f
};


// -----------------------------------------------------------------------------
void
testGLF(CSurface * surface)
{
  // Background color
  glClearColor(fogColor[0], fogColor[1], fogColor[2], fogColor[3]);
  
  // Depth test
  glClearDepthf(1.0f);
  glDepthFunc(GL_LEQUAL);
  //glEnable(GL_DEPTH_TEST);
  
  // Backface culling
  glCullFace(GL_BACK);
  //glEnable(GL_CULL_FACE);
  
  // Shade model
  glShadeModel(/*GL_FLAT*/GL_SMOOTH);
  
  // Lighting
  //glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
  //glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
  //glEnable(GL_LIGHT0);
  //glEnable(GL_LIGHTING);
  
  // Fog
  glFogfv(GL_FOG_COLOR, fogColor);
  glFogf(GL_FOG_DENSITY, 0.35f);
  glFogf(GL_FOG_START, 1.0f);
  glFogf(GL_FOG_END, 10.0f);
  //glEnable(GL_FOG);
  
  // Pointers to data
  glVertexPointer(3, GL_FLOAT, 0, triangle);
  glColorPointer(4, GL_FLOAT, 0, colors);
  //glNormalPointer(GL_FLOAT, 0, normals);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  //glEnableClientState(GL_NORMAL_ARRAY);

  // Viewport & Perspective
  glViewport(0, 0, surface->width(), surface->height());
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0f, (float)surface->width() / (float)surface->height(), 0.1f, 100.0f);
  
  // Move up a little
  glTranslatef(0.0f, -2.0f, 0.0f);
  // Look down a little
  glRotatef(23.0f, 1.0f, 0.0f, 0.0f);
  glMatrixMode(GL_MODELVIEW);

  // Show Pyramid for 1 full rotation around y axis
  for(GLfloat yrot(0.0f); yrot < 360.0f; yrot += 2.0f)
  {
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT);

    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -6.0f);
    glRotatef(yrot, 0.0f, 1.0f, 0.0f);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, square_vcount);
    glDrawArrays(GL_TRIANGLE_FAN, square_vcount, pyramid_vcount);
/*
    glLoadIdentity();
    glTranslatef(-2.0f, 0.0f, -8.0f);
    glRotatef(yrot, 0.0f, 1.0f, 0.0f);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, square_vcount);
    glDrawArrays(GL_TRIANGLE_FAN, square_vcount, pyramid_vcount);

    glLoadIdentity();
    glTranslatef(2.0f, 0.0f, -8.0f);
    glRotatef(yrot, 0.0f, 1.0f, 0.0f);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, square_vcount);
    glDrawArrays(GL_TRIANGLE_FAN, square_vcount, pyramid_vcount);
*/
    glFlush();

    // Display progress bar
    //surface->setFillColor(255, 255, 255);
    //surface->fillRect(1, surface->height() - 12, surface->width() - 2, 10);
    //surface->setFillColor(0, 0, 0);
    //surface->fillRect(3, surface->height() - 10, ((surface->width() - 6) * static_cast<int>(yrot)) / 360, 6);

    surface->swap(true);
  }
}
