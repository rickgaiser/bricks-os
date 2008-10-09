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
