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


#ifndef RAYTRACER_H
#define RAYTRACER_H


#include "kernel/videoManager.h"
#include "vhl/color.h"
#include "scene.h"


// -----------------------------------------------------------------------------
class CRaytracer
{
public:
  CRaytracer();

  void setSurface(CSurface * surface);
  void render();

private:
  CAPrimitive * trace(CRay & ray, color4f & color, int depth, float rindex, float & dist);
  float calcShade(CAPrimitive * light, vector3f ip, vector3f & dir);
  int findNearest(CRay & ray, float & dist, CAPrimitive *& prim);

private:
  CSurface * pSurface_;
  int iWidth_;
  int iHeight_;
  uint16_t * pData_;

  float fStartX_;
  float fDeltaX_;
  float fPixelDeltaX_;
  float fStartY_;
  float fDeltaY_;
  float fPixelDeltaY_;

  CScene scene_;
};


#endif
