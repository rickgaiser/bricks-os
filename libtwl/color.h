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


#ifndef TWL_COLOR_H
#define TWL_COLOR_H


#include "kernel/videoManager.h"


namespace twl
{


#define clBlack BxColorFormat_FromRGB(cfA8R8G8B8, 0,0,0)
#define clDGray BxColorFormat_FromRGB(cfA8R8G8B8, 64,64,64)
#define clGray  BxColorFormat_FromRGB(cfA8R8G8B8, 128,128,128)
#define clLGray BxColorFormat_FromRGB(cfA8R8G8B8, 192,192,192)
#define clWhite BxColorFormat_FromRGB(cfA8R8G8B8, 255,255,255)

#define clRed   BxColorFormat_FromRGB(cfA8R8G8B8, 255,0,0)
#define clGreen BxColorFormat_FromRGB(cfA8R8G8B8, 0,255,0)
#define clBlue  BxColorFormat_FromRGB(cfA8R8G8B8, 0,0,255)


// Default desktop color
extern color_t clDesktop;
// Window title-bar colors
extern color_t clActiveWindowL;
extern color_t clActiveWindowR;
extern color_t clActiveWindow;
extern color_t clInActiveWindowL;
extern color_t clInActiveWindowR;
extern color_t clInActiveWindow;
// Panel colors
extern color_t clPanelLight;
extern color_t clPanelShade1;
extern color_t clPanelShade2;
extern color_t clPanelFill;


};


#endif
