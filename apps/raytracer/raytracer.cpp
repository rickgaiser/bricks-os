#include "raytracer.h"
#include "stdlib.h"


#define EPSILON 0.0001f
#define TRACEDEPTH 3
#define fColor_to_rgb555(c) \
  (0x8000 | \
  (((uint16_t)(c.b*31.0f) << 10) & 0x7c00) | \
  (((uint16_t)(c.g*31.0f) <<  5) & 0x03e0) | \
  (((uint16_t)(c.r*31.0f)      ) & 0x001f))


// -----------------------------------------------------------------------------
CRaytracer::CRaytracer()
 : pSurface_(NULL)
 , iWidth_(0)
 , iHeight_(0)
 , pData_(NULL)
{
  scene_.init();
}

// -----------------------------------------------------------------------------
void
CRaytracer::setSurface(CSurface * surface)
{
  pSurface_ = surface;
  iWidth_   = surface->width();
  iHeight_  = surface->height();
  pData_    = (uint16_t *)surface->p;
}

// -----------------------------------------------------------------------------
void
CRaytracer::render()
{
  TVector3<float> origin(0, 0, -5);
  unsigned int dataIndex(0);

  for(int y(0); y < iHeight_; y++)
  {
    float yPos = -(((float)y) - (iHeight_>>1) + 0.5f);
    yPos /= (iHeight_>>1);
    yPos *= 3;
    for(int x(0); x < iWidth_; x++)
    {
      float xPos = ((float)x) - (iWidth_>>1) + 0.5f;
      xPos /= (iWidth_>>1);
      xPos *= 4;
      TVector3<float> dir = TVector3<float>(xPos, yPos, 0) - origin;
      dir.normalize();
      CRay ray(origin, dir);
      TColor<float> col(0.0f, 0.0f, 0.0f, 0.0f);
      float dist;

      trace(ray, col, 1, 1.0f, dist);

      pData_[dataIndex++] = fColor_to_rgb555(col);
    }
  }
}

//-----------------------------------------------------------------------------
inline GLfloat
my_pow(GLfloat x, int y)
{
  GLfloat rv(x);
  for(int i(1); i < y; i++)
    rv *= x;
  return rv;
}
// -----------------------------------------------------------------------------
CSphere light1(TVector3<float>(0.0f, 0.0f, 3.0f), 0.1f);
// -----------------------------------------------------------------------------
void
CRaytracer::trace(CRay & ray, TColor<float> & color, int depth, float rindex, float & dist)
{
  dist = 1000000.0f;
  CAPrimitive * pPrim = NULL;

  for(int i(0); i < scene_.count_; i++)
  {
    if(scene_.prim_[i]->intersect(ray, dist) == 1)
    {
      pPrim = scene_.prim_[i];
    }
  }

  if(pPrim != NULL)
  {
    TVector3<float> intersect = ray.getOrigin() + (ray.getDirection() * dist);
    TVector3<float> L = (light1.getCenter() - intersect).normalize();
    TVector3<float> N = pPrim->getNormal(intersect);
    // Diffuse reflection
    if(pPrim->getMaterial().getDiffuse() > 0.0f)
    {
      float dot = L.dotProduct(N);
      if(dot > 0)
      {
        color +=
          pPrim->getMaterial().getColor() *
          pPrim->getMaterial().getDiffuse() *
//          light->getMaterial().getColor() *
          dot;
      }
    }
    // Specular reflection
    if(pPrim->getMaterial().getSpecular() > 0.0f)
    {
      TVector3<float> R = L.getInverted().getReflection(N);
      float dot = R.dotProduct(ray.getDirection());
      if(dot > 0)
      {
        float spec = my_pow(dot, 10);

        color +=
          pPrim->getMaterial().getColor() *
          pPrim->getMaterial().getSpecular() *
//          light->getMaterial().getColor() *
          spec;
      }
    }
    // Reflection
    if(pPrim->getMaterial().getReflection() > 0.0f)
    {
      if(depth < TRACEDEPTH)
      {
        TVector3<float> R = ray.getDirection().getInverted().getReflection(N);
        TColor<float>   rcolor(0.0f, 0.0f, 0.0f, 0.0f);
        TVector3<float> origin = intersect + R * EPSILON;
        CRay rray(origin, R);
        float rdist;
        trace(rray, rcolor, depth + 1, rindex, rdist);

        color +=
          pPrim->getMaterial().getColor() *
          pPrim->getMaterial().getReflection() *
          rcolor;
      }
    }
    color.clamp();
  }
}
