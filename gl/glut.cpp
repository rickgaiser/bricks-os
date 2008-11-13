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


#include "GL/glut.h"
#include "GL/gl.h"
#include "kernel/debug.h"
#include "kernel/videoManager.h"
#include "kernel/inputManager.h"
#include "kernel/3dRenderer.h"
#include "asm/arch/registers.h"
#include "stdlib.h"

#ifdef PSP
#include <pspctrl.h>
#endif

// NDS Renders to screen directly
#if defined(NDS)
#else
  #define BUFFER_A
#endif

// NGC/Wii Uses copy from RGB to YPbPr framebuffer
// NDS Renders to screen directly
#if defined(NGC) || defined(WII) || defined(NDS)
#else
  #define BUFFER_B // Double buffering
#endif


extern void glMakeCurrent(I3DRenderer * ctx);


// Selected device
CAVideoDevice * pGlutDevice = NULL;
CAControllerDevice * pController = NULL;

// Selected mode
const SVideoMode * pGlutMode = NULL;

// 2 surfaces for double buffering
#ifdef BUFFER_A
CSurface * pGlutSurfaceA = NULL;
#endif
#ifdef BUFFER_B
CSurface * pGlutSurfaceB = NULL;
bool bGlutActiveDisplay = false;
#endif

// 3D rendering device
I3DRenderer * pGlutRenderer;


typedef void (* glutDisplayFunction)(void);
typedef void (* glutReshapeFunction)(int, int);
typedef void (* glutKeyboardFunction)(unsigned char, int, int);
typedef void (* glutSpecialFunction)(int, int, int);
glutDisplayFunction  fpGlutDisplay  = NULL;
glutReshapeFunction  fpGlutReshape  = NULL;
glutKeyboardFunction fpGlutKeyboard = NULL;
glutSpecialFunction  fpGlutSpecial  = NULL;


// -----------------------------------------------------------------------------
void
glutInit(int * pargc, char ** argv)
{
  CAVideoDevice ** devices;
  int iDeviceCount;
  CAInputDevice ** inputDevices;
  int iInputDeviceCount;

  // Initialize video device
  videoManager.listDevices(&devices, &iDeviceCount);
  if(iDeviceCount > 0)
  {
    pGlutDevice = devices[0];

    pGlutDevice->getDefaultMode(&pGlutMode);
    if(pGlutMode != NULL)
    {
      // Set default video mode
      pGlutDevice->setMode(pGlutMode);

      // Allocate two surfaces for double buffering
#ifdef BUFFER_A
      pGlutDevice->getSurface(&pGlutSurfaceA, pGlutMode->width, pGlutMode->height);
#endif
#ifdef BUFFER_B
      pGlutDevice->getSurface(&pGlutSurfaceB, pGlutMode->width, pGlutMode->height);
#endif

      // Allocate renderer and activate it
      pGlutDevice->get3DRenderer(&pGlutRenderer);
      glMakeCurrent(pGlutRenderer);

#ifdef BUFFER_A
      pGlutRenderer->setSurface(pGlutSurfaceA);
      pGlutDevice->displaySurface(pGlutSurfaceA);
#endif
    }
    else
      printk("ERROR: Device has no modes!\n");
  }
  else
    printk("ERROR: No video devices!\n");

  // Initialize input device
  inputManager.listDevices(&inputDevices, &iInputDeviceCount);
  for(int i(0); i < iInputDeviceCount; i++)
  {
    if(inputDevices[i]->type() == IDT_CONTROLLER)
    {
      pController = (CAControllerDevice *)inputDevices[i];
      break;
    }
  }
}

// -----------------------------------------------------------------------------
void
glutDisplayFunc(void (* callback)(void))
{
  fpGlutDisplay = callback;
}

// -----------------------------------------------------------------------------
void
glutReshapeFunc(void (* callback)(int, int))
{
  fpGlutReshape = callback;
}

// -----------------------------------------------------------------------------
void
glutKeyboardFunc(void (* callback)(unsigned char, int, int))
{
  fpGlutKeyboard = callback;
}

// -----------------------------------------------------------------------------
void
glutSpecialFunc(void (* callback)(int, int, int))
{
  fpGlutSpecial = callback;
}

// -----------------------------------------------------------------------------
void
glutProcessKeys()
{
  bool bUp(false), bDown(false), bLeft(false), bRight(true);

  if(pController != NULL)
  {
    uint32_t keys = pController->getButtonState();
    bUp    = (keys & CTRL_BTN_UP);
    bDown  = (keys & CTRL_BTN_DOWN);
    bLeft  = (keys & CTRL_BTN_LEFT);
    bRight = (keys & CTRL_BTN_RIGHT);
  }
  else
  {
#if defined(NGC) || defined(WII)
    uint32_t datah, datal;
    datah  = REG_SI_CHANNEL0_INBUFH;
    datal  = REG_SI_CHANNEL0_INBUFL;
    bUp    = (datah & KEY_UP);
    bDown  = (datah & KEY_DOWN);
    bLeft  = (datah & KEY_LEFT);
    bRight = (datah & KEY_RIGHT);
#endif
#ifdef PSP
    SceCtrlData pad;
    sceCtrlReadBufferPositive(&pad, 1);
    bUp    = (pad.Buttons & PSP_CTRL_UP);
    bDown  = (pad.Buttons & PSP_CTRL_DOWN);
    bLeft  = (pad.Buttons & PSP_CTRL_LEFT);
    bRight = (pad.Buttons & PSP_CTRL_RIGHT);
#endif
  }

  if(bUp)
    fpGlutSpecial(GLUT_KEY_UP, 0, 0);
  if(bDown)
    fpGlutSpecial(GLUT_KEY_DOWN, 0, 0);
  if(bLeft)
    fpGlutSpecial(GLUT_KEY_LEFT, 0, 0);
  if(bRight)
    fpGlutSpecial(GLUT_KEY_RIGHT, 0, 0);
}

// -----------------------------------------------------------------------------
void
glutMainLoop(void)
{
  if((fpGlutDisplay == NULL) || (fpGlutReshape == NULL))
    return;

  fpGlutReshape(pGlutMode->width, pGlutMode->height);

  while(true)
  {
    if(fpGlutSpecial != NULL)
      glutProcessKeys();

    fpGlutDisplay();
  }
}

// -----------------------------------------------------------------------------
void
glutSwapBuffers(void)
{
  pGlutDevice->waitVSync();
#ifdef BUFFER_B
  pGlutRenderer->setSurface(bGlutActiveDisplay ? pGlutSurfaceA : pGlutSurfaceB);
  pGlutDevice->displaySurface(bGlutActiveDisplay ? pGlutSurfaceB : pGlutSurfaceA);
  bGlutActiveDisplay = !bGlutActiveDisplay;
#endif
}
