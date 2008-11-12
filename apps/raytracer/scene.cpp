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


#include "scene.h"
#include "stdlib.h"
#include <math.h>


#define fabs(a) ((a) >= 0.0f ? (a) : -(a))


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
int
CPlane::intersect(CRay & ray, float & dist)
{
  float dot = ray.getDirection().dotProduct(normal_);
  if(dot != 0)
  {
    float di = -(ray.getOrigin().dotProduct(normal_) + d_) / dot;
    if(di > 0)
    {
      if(di < dist)
      {
        dist = di;
        return 1;
      }
    }
  }

  return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
CSphere::CSphere(vector3f center, float radius)
 : center_(center)
 , radius_(radius)
 , radiusSq_(radius * radius)
 , radiusR_(1.0f / radius)
{
}

// -----------------------------------------------------------------------------
int
CSphere::intersect(CRay & ray, float & dist)
{
  vector3f v = ray.getOrigin() - center_;
  float b = -v.dotProduct(ray.getDirection());
  float det = (b * b) - v.dotProduct(v) + radiusSq_;
  int retval(0);

  if(det > 0)
  {
    det = sqrtf(det);
    float i1 = b - det;
    float i2 = b + det;
    if(i2 > 0)
    {
      if(i1 < 0)
      {
        if(i2 < dist)
        {
          dist = i2;
          retval = -1;
        }
      }
      else
      {
        if(i1 < dist)
        {
          dist = i1;
          retval = 1;
        }
      }
    }
  }

  return retval;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
CBox::CBox()
 : box_(vector3f(0, 0, 0), vector3f(0, 0, 0))
 , grid_(0)
{
}

// -----------------------------------------------------------------------------
CBox::CBox(CAABB box)
 : box_(box)
 , grid_(0)
{
}

// -----------------------------------------------------------------------------
int
CBox::intersect(CRay & ray, float & dist)
{
  float pDist[6];
  vector3f ip[6];
  vector3f d = ray.getDirection();
  vector3f o = ray.getOrigin();
  bool retval = 0;
  for(int i(0); i < 6; i++)
    pDist[i] = -1;
  vector3f v1 = box_.getPos();
  vector3f v2 = box_.getPos() + getSize();
  if(d.x)
  {
    float rc = 1.0f / d.x;
    pDist[0] = (v1.x - o.x) * rc;
    pDist[3] = (v2.x - o.x) * rc;
  }
  if (d.y)
  {
    float rc = 1.0f / d.y;
    pDist[1] = (v1.y - o.y) * rc;
    pDist[4] = (v2.y - o.y) * rc;
  }
  if (d.z)
  {
    float rc = 1.0f / d.z;
    pDist[2] = (v1.z - o.z) * rc;
    pDist[5] = (v2.z - o.z) * rc;
  }
  for(int i(0); i < 6; i++)
  {
    if(pDist[i] > 0)
    {
      ip[i] = o + d * pDist[i];
      if((ip[i].x > (v1.x - EPSILON)) && (ip[i].x < (v2.x + EPSILON)) &&
         (ip[i].y > (v1.y - EPSILON)) && (ip[i].y < (v2.y + EPSILON)) &&
         (ip[i].z > (v1.z - EPSILON)) && (ip[i].z < (v2.z + EPSILON)))
      {
        if(pDist[i] < dist)
        {
          dist = pDist[i];
          retval = 1;
        }
      }
    }
  }
  return retval;
}

// -----------------------------------------------------------------------------
vector3f
CBox::getNormal(vector3f & pos)
{
  float pDist[6];

  pDist[0] = (float)fabs(box_.getSize().x - box_.getPos().x);
  pDist[1] = (float)fabs(box_.getSize().x + box_.getSize().x - box_.getPos().x);
  pDist[2] = (float)fabs(box_.getSize().y - box_.getPos().y);
  pDist[3] = (float)fabs(box_.getSize().y + box_.getSize().y - box_.getPos().y);
  pDist[4] = (float)fabs(box_.getSize().z - box_.getPos().z);
  pDist[5] = (float)fabs(box_.getSize().z + box_.getSize().z - box_.getPos().z);

  int best = 0;
  float bdist = pDist[0];
  for(int i(1) ; i < 6; i++)
  {
    if(pDist[i] < bdist)
    {
      bdist = pDist[i];
      best = i;
    }
  }
       if(best == 0) return vector3f(-1,  0,  0);
  else if(best == 1) return vector3f( 1,  0,  0);
  else if(best == 2) return vector3f( 0, -1,  0);
  else if(best == 3) return vector3f( 0,  1,  0);
  else if(best == 4) return vector3f( 0,  0, -1);
  else               return vector3f( 0,  0,  1);
}

// -----------------------------------------------------------------------------
void
CBox::setLight(bool light)
{
  CAPrimitive::setLight(light);
  if(!grid_)
  {
    grid_ = new float[32];
    grid_[ 0] = 1, grid_[ 1] = 2;
    grid_[ 2] = 3, grid_[ 3] = 3;
    grid_[ 4] = 2, grid_[ 5] = 0;
    grid_[ 6] = 0, grid_[ 7] = 1;
    grid_[ 8] = 2, grid_[ 9] = 3;
    grid_[10] = 0, grid_[11] = 3;
    grid_[12] = 0, grid_[13] = 0;
    grid_[14] = 2, grid_[15] = 2;
    grid_[16] = 3, grid_[17] = 1;
    grid_[18] = 1, grid_[19] = 3;
    grid_[20] = 1, grid_[21] = 0;
    grid_[22] = 3, grid_[23] = 2;
    grid_[24] = 2, grid_[25] = 1;
    grid_[26] = 3, grid_[27] = 0;
    grid_[28] = 1, grid_[29] = 1;
    grid_[30] = 0, grid_[31] = 2;

    for(int i(0); i < 16; i++)
    {
      grid_[i * 2] = grid_[i * 2] * box_.getSize().x / 4 + box_.getPos().x;
      grid_[i * 2 + 1] = grid_[i * 2 + 1] * box_.getSize().z / 4 + box_.getPos().z;
    }
  }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
CScene::CScene()
 : primCount_(0)
 , lightCount_(0)
{
}

// -----------------------------------------------------------------------------
void
CScene::init()
{
  CAPrimitive * pPrim;
  color4f clWall   (0.6f, 0.6f, 0.6f, 1.0f); // Top/Bottom/Left/Right
  color4f clBack   (0.6f, 0.6f, 0.6f, 1.0f);
  color4f clLight  (1.0f, 1.0f, 1.0f, 1.0f);
  color4f clSphere1(1.0f, 0.0f, 0.0f, 1.0f);
  color4f clSphere2(0.0f, 1.0f, 0.0f, 1.0f);
  color4f clSphere3(0.0f, 0.0f, 1.0f, 1.0f);

  // ---------------------------------------------------------------
  // Lights
  // ---------------------------------------------------------------
  //pPrim = new CSphere(vector3f( 0.0f,  2.5f,  1.0f), 0.2f);
  //pPrim->setLight(true);
  //addLight(pPrim);
//  pPrim = new CSphere(vector3f( 0.0f,  2.5f,  5.0f), 0.2f);
//  pPrim->getMaterial().setColor(clLight);
//  pPrim->setLight(true);
//  addLight(pPrim);
  //pPrim = new CSphere(vector3f( 0.0f,  2.5f,  9.0f), 0.2f);
  //pPrim->setLight(true);
  //addLight(pPrim);
  pPrim = new CBox(CAABB(vector3f(-1.0f,  2.5f,  5.0f), vector3f(2, 0.1f, 2)));
  pPrim->getMaterial().setColor(clLight);
  pPrim->setLight(true);
  addLight(pPrim);

  // ---------------------------------------------------------------
  // Primitives
  // ---------------------------------------------------------------
  // Top
//  pPrim = new CPlaneMirror(vector3f( 0.0f, -1.0f,  0.0f), 3.0f);
  pPrim = new CPlane(vector3f( 0.0f, -1.0f,  0.0f), 3.0f);
  pPrim->getMaterial().setColor(clWall);
  pPrim->getMaterial().setDiffuse(0.8f);
  pPrim->getMaterial().setReflection(0.0f);
  addPrimitive(pPrim);
  // Left
  pPrim = new CPlaneMirror(vector3f( 1.0f,  0.0f,  0.0f), 3.0f);
//  pPrim = new CPlane(vector3f( 1.0f,  0.0f,  0.0f), 3.0f);
//  pPrim->getMaterial().setColor(clWall);
//  pPrim->getMaterial().setDiffuse(0.8f);
//  pPrim->getMaterial().setReflection(0.0f);
  addPrimitive(pPrim);
  // Bottom
  pPrim = new CPlane(vector3f( 0.0f,  1.0f,  0.0f), 3.0f);
  pPrim->getMaterial().setColor(clWall);
  pPrim->getMaterial().setDiffuse(0.8f);
  pPrim->getMaterial().setReflection(0.0f);
  addPrimitive(pPrim);
  // Right
//  pPrim = new CPlaneMirror(vector3f(-1.0f,  0.0f,  0.0f), 3.0f);
  pPrim = new CPlane(vector3f(-1.0f,  0.0f,  0.0f), 3.0f);
  pPrim->getMaterial().setColor(clWall);
  pPrim->getMaterial().setDiffuse(0.8f);
  pPrim->getMaterial().setReflection(0.0f);
  addPrimitive(pPrim);
  // Back
  pPrim = new CPlane (vector3f( 0.0f,  0.0f, -1.0f),  8.0f);
  pPrim->getMaterial().setColor(clBack);
  pPrim->getMaterial().setDiffuse(0.8f);
  pPrim->getMaterial().setReflection(0.0f);
  addPrimitive(pPrim);
  // Front
  pPrim = new CPlane (vector3f( 0.0f,  0.0f,  1.0f),  6.0f);
  pPrim->getMaterial().setColor(clBack);
  pPrim->getMaterial().setDiffuse(0.8f);
  pPrim->getMaterial().setReflection(0.0f);
  addPrimitive(pPrim);

  // Sphere
  pPrim = new CSphere(vector3f(-1.0f,  1.0f,  4.0f),  1.0f);
  pPrim->getMaterial().setColor(clSphere1);
  pPrim->getMaterial().setDiffuse(0.6f);
  pPrim->getMaterial().setReflection(0.5f);
  addPrimitive(pPrim);
  // Sphere
  pPrim = new CSphere(vector3f( 1.0f, -1.0f,  5.0f),  1.0f);
  pPrim->getMaterial().setColor(clSphere2);
  pPrim->getMaterial().setDiffuse(0.6f);
  pPrim->getMaterial().setReflection(0.5f);
  addPrimitive(pPrim);
  // Sphere
  pPrim = new CSphere(vector3f( 0.5f,  0.5f,  6.0f),  1.0f);
  pPrim->getMaterial().setColor(clSphere3);
  pPrim->getMaterial().setDiffuse(0.6f);
  pPrim->getMaterial().setReflection(0.5f);
  addPrimitive(pPrim);
}

// -----------------------------------------------------------------------------
void
CScene::addPrimitive(CAPrimitive * prim)
{
  if(primCount_ < MAX_PRIM_COUNT)
  {
    prim_[primCount_] = prim;
    primCount_++;
  }
}

// -----------------------------------------------------------------------------
void
CScene::addLight(CAPrimitive * prim)
{
  if(lightCount_ < MAX_LIGHT_COUNT)
  {
    light_[lightCount_] = prim;
    lightCount_++;
    this->addPrimitive(prim);
  }
}
