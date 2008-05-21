#include "GLES/gl.h"
#include "GLES/gl_extra.h"
#include "GL/glu.h"
#include "kernel/videoManager.h"
#include "../gl/context.h"


extern void glMakeCurrent(I3DRenderer * ctx);
extern void initPyramidF();
extern void drawPyramidF();
extern void initCubeF();
extern void drawCubeF();
extern void initTeapotF();
extern void drawTeapotF();


// Material
const GLfloat matAmbient []   = { 0.4f,  0.2f,  0.0f,  1.0f};
const GLfloat matDiffuse []   = { 0.8f,  0.4f,  0.0f,  1.0f};
const GLfloat matSpecular[]   = { 0.8f,  0.4f,  0.0f,  1.0f};
const GLfloat matShininess    = 8.0f;
// Light
const GLfloat lightAmbient [] = { 1.0f,  1.0f,  1.0f,  1.0f};
const GLfloat lightDiffuse [] = { 1.0f,  1.0f,  1.0f,  1.0f};
const GLfloat lightSpecular[] = { 1.0f,  1.0f,  1.0f,  1.0f};
const GLfloat lightPosition[] = {-5.0f, 10.0f, 10.0f,  1.0f};
// Fog
const GLfloat fogColor[]      = { 0.4f,  0.4f,  0.4f,  1.0f};


// -----------------------------------------------------------------------------
void
renderPyramid(CAVideoDevice * device, I3DRenderer * renderer, CSurface * surface_a, CSurface * surface_b)
{
  bool bDisplayB(true);

  // Set buffers
  renderer->setSurface(bDisplayB ? surface_b : surface_a);
  device->displaySurface(bDisplayB ? surface_a : surface_b);
  bDisplayB = !bDisplayB;

  // Show 1 full rotation around y axis
  for(GLfloat yrot(0.0f); yrot < 360.0f; yrot += 1.0f)
  {
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT);

    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -6.0f);
    glRotatef(yrot, 0.0f, 1.0f, 0.0f);
    drawPyramidF();

    // Flush everything to surface
    glFlush();

    // Swap display and render buffers
    renderer->setSurface(bDisplayB ? surface_b : surface_a);
    device->displaySurface(bDisplayB ? surface_a : surface_b);
    bDisplayB = !bDisplayB;
  }
}

// -----------------------------------------------------------------------------
void
renderCube(CAVideoDevice * device, I3DRenderer * renderer, CSurface * surface_a, CSurface * surface_b)
{
  bool bDisplayB(true);

  // Set buffers
  renderer->setSurface(bDisplayB ? surface_b : surface_a);
  device->displaySurface(bDisplayB ? surface_a : surface_b);
  bDisplayB = !bDisplayB;

  // Show 1 full rotation around y axis
  for(GLfloat yrot(0.0f); yrot < 360.0f; yrot += 1.0f)
  {
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT);

    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -6.0f);
    glRotatef(yrot, 0.0f, 1.0f, 0.0f);
    drawCubeF();

    // Flush everything to surface
    glFlush();

    // Swap display and render buffers
    renderer->setSurface(bDisplayB ? surface_b : surface_a);
    device->displaySurface(bDisplayB ? surface_a : surface_b);
    bDisplayB = !bDisplayB;
  }
}

// -----------------------------------------------------------------------------
void
testGL(CAVideoDevice * device, CSurface * surface_a, CSurface * surface_b)
{
  I3DRenderer * renderer;
  device->get3DRenderer(&renderer);
  glMakeCurrent(renderer);

  // Automatically wait for VSync
  device->setVSync(true);

  // Background color
  glClearColor(fogColor[0], fogColor[1], fogColor[2], fogColor[3]);

  // Depth test
  glClearDepthf(1.0f);
  glDepthFunc(GL_LEQUAL);
  //glEnable(GL_DEPTH_TEST);

  // Backface culling
  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);

  // Material
  glMaterialfv(GL_FRONT, GL_AMBIENT,   matAmbient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE,   matDiffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR,  matSpecular);
  glMaterialf (GL_FRONT, GL_SHININESS, matShininess);

  // Lighting
  glLightfv(GL_LIGHT0, GL_AMBIENT,  lightAmbient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE,  lightDiffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
  glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
  glEnable(GL_LIGHT0);
  //glEnable(GL_LIGHTING);

  // Fog
  glFogfv(GL_FOG_COLOR, fogColor);
  glFogf(GL_FOG_DENSITY, 0.35f);
  glFogf(GL_FOG_START, 1.0f);
  glFogf(GL_FOG_END, 10.0f);
  //glEnable(GL_FOG);

  // Viewport & Perspective
  glViewport(0, 0, surface_a->width(), surface_a->height());
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0f, (float)surface_a->width() / (float)surface_a->height(), 0.1f, 100.0f);

  // Move up a little
  glTranslatef(0.0f, -2.0f, 0.0f);
  // Look down a little
  glRotatef(23.0f, 1.0f, 0.0f, 0.0f);

  initPyramidF();
  initCubeF();
  glMatrixMode(GL_MODELVIEW);

  while(true)
  {
    // Without Lighting
    glDisable(GL_LIGHTING);
    // Flat
    glShadeModel(GL_FLAT);
    renderPyramid(device, renderer, surface_a, surface_b);
    // Smooth
    glShadeModel(GL_SMOOTH);
    renderPyramid(device, renderer, surface_a, surface_b);

    // With Lighting
    glEnable(GL_LIGHTING);
    // Flat
    glShadeModel(GL_FLAT);
    renderPyramid(device, renderer, surface_a, surface_b);
    // Smooth
    glShadeModel(GL_SMOOTH);
    renderPyramid(device, renderer, surface_a, surface_b);

    // With textures
    glDisable(GL_LIGHTING);
    glShadeModel(GL_FLAT);
    glEnable(GL_TEXTURE_2D);
    renderCube(device, renderer, surface_a, surface_b);
  }

  glMakeCurrent(NULL);
  delete renderer;
}
