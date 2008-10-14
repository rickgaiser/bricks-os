#include "GLES/gl.h"
#include "GLES/gl_extra.h"
#include "GL/glu.h"
#include "kernel/videoManager.h"
#include "glconfig.h"
#include "../gl/context.h"


extern void glMakeCurrent(I3DRenderer * ctx);
extern void initPyramidF();
extern void drawPyramidF();
#ifdef ENABLE_TEXTURES
extern void initCubeF();
extern void drawCubeF();
#endif


#ifdef ENABLE_LIGHTING
// Material
const GLfloat matAmbient []   = { 0.2f,  0.2f,  0.2f,  1.0f};
const GLfloat matDiffuse []   = { 0.6f,  0.6f,  0.6f,  1.0f};
const GLfloat matSpecular[]   = { 0.2f,  0.2f,  0.2f,  1.0f};
const GLfloat matShininess    = 8.0f;
// Light
const GLfloat lightAmbient [] = { 1.0f,  1.0f,  1.0f,  1.0f};
const GLfloat lightDiffuse [] = { 1.0f,  1.0f,  1.0f,  1.0f};
const GLfloat lightSpecular[] = { 1.0f,  1.0f,  1.0f,  1.0f};
const GLfloat lightPosition[] = {-5.0f, 10.0f, 10.0f,  1.0f};
#endif
// Fog/Clear color
const GLfloat fogColor[]      = { 0.4f,  0.4f,  0.4f,  1.0f};


// -----------------------------------------------------------------------------
int
renderPyramidF(CAVideoDevice * device, I3DRenderer * renderer, CSurface * surface_a, CSurface * surface_b)
{
  int iFrameCount(0);
  int iStartFrame;
  int iCurrentFrame;

#ifdef ENABLE_DOUBLE_BUFFERED
  bool bDisplayB(true);

  // Set buffers
  renderer->setSurface(bDisplayB ? surface_b : surface_a);
  device->displaySurface(bDisplayB ? surface_a : surface_b);
  bDisplayB = !bDisplayB;
#endif

  iStartFrame = device->getFrameNr();

  // Show 1 full rotation around y axis
  for(GLfloat yrot(0.0f); yrot < 360.0f;)
  {
#ifdef ENABLE_DEPTH_TEST
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#else
    glClear(GL_COLOR_BUFFER_BIT);
#endif

    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -6.0f);
    glRotatef(yrot, 0.0f, 1.0f, 0.0f);
    drawPyramidF();

    // Flush everything to surface
    glFlush();

    // Swap display and render buffers
#ifdef ENABLE_DOUBLE_BUFFERED
    renderer->setSurface(bDisplayB ? surface_b : surface_a);
    device->displaySurface(bDisplayB ? surface_a : surface_b);
    bDisplayB = !bDisplayB;
#endif
#ifdef ENABLE_VSYNC
    device->waitVSync();
#endif

    iFrameCount++;
    iCurrentFrame = device->getFrameNr();
    yrot = 1.0f * (iCurrentFrame - iStartFrame);
  }

  return iFrameCount;
}

#ifdef ENABLE_TEXTURES
// -----------------------------------------------------------------------------
int
renderCubeF(CAVideoDevice * device, I3DRenderer * renderer, CSurface * surface_a, CSurface * surface_b)
{
  int iFrameCount(0);
  int iStartFrame;
  int iCurrentFrame;

#ifdef ENABLE_DOUBLE_BUFFERED
  bool bDisplayB(true);

  // Set buffers
  renderer->setSurface(bDisplayB ? surface_b : surface_a);
  device->displaySurface(bDisplayB ? surface_a : surface_b);
  bDisplayB = !bDisplayB;
#endif

  iStartFrame = device->getFrameNr();

  // Show 1 full rotation around y axis
  for(GLfloat yrot(0.0f); yrot < 360.0f;)
  {
#ifdef ENABLE_DEPTH_TEST
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#else
    glClear(GL_COLOR_BUFFER_BIT);
#endif

    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -6.0f);
    glRotatef(yrot, 0.0f, 1.0f, 0.0f);
    drawCubeF();

    // Flush everything to surface
    glFlush();

    // Swap display and render buffers
#ifdef ENABLE_DOUBLE_BUFFERED
    renderer->setSurface(bDisplayB ? surface_b : surface_a);
    device->displaySurface(bDisplayB ? surface_a : surface_b);
    bDisplayB = !bDisplayB;
#endif
#ifdef ENABLE_VSYNC
    device->waitVSync();
#endif

    iFrameCount++;
    iCurrentFrame = device->getFrameNr();
    yrot = 1.0f * (iCurrentFrame - iStartFrame);
  }

  return iFrameCount;
}
#endif

// -----------------------------------------------------------------------------
void
testGLF(CAVideoDevice * device, CSurface * surface_a, CSurface * surface_b)
{
  I3DRenderer * renderer;
  device->get3DRenderer(&renderer);
  glMakeCurrent(renderer);

  // Set buffers
  renderer->setSurface(surface_a);
  device->displaySurface(surface_a);

  // Do not automatically wait for VSync
  device->setVSync(false);

  // Background color
  glClearColor(fogColor[0], fogColor[1], fogColor[2], fogColor[3]);

#ifdef ENABLE_DEPTH_TEST
  // Depth test
  glClearDepthf(1.0f);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_DEPTH_TEST);
#endif

  // Backface culling
  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);

#ifdef ENABLE_LIGHTING
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
#endif

#ifdef ENABLE_FOG
  // Fog
  glFogfv(GL_FOG_COLOR, fogColor);
  glFogf(GL_FOG_DENSITY, 0.35f);
  glFogf(GL_FOG_START, 1.0f);
  glFogf(GL_FOG_END, 10.0f);
  glEnable(GL_FOG);
#endif

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
#ifdef ENABLE_TEXTURES
  initCubeF();
#endif

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Notify user of error
  if(glGetError() != GL_NO_ERROR)
  {
    glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
  }

  while(true)
  {
    // Flat
//    glShadeModel(GL_FLAT);
//    renderPyramidF(device, renderer, surface_a, surface_b);
    // Smooth
    glShadeModel(GL_SMOOTH);
    renderPyramidF(device, renderer, surface_a, surface_b);

#ifdef ENABLE_LIGHTING
    glEnable(GL_LIGHTING);

    // Flat
//    glShadeModel(GL_FLAT);
//    renderPyramidF(device, renderer, surface_a, surface_b);
    // Smooth
    glShadeModel(GL_SMOOTH);
    renderPyramidF(device, renderer, surface_a, surface_b);

    glDisable(GL_LIGHTING);
#endif

#ifdef ENABLE_TEXTURES
    glEnable(GL_TEXTURE_2D);

    // Flat
    glShadeModel(GL_FLAT);
    renderCubeF(device, renderer, surface_a, surface_b);

    glDisable(GL_TEXTURE_2D);
#endif
  }

  glMakeCurrent(NULL);
  delete renderer;
}
