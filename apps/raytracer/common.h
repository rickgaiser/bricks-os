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


#ifndef COMMON_H
#define COMMON_H


#include "vhl/vector.h"
#include "vhl/color.h"


// -----------------------------------------------------------------------------
#define vector3f TVector3<float>
#define vector4f TVector4<float>
//#define color3f  TColor3<float>
#define color4f  TColor<float>


// -----------------------------------------------------------------------------
#define EPSILON 0.0001f


// -----------------------------------------------------------------------------
class CRay
{
public:
  CRay(){};
  CRay(vector3f origin, vector3f direction) : origin_(origin), direction_(direction){};

  void setOrigin   (vector3f & origin)    {origin_    = origin;}
  void setDirection(vector3f & direction) {direction_ = direction;}

  vector3f & getOrigin()    {return origin_;}
  vector3f & getDirection() {return direction_;}

private:
  vector3f origin_;
  vector3f direction_;
};

// -----------------------------------------------------------------------------
class CAABB
{
public:
  CAABB() : pos_(vector3f(0, 0, 0)), size_(vector3f(0, 0, 0)) {};
  CAABB(vector3f pos, vector3f size) : pos_(pos), size_(size) {};

  vector3f & getPos()  {return pos_;}
  vector3f & getSize() {return size_;}

  bool intersect(CAABB & b2)
  {
    vector3f v1 = b2.getPos();
    vector3f v2 = b2.getPos() + b2.getSize();
    vector3f v3 = pos_;
    vector3f v4 = pos_ + size_;
    return ((v4.x > v1.x) && (v3.x < v2.x) &&  // x-axis overlap
            (v4.y > v1.y) && (v3.y < v2.y) &&  // y-axis overlap
            (v4.z > v1.z) && (v3.z < v2.z));   // z-axis overlap
  }
  bool contains(vector3f pos)
  {
    vector3f v1 = pos_;
    vector3f v2 = pos_ + size_;
    return ((pos.x > (v1.x - EPSILON)) && (pos.x < (v2.x + EPSILON)) &&
            (pos.y > (v1.y - EPSILON)) && (pos.y < (v2.y + EPSILON)) &&
            (pos.z > (v1.z - EPSILON)) && (pos.z < (v2.z + EPSILON)));
  }

private:
  vector3f pos_;
  vector3f size_;
};


#endif
