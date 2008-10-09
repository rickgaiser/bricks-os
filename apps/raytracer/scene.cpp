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
  pPrim = new CSphere(vector3f( 0.0f,  2.5f,  5.0f), 0.2f);
  pPrim->getMaterial().setColor(clLight);
  pPrim->setLight(true);
  addLight(pPrim);
  //pPrim = new CSphere(vector3f( 0.0f,  2.5f,  9.0f), 0.2f);
  //pPrim->setLight(true);
  //addLight(pPrim);

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
