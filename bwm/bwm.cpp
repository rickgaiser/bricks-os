#include "bwm/bwm.h"
#include "kernel/videoManager.h"
#ifdef CONFIG_GL
#include "GL/gl.h"
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
  // Set render surface
  glSetSurface(surface);

  // Initialize GL
  glClearColor(0.23f, 0.43f, 0.65f, 0.0f);
  glEnable(GL_DEPTH_TEST);
//  glClearDepth(1.0);
  glEnable(GL_CULL_FACE);
  glShadeModel(/*GL_FLAT*/GL_SMOOTH);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glViewport(0, 0, surface->width, surface->height);
  gluPerspective(45.0f, (float)surface->width / (float)surface->height, 0.1f, 100.0f);

  // Show Pyramid for 1 full rotation around y axis
  for(float fYRotTriangle(0.0f); fYRotTriangle < 360.0f; fYRotTriangle += 2.0f)
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -3.0f);
    glRotatef(fYRotTriangle, 0.0f, 1.0f, 0.0f);

    glBegin(GL_TRIANGLES);
      // Front
      glColor3ub(255,0,0);
      glVertex3i(0,1,0);
      glColor3ub(0,255,0);
      glVertex3i(-1,-1,1);
      glColor3ub(0,0,255);
      glVertex3i(1,-1,1);
      // Right
      glColor3ub(255,0,0);
      glVertex3i(0,1,0);
      glColor3ub(0,0,255);
      glVertex3i(1,-1,1);
      glColor3ub(0,255,0);
      glVertex3i(1,-1,-1);
      // Back
      glColor3ub(255,0,0);
      glVertex3i(0,1,0);
      glColor3ub(0,255,0);
      glVertex3i(1,-1,-1);
      glColor3ub(0,0,255);
      glVertex3i(-1,-1,-1);
      // Left
      glColor3ub(255,0,0);
      glVertex3i(0,1,0);
      glColor3ub(0,0,255);
      glVertex3i(-1,-1,-1);
      glColor3ub(0,255,0);
      glVertex3i(-1,-1,1);
    glEnd();

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
