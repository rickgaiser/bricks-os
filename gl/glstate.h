/*
 * Bricks-OS, Operating System for Game Consoles
 * Copyright (C) 2009 Maximus32 <Maximus32@bricks-os.org>
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


#ifndef GL_GLSTATE_H
#define GL_GLSTATE_H


#include "GL/gl.h"
#include "vhl/vector.h"
#include "vhl/color.h"


//-----------------------------------------------------------------------------
template <class T>
struct TTexturing
{
  bool          enabled;

  T             coordCurrent[4];
  GLenum        envMode;
  TColor<T>     envColor;
};

//-----------------------------------------------------------------------------
template <class T>
struct TDepthTest
{
  bool          enabled;

  bool          mask;
  GLenum        function;
  T             clear;
  T             rangeNear;
  T             rangeFar;
};

//-----------------------------------------------------------------------------
struct SBlending
{
  bool          enabled;

  GLenum        sourceFactor;
  GLenum        destFactor;
};

//-----------------------------------------------------------------------------
template <class T>
struct TAlphaTest
{
  bool        enabled;

  GLenum      func;
  T           value;
};

//-----------------------------------------------------------------------------
template <class T>
struct TLight
{
  bool          enabled;

  TColor<T>     ambient;
  TColor<T>     diffuse;
  TColor<T>     specular;

  TVector4<T>   position;
  TVector3<T>   positionNormal;
};

//-----------------------------------------------------------------------------
template <class T>
struct TLighting
{
  bool          enabled;

  TLight<T>     light[8];

  bool          normalizeEnabled;

  TVector3<T>   normal;
};

//-----------------------------------------------------------------------------
template <class T>
struct TMaterial
{
  TColor<T>   ambient;
  TColor<T>   diffuse;
  TColor<T>   specular;
  TColor<T>   emission;
  T           shininess;
};

//-----------------------------------------------------------------------------
template <class T>
struct TFog
{
  bool        enabled;

  T           mode;
  T           density;
  T           start;
  T           end;
  TColor<T>   color;
};

//-----------------------------------------------------------------------------
template <class T>
struct TGLState
{
  // Colors
  TColor<T>       clCurrent;
  TColor<T>       clClear;
  GLenum          shadingModel;
  // Textures
  TTexturing<T>   texturing;
  // Depth testing
  TDepthTest<T>   depthTest;
  // Alpha Blending
  SBlending       blending;
  // Alpha testing
  TAlphaTest<T>   alphaTest;
  // Lighting
  TLighting<T>    lighting;
  // Material
  TMaterial<T>    materialFront;
  TMaterial<T>    materialBack;
  // Fog
  TFog<T>         fog;
};


#endif
