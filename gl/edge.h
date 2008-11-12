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


#ifndef EDGE_H
#define EDGE_H


#include "GL/gl.h"
#include "context.h"
#include "inttypes.h"
#include "asm/arch/config.h"


#ifdef CONFIG_FPU
//-----------------------------------------------------------------------------
class CEdgeF
{
public:
  CEdgeF(uint32_t height);
  ~CEdgeF();

  void add  (const SVertexF & from, const SVertexF & to);
  void addZ (const SVertexF & from, const SVertexF & to);
  void addC (const SVertexF & from, const SVertexF & to);
  void addZC(const SVertexF & from, const SVertexF & to);
  void addT (const SVertexF & from, const SVertexF & to);
  void addZT(const SVertexF & from, const SVertexF & to);

public:
  // Edge x
  GLint * x_;
  // Edge depth
  GLfloat * z_;
  // Texture coordinates
  GLfloat * ts_;
  GLfloat * tt_;
  // Edge color
  GLfloat * r_;
  GLfloat * g_;
  GLfloat * b_;
  GLfloat * a_;

  int32_t iHeight_;
};
#else
//-----------------------------------------------------------------------------
class CEdgeFx
{
public:
  CEdgeFx(uint32_t height);
  ~CEdgeFx();

  void add  (const SVertexFx & from, const SVertexFx & to);
  void addZ (const SVertexFx & from, const SVertexFx & to);
  void addC (const SVertexFx & from, const SVertexFx & to);
  void addZC(const SVertexFx & from, const SVertexFx & to);
  void addT (const SVertexFx & from, const SVertexFx & to);
  void addZT(const SVertexFx & from, const SVertexFx & to);

public:
  // Edge x
  GLint  * x_;
  // Edge depth
  CFixed * z_;
  // Texture coordinates
  CFixed * ts_;
  CFixed * tt_;
  // Edge color
  CFixed * r_;
  CFixed * g_;
  CFixed * b_;
  CFixed * a_;

  int32_t iHeight_;
};
#endif // CONFIG_FPU


#include "edge.inl"


#endif
