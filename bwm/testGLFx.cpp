#include "GLES/gl.h"
#include "GLES/gl_extra.h"
#include "kernel/videoManager.h"
#include "../gl/fixedPoint.h"


const GLfixed lightAmbient[] = {gl_fpfromf(0.1f), gl_fpfromf(0.1f), gl_fpfromf(0.1f), gl_fpfromf(1.0f)};
const GLfixed lightDiffuse[] = {gl_fpfromf(1.0f), gl_fpfromf(1.0f), gl_fpfromf(1.0f), gl_fpfromf(1.0f)};
const GLfixed fogColor[]     = {gl_fpfromf(0.5f), gl_fpfromf(0.5f), gl_fpfromf(0.5f), gl_fpfromf(0.5f)};

const GLfloat triangle[] =
{
  // Front
   0.0f,  1.0f,  0.0f,
  -1.0f, -1.0f,  1.0f,
   1.0f, -1.0f,  1.0f,
  // Right
   0.0f,  1.0f,  0.0f,
   1.0f, -1.0f,  1.0f,
   1.0f, -1.0f, -1.0f,
  // Back
   0.0f,  1.0f,  0.0f,
   1.0f, -1.0f, -1.0f,
  -1.0f, -1.0f, -1.0f,
  // Left
   0.0f,  1.0f,  0.0f,
  -1.0f, -1.0f, -1.0f,
  -1.0f, -1.0f,  1.0f
};
const GLint triangle_count(4);

const GLfloat colors[] =
{
  // Front
  1.0f, 0.0f, 0.0f, 1.0f,
  0.0f, 1.0f, 0.0f, 1.0f,
  0.0f, 0.0f, 1.0f, 1.0f,
  // Right
  1.0f, 0.0f, 0.0f, 1.0f,
  0.0f, 0.0f, 1.0f, 1.0f,
  0.0f, 1.0f, 0.0f, 1.0f,
  // Back
  1.0f, 0.0f, 0.0f, 1.0f,
  0.0f, 1.0f, 0.0f, 1.0f,
  0.0f, 0.0f, 1.0f, 1.0f,
  // Left
  1.0f, 0.0f, 0.0f, 1.0f,
  0.0f, 0.0f, 1.0f, 1.0f,
  0.0f, 1.0f, 0.0f, 1.0f
};

const GLfloat normals[] =
{
  // Front
  0.0f, 0.0f, 1.0f,
  0.0f, 0.0f, 1.0f,
  0.0f, 0.0f, 1.0f,
  // Right
  1.0f, 0.0f, 0.0f,
  1.0f, 0.0f, 0.0f,
  1.0f, 0.0f, 0.0f,
  // Back
  0.0f, 0.0f,-1.0f,
  0.0f, 0.0f,-1.0f,
  0.0f, 0.0f,-1.0f,
  // Left
  -1.0f, 0.0f, 0.0f,
  -1.0f, 0.0f, 0.0f,
  -1.0f, 0.0f, 0.0f
};


// -----------------------------------------------------------------------------
void
testGLFx(CSurface * surface)
{
  // Initialize GL
  // Background color
  glClearColorx(fogColor[0], fogColor[1], fogColor[2], fogColor[3]);
  // Depth test
  glClearDepthx(gl_fpfromf(1.0f));
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
  glLightxv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
  glLightxv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHTING);
  // Fog
  glFogxv(GL_FOG_COLOR, fogColor);
  glFogx(GL_FOG_DENSITY, gl_fpfromf(0.35f));
  glFogx(GL_FOG_START, gl_fpfromf(1.0f));
  glFogx(GL_FOG_END, gl_fpfromf(10.0f));
  glEnable(GL_FOG);
  // Pointers to data
  glVertexPointer(3, GL_FLOAT, 0, triangle);
  glColorPointer(4, GL_FLOAT, 0, colors);
  glNormalPointer(GL_FLOAT, 0, normals);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  // Move up a little
//  glTranslatef(0.0f, -2.0f, 0.0f);
  // Look down a little
//  glRotatef(-30.0f, 1.0f, 0.0f, 0.0f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Show Pyramid for 1 full rotation around y axis
  for(GLfixed yrot = gl_fpfromi(0); yrot < gl_fpfromi(360); yrot += gl_fpfromi(2))
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    glTranslatex(gl_fpfromi(0), gl_fpfromi(0), gl_fpfromi(-4));
    glRotatex(yrot, gl_fpfromi(0), gl_fpfromi(1), gl_fpfromi(0));
    glDrawArrays(GL_TRIANGLES, 0, triangle_count);

    glLoadIdentity();
    glTranslatex(gl_fpfromi(-2), gl_fpfromi(0), gl_fpfromi(-7));
    glRotatex(yrot, gl_fpfromi(0), gl_fpfromi(1), gl_fpfromi(0));
    glDrawArrays(GL_TRIANGLES, 0, triangle_count);

    glLoadIdentity();
    glTranslatex(gl_fpfromi(2), gl_fpfromi(0), gl_fpfromi(-7));
    glRotatex(yrot, gl_fpfromi(0), gl_fpfromi(1), gl_fpfromi(0));
    glDrawArrays(GL_TRIANGLES, 0, triangle_count);

    glFlush();

    // Display progress bar
    surface->setFillColor(255, 255, 255);
    surface->fillRect(1, surface->height() - 12, surface->width() - 2, 10);
    surface->setFillColor(0, 0, 0);
    surface->fillRect(3, surface->height() - 10, ((surface->width() - 6) * gl_fptoi(yrot)) / 360, 6);

    surface->swap(true);
  }
}
