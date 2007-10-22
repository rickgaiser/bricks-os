#include "GLES/gl.h"
#include "GLES/gl_extra.h"
#include "kernel/videoManager.h"
#include "../gl/fixedPoint.h"


const GLfixed lightAmbient[] = {gl_fpfromf(0.1f), gl_fpfromf(0.1f), gl_fpfromf(0.1f), gl_fpfromf(1.0f)};
const GLfixed lightDiffuse[] = {gl_fpfromf(1.0f), gl_fpfromf(1.0f), gl_fpfromf(1.0f), gl_fpfromf(1.0f)};
const GLfixed fogColor[]     = {gl_fpfromf(0.5f), gl_fpfromf(0.5f), gl_fpfromf(0.5f), gl_fpfromf(0.5f)};

const GLfixed triangle[] =
{
  // Square (strip)
  gl_fpfromf(-1.0f), gl_fpfromf(-1.5f), gl_fpfromf( 1.0f),
  gl_fpfromf( 1.0f), gl_fpfromf(-1.5f), gl_fpfromf( 1.0f),
  gl_fpfromf(-1.0f), gl_fpfromf(-1.5f), gl_fpfromf(-1.0f),
  gl_fpfromf( 1.0f), gl_fpfromf(-1.5f), gl_fpfromf(-1.0f),

  // Pyramid (fan)
  gl_fpfromf( 0.0f), gl_fpfromf( 1.0f), gl_fpfromf( 0.0f),
  gl_fpfromf(-1.0f), gl_fpfromf(-1.0f), gl_fpfromf( 1.0f),
  gl_fpfromf( 1.0f), gl_fpfromf(-1.0f), gl_fpfromf( 1.0f),
  gl_fpfromf( 1.0f), gl_fpfromf(-1.0f), gl_fpfromf(-1.0f),
  gl_fpfromf(-1.0f), gl_fpfromf(-1.0f), gl_fpfromf(-1.0f),
  gl_fpfromf(-1.0f), gl_fpfromf(-1.0f), gl_fpfromf( 1.0f)
};
const GLint square_vcount(4);
const GLint pyramid_vcount(6);

const GLfixed colors[] =
{
  // Square
  gl_fpfromf(0.2f), gl_fpfromf(0.2f), gl_fpfromf(0.2f), gl_fpfromf(1.0f),
  gl_fpfromf(0.2f), gl_fpfromf(0.2f), gl_fpfromf(0.2f), gl_fpfromf(1.0f),
  gl_fpfromf(0.2f), gl_fpfromf(0.2f), gl_fpfromf(0.2f), gl_fpfromf(1.0f),
  gl_fpfromf(0.2f), gl_fpfromf(0.2f), gl_fpfromf(0.2f), gl_fpfromf(1.0f),

  // Pyramid
  gl_fpfromf(1.0f), gl_fpfromf(0.0f), gl_fpfromf(0.0f), gl_fpfromf(1.0f),
  gl_fpfromf(0.0f), gl_fpfromf(1.0f), gl_fpfromf(0.0f), gl_fpfromf(1.0f),
  gl_fpfromf(0.0f), gl_fpfromf(0.0f), gl_fpfromf(1.0f), gl_fpfromf(1.0f),
  gl_fpfromf(0.0f), gl_fpfromf(1.0f), gl_fpfromf(0.0f), gl_fpfromf(1.0f),
  gl_fpfromf(0.0f), gl_fpfromf(0.0f), gl_fpfromf(1.0f), gl_fpfromf(1.0f),
  gl_fpfromf(0.0f), gl_fpfromf(1.0f), gl_fpfromf(0.0f), gl_fpfromf(1.0f)
};

const GLfixed normals[] =
{
  // Square
  gl_fpfromf( 0.0f), gl_fpfromf(1.0f), gl_fpfromf( 0.0f),
  gl_fpfromf( 0.0f), gl_fpfromf(1.0f), gl_fpfromf( 0.0f),
  gl_fpfromf( 0.0f), gl_fpfromf(1.0f), gl_fpfromf( 0.0f),
  gl_fpfromf( 0.0f), gl_fpfromf(1.0f), gl_fpfromf( 0.0f),

  // Pyramid
  gl_fpfromf( 0.0f), gl_fpfromf(1.0f), gl_fpfromf( 0.0f),
  gl_fpfromf(-1.0f), gl_fpfromf(0.0f), gl_fpfromf( 1.0f),
  gl_fpfromf( 1.0f), gl_fpfromf(0.0f), gl_fpfromf( 1.0f),
  gl_fpfromf( 1.0f), gl_fpfromf(0.0f), gl_fpfromf(-1.0f),
  gl_fpfromf(-1.0f), gl_fpfromf(0.0f), gl_fpfromf(-1.0f),
  gl_fpfromf(-1.0f), gl_fpfromf(0.0f), gl_fpfromf( 1.0f)
};


// -----------------------------------------------------------------------------
void
testGLFx(CSurface * surface)
{
  // Initialize GL
  // Background color
  glClearColorx(fogColor[0], fogColor[1], fogColor[2], fogColor[3]);
  // Depth test
  glClearDepthx(gl_fpfromi(1));
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_DEPTH_TEST);
  // Backface culling
  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);
  // Shade model
  glShadeModel(/*GL_FLAT*/GL_SMOOTH);
  // Viewport & Perspective
  glViewport(0, 0, surface->width(), surface->height());
//  gluPerspective(45.0f, (float)surface->width() / (float)surface->height(), 0.1f, 100.0f);
  // Lighting
  //glLightxv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
  //glLightxv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
  //glEnable(GL_LIGHT0);
  //glEnable(GL_LIGHTING);
  // Fog
  glFogxv(GL_FOG_COLOR, fogColor);
  glFogx(GL_FOG_DENSITY, gl_fpfromf(0.35f));
  glFogx(GL_FOG_START, gl_fpfromi(1));
  glFogx(GL_FOG_END, gl_fpfromi(10));
  glEnable(GL_FOG);
  // Pointers to data
  glVertexPointer(3, GL_FIXED, 0, triangle);
  glColorPointer(4, GL_FIXED, 0, colors);
  //glNormalPointer(GL_FIXED, 0, normals);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  //glEnableClientState(GL_NORMAL_ARRAY);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  GLfloat fRatio = (float)surface->height() / (float)surface->width();
  glFrustumx(gl_fpfromf(0.5f), gl_fpfromf(-0.5f), gl_fpfromf(-0.5f * fRatio), gl_fpfromf(0.5f * fRatio), gl_fpfromf(0.5f), gl_fpfromf(100.0f));
  // Move up a little
  //glTranslatex(gl_fpfromi(0), gl_fpfromi(-2), gl_fpfromi(0));
  // Look down a little
  //glRotatex(gl_fpfromi(-30), gl_fpfromi(1), gl_fpfromi(0), gl_fpfromi(0));
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Show Pyramid for 1 full rotation around y axis
  for(GLfixed yrot = gl_fpfromi(0); yrot < gl_fpfromi(360); yrot += gl_fpfromi(2))
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    glTranslatex(gl_fpfromi(0), gl_fpfromi(0), gl_fpfromi(-6));
    glRotatex(yrot, gl_fpfromi(0), gl_fpfromi(1), gl_fpfromi(0));
    glDrawArrays(GL_TRIANGLE_STRIP, 0, square_vcount);
    glDrawArrays(GL_TRIANGLE_FAN, square_vcount, pyramid_vcount);
/*
    glLoadIdentity();
    glTranslatex(gl_fpfromi(-2), gl_fpfromi(0), gl_fpfromi(-8));
    glRotatex(yrot, gl_fpfromi(0), gl_fpfromi(1), gl_fpfromi(0));
    glDrawArrays(GL_TRIANGLE_STRIP, 0, square_vcount);
    glDrawArrays(GL_TRIANGLE_FAN, square_vcount, pyramid_vcount);

    glLoadIdentity();
    glTranslatex(gl_fpfromi(2), gl_fpfromi(0), gl_fpfromi(-8));
    glRotatex(yrot, gl_fpfromi(0), gl_fpfromi(1), gl_fpfromi(0));
    glDrawArrays(GL_TRIANGLE_STRIP, 0, square_vcount);
    glDrawArrays(GL_TRIANGLE_FAN, square_vcount, pyramid_vcount);
*/
    glFlush();

    // Display progress bar
    surface->setFillColor(255, 255, 255);
    surface->fillRect(1, surface->height() - 12, surface->width() - 2, 10);
    surface->setFillColor(0, 0, 0);
    surface->fillRect(3, surface->height() - 10, ((surface->width() - 6) * gl_fptoi(yrot)) / 360, 6);

    surface->swap(true);
  }
}
