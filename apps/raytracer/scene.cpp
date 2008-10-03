#include "scene.h"
#include "stdlib.h"
#include <math.h>


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
CSphere::CSphere(TVector3<float> center, float radius)
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
  TVector3<float> v = ray.getOrigin() - center_;
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
void
CScene::init()
{
  TColor<float> clWall   (0.6f, 0.6f, 0.6f, 1.0f); // Top/Bottom/Left/Right
  TColor<float> clBack   (0.6f, 0.6f, 0.6f, 1.0f); // Back
  TColor<float> clSphere1(1.0f, 0.0f, 0.0f, 1.0f);
  TColor<float> clSphere2(0.0f, 1.0f, 0.0f, 1.0f);
  TColor<float> clSphere3(0.0f, 0.0f, 1.0f, 1.0f);

  // ---------------------------------------------------------------
  // Lights
  // ---------------------------------------------------------------
  light_[0] = new CSphere(TVector3<float>( 0.0f,  2.5f,  3.0f), 0.1f);
  light_[1] = new CSphere(TVector3<float>( 0.0f,  2.5f,  7.0f), 0.1f);
  lightCount_ = 2;

  // ---------------------------------------------------------------
  // Primitives
  // ---------------------------------------------------------------
  // Top/Left plane
/*
  prim_[0] = new CPlaneMirror(TVector3<float>( 0.0f, -1.0f,  0.0f), 3.0f);
  prim_[1] = new CPlaneMirror(TVector3<float>( 1.0f,  0.0f,  0.0f), 3.0f);
*/
  prim_[0] = new CPlane(TVector3<float>( 0.0f, -1.0f,  0.0f), 3.0f);
  prim_[0]->getMaterial().setColor(clWall);
  prim_[0]->getMaterial().setDiffuse(1.0f);
  prim_[0]->getMaterial().setReflection(0.0f);
  prim_[1] = new CPlane(TVector3<float>( 1.0f,  0.0f,  0.0f), 3.0f);
  prim_[1]->getMaterial().setColor(clWall);
  prim_[1]->getMaterial().setDiffuse(1.0f);
  prim_[1]->getMaterial().setReflection(0.0f);
  // Bottom/Right plane
  prim_[2] = new CPlane(TVector3<float>( 0.0f,  1.0f,  0.0f), 3.0f);
  prim_[2]->getMaterial().setColor(clWall);
  prim_[2]->getMaterial().setDiffuse(1.0f);
  prim_[2]->getMaterial().setReflection(0.0f);
  prim_[3] = new CPlane(TVector3<float>(-1.0f,  0.0f,  0.0f), 3.0f);
  prim_[3]->getMaterial().setColor(clWall);
  prim_[3]->getMaterial().setDiffuse(1.0f);
  prim_[3]->getMaterial().setReflection(0.0f);
  // Back plane
  prim_[4] = new CPlane (TVector3<float>( 0.0f,  0.0f, -1.0f), 10.0f);
  prim_[4]->getMaterial().setColor(clBack);
  prim_[4]->getMaterial().setDiffuse(1.0f);
  prim_[4]->getMaterial().setReflection(0.0f);

  // Sphere
  prim_[5] = new CSphere(TVector3<float>(-1.0f,  1.0f,  6.0f),  1.0f);
  prim_[5]->getMaterial().setColor(clSphere1);
  prim_[5]->getMaterial().setDiffuse(1.0f);
  prim_[5]->getMaterial().setReflection(0.0f);
  // Sphere
  prim_[6] = new CSphere(TVector3<float>( 1.0f, -1.0f,  7.0f),  1.0f);
  prim_[6]->getMaterial().setColor(clSphere2);
  prim_[6]->getMaterial().setDiffuse(1.0f);
  prim_[6]->getMaterial().setReflection(0.0f);
  // Sphere
  prim_[7] = new CSphere(TVector3<float>( 0.5f,  0.5f,  8.0f),  1.0f);
  prim_[7]->getMaterial().setColor(clSphere3);
  prim_[7]->getMaterial().setDiffuse(1.0f);
  prim_[7]->getMaterial().setReflection(0.0f);

  primCount_ = 8;
}
