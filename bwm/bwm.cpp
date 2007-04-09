#include "bwm/bwm.h"
#include "kernel/videoManager.h"
#ifdef CONFIG_GL
//#include "EGL/egl.h"
#include "GLES/gl.h"
#include "GLES/gl_extra.h"
#endif // CONFIG_GL
#include "iostream"


// Default desktop color
color_t clDesktop(BxRGB(58, 110, 165));
// Window title-bar colors
color_t clActiveWindowL(BxRGB(10, 36, 106));
color_t clActiveWindowR(BxRGB(166, 202, 240));
color_t clActiveWindow(clActiveWindowL);
color_t clInActiveWindowL(BxRGB(128, 128, 128));
color_t clInActiveWindowR(BxRGB(192, 192, 192));
color_t clInActiveWindow(clInActiveWindowL);
// Panel colors
color_t clPanelLight(BxRGB(255, 255, 255));
color_t clPanelShade1(BxRGB(64, 64, 64));
color_t clPanelShade2(BxRGB(128, 128, 128));
color_t clPanelFill(BxRGB(212, 208, 200));


#ifdef CONFIG_GL
const GLfloat lightAmbient[] = {0.1f, 0.1f, 0.1f, 1.0f};
const GLfloat lightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
const GLfloat fogColor[]     = {0.5f, 0.5f, 0.5f, 0.5f};

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
GLint triangle_count(4);

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
#endif // CONFIG_GL


// -----------------------------------------------------------------------------
void
testFill(CSurface * surface)
{
  for(int i(0); i < 0x001f; i++)
  {
    // Fill entire screen with one color
    surface->fill(i);
    // Display progress bar
    surface->fillRect(1, surface->height - 12, surface->width - 2, 10, clWhite);
    surface->fillRect(3, surface->height - 10, ((surface->width - 6) * i) / 0x001f, 6, clBlack);
    // Swap back and front buffer, placing the rendered image on screen
    surface->swap();
  }
}

#ifdef CONFIG_GL
// -----------------------------------------------------------------------------
void
testGL(CSurface * surface)
{
  //EGLDisplay   egldisplay;
  //EGLConfig    eglconfig = 0;
  //EGLSurface   eglsurface;
  //EGLContext   eglcontext;

  //initialize EGL
  //egldisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  //eglInitialize(egldisplay, 0, 0);
  //eglBindAPI(EGL_OPENGL_ES_API);
  //eglChooseConfig(egldisplay, s_configAttribs, &eglconfig, 1, &numconfigs);
  //eglsurface = eglCreateWindowSurface(egldisplay, eglconfig, (EGLNativeDisplayType)surface, 0);
  //eglcontext = eglCreateContext(egldisplay, eglconfig, 0, 0);
  //eglMakeCurrent(egldisplay, eglsurface, eglsurface, eglcontext);

  // Set render surface
  glSetSurface(surface);

  // Initialize GL
//  glClearColor(0.23f, 0.43f, 0.65f, 0.0f);
  glClearColor(fogColor[0], fogColor[1], fogColor[2], fogColor[3]);
  glEnable(GL_DEPTH_TEST);
  glClearDepthf(1.0f);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_CULL_FACE);
  glShadeModel(/*GL_FLAT*/GL_SMOOTH);
  glViewport(0, 0, surface->width, surface->height);
  gluPerspective(45.0f, (float)surface->width / (float)surface->height, 0.1f, 100.0f);

  // Lighting
  glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHTING);

  // Fog
  glFogfv(GL_FOG_COLOR, fogColor);
  glFogf(GL_FOG_DENSITY, 0.35f);
  glFogf(GL_FOG_START, 1.0f);
  glFogf(GL_FOG_END, 10.0f);
  glEnable(GL_FOG);

  glVertexPointer(3, GL_FLOAT, 0, triangle);
  glColorPointer(4, GL_FLOAT, 0, colors);
  glNormalPointer(GL_FLOAT, 0, normals);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  // Move up a little
  glTranslatef(0.0f, -2.0f, 0.0f);
  // Look down a little
  glRotatef(-30.0f, 1.0f, 0.0f, 0.0f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Show Pyramid for 1 full rotation around y axis
  for(float fYRotTriangle(0.0f); fYRotTriangle < 360.0f; fYRotTriangle += 2.0f)
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -4.0f);
    glRotatef(fYRotTriangle, 0.0f, 1.0f, 0.0f);
    glDrawArrays(GL_TRIANGLES, 0, triangle_count);

    glLoadIdentity();
    glTranslatef(-2.0f, 0.0f, -7.0f);
    glRotatef(fYRotTriangle, 0.0f, 1.0f, 0.0f);
    glDrawArrays(GL_TRIANGLES, 0, triangle_count);

    glLoadIdentity();
    glTranslatef(2.0f, 0.0f, -7.0f);
    glRotatef(fYRotTriangle, 0.0f, 1.0f, 0.0f);
    glDrawArrays(GL_TRIANGLES, 0, triangle_count);

    glFlush();

    // Display progress bar
    surface->fillRect(1, surface->height - 12, surface->width - 2, 10, clWhite);
    surface->fillRect(3, surface->height - 10, ((surface->width - 6) * static_cast<int>(fYRotTriangle)) / 360, 6, clBlack);

    surface->swap();
  }
}
#endif // CONFIG_GL

// -----------------------------------------------------------------------------
int
bwm(int argc, char * argv[])
{
  CAVideoDevice ** devices;
  int iDeviceCount;
  videoManager.listDevices(&devices, &iDeviceCount);
  if(iDeviceCount > 0)
  {
    for(int iDev(0); iDev < iDeviceCount; iDev++)
    {
      const SVideoMode * modes;
      int iModeCount;
      devices[iDev]->listModes(&modes, &iModeCount);
      if(iModeCount > 0)
      {
        for(int iMode(0); iMode < iModeCount; iMode++)
        {
          if((modes[iMode].format == pfR5G5B5) || (modes[iMode].format == pfA1R5G5B5))
          {
            devices[iDev]->setMode(&modes[iMode]);
            CSurface * pVideoSurface;
            devices[iDev]->getSurface(&pVideoSurface, stSCREEN);

            while(true)
            {
#ifndef CONFIG_GL
              testFill(pVideoSurface);
#else
              testGL(pVideoSurface);
#endif // CONFIG_GL
            }
            delete pVideoSurface;
          }
        }
      }
      else
      {
        std::cout<<"ERROR: Device has no modes!"<<std::endl;
      }
    }
  }
  else
  {
    std::cout<<"ERROR: No video devices!"<<std::endl;
  }

  return 0;
}
