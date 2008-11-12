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


#ifndef SCENE_H
#define SCENE_H


#include "vhl/vector.h"
#include "vhl/color.h"
#include "common.h"


#define MAX_PRIM_COUNT   20
#define MAX_LIGHT_COUNT   4


// -----------------------------------------------------------------------------
class CMaterial
{
public:
  CMaterial(){}

  void setColor(color4f & color){color_ = color;}
  color4f & getColor(){return color_;}

  void setDiffuse(float diffuse){diffuse_ = diffuse;}
  float getDiffuse(){return diffuse_;}

  float getSpecular(){return 1.0f - diffuse_;}

  void setReflection(float reflection){reflection_ = reflection;}
  float getReflection(){return reflection_;}

private:
  color4f color_;
  float diffuse_;
  float reflection_;
};

// -----------------------------------------------------------------------------
class CAPrimitive
{
public:
  enum
  {
    SPHERE = 1,
    PLANE,
    AABB,
  };

  CAPrimitive() : bLight_(false) {}
  virtual ~CAPrimitive() {}

  virtual void setLight(bool light){bLight_ = light;}
  virtual bool getLight(){return bLight_;}

  virtual int getType() = 0;
  virtual int intersect(CRay & ray, float & dist) = 0;
  virtual vector3f getNormal(vector3f & pos) = 0;

  void setMaterial(CMaterial & mat){material_ = mat;}
  CMaterial & getMaterial(){return material_;}

private:
  CMaterial material_;
  bool bLight_;
};

// -----------------------------------------------------------------------------
class CPlane
 : public CAPrimitive
{
public:
  CPlane(vector3f normal, float d) : normal_(normal), d_(d) {}

  int getType(){return PLANE;}
  virtual int intersect(CRay & ray, float & dist);
  virtual vector3f getNormal(vector3f & pos){return normal_;};

private:
  vector3f normal_;
  float d_;
};

// -----------------------------------------------------------------------------
class CPlaneMirror
 : public CPlane
{
public:
  CPlaneMirror(vector3f normal, float d) : CPlane(normal,d)
  {
    color4f c(0.6f, 0.6f, 0.6f, 1.0f);

    this->getMaterial().setColor(c);
    this->getMaterial().setDiffuse(0.2f);
    this->getMaterial().setReflection(1.0f);
  }
};

// -----------------------------------------------------------------------------
class CBox
 : public CAPrimitive
{
public:
  CBox();
  CBox(CAABB box);

  virtual void setLight(bool light);

  int getType(){return AABB;}
  virtual int intersect(CRay & ray, float & dist);
  virtual vector3f getNormal(vector3f & pos);

  bool intersectBox(CAABB & box) {return box_.intersect(box);}
  bool contains(vector3f & pos)   {return box_.contains(pos);}
  vector3f & getPos()  {return box_.getPos();}
  vector3f & getSize() {return box_.getSize();}
  float getGridX(int idx) {return grid_[idx << 1];}
  float getGridY(int idx) {return grid_[(idx << 1) + 1];}
  CAABB getAABB() {return box_;}

protected:
  CAABB box_;
  float * grid_;
};

// -----------------------------------------------------------------------------
class CSphere
 : public CAPrimitive
{
public:
  CSphere(vector3f center, float radius);

  int getType(){return SPHERE;}
  virtual int intersect(CRay & ray, float & dist);
  virtual vector3f getNormal(vector3f & pos){return (pos - center_) * radiusR_;};

  vector3f & getCenter(){return center_;}

private:
  vector3f center_;
  float radius_;
  float radiusSq_;
  float radiusR_;
};

// -----------------------------------------------------------------------------
class CScene
{
public:
  CScene();

  void init();

public:
  CAPrimitive * prim_[MAX_PRIM_COUNT];
  int primCount_;

  CAPrimitive * light_[MAX_LIGHT_COUNT];
  int lightCount_;

private:
  void addPrimitive(CAPrimitive * prim);
  void addLight(CAPrimitive * prim);
};


#endif
