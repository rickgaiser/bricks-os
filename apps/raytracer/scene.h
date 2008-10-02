#ifndef SCENE_H
#define SCENE_H


#include "vhl/vector.h"
#include "vhl/color.h"
#include "common.h"


// -----------------------------------------------------------------------------
class CMaterial
{
public:
  CMaterial(){}

  void setColor(TColor<float> & color){color_ = color;}
  TColor<float> & getColor(){return color_;}

  void setDiffuse(float diffuse){diffuse_ = diffuse;}
  float getDiffuse(){return diffuse_;}

  float getSpecular(){return 1.0f - diffuse_;}

  void setReflection(float reflection){reflection_ = reflection;}
  float getReflection(){return reflection_;}

private:
  TColor<float> color_;
  float diffuse_;
  float reflection_;
};

// -----------------------------------------------------------------------------
class CAPrimitive
{
public:
  virtual ~CAPrimitive(){}

  virtual int intersect(CRay & ray, float & dist) = 0;
  virtual TVector3<float> getNormal(TVector3<float> & pos) = 0;

  void setMaterial(CMaterial & mat){material_ = mat;}
  CMaterial & getMaterial(){return material_;}

private:
  CMaterial material_;
};

// -----------------------------------------------------------------------------
class CPlane
 : public CAPrimitive
{
public:
  CPlane(TVector3<float> normal, float d) : normal_(normal), d_(d) {}

  virtual int intersect(CRay & ray, float & dist);
  virtual TVector3<float> getNormal(TVector3<float> & pos){return normal_;};

private:
  TVector3<float> normal_;
  float d_;
};

// -----------------------------------------------------------------------------
class CSphere
 : public CAPrimitive
{
public:
  CSphere(TVector3<float> center, float radius);

  virtual int intersect(CRay & ray, float & dist);
  virtual TVector3<float> getNormal(TVector3<float> & pos){return (pos - center_) * radiusR_;};

  TVector3<float> & getCenter(){return center_;}

private:
  TVector3<float> center_;
  float radius_;
  float radiusSq_;
  float radiusR_;
};

// -----------------------------------------------------------------------------
class CScene
{
public:
  CScene(){}

  void init();

public:
  CAPrimitive * prim_[10];
  int count_;
};


#endif