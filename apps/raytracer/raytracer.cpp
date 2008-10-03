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
void
CRaytracer::trace(CRay & ray, TColor<float> & color, int depth, float rindex, float & dist)
{
  dist = 1000000.0f;
  CAPrimitive * pPrim = NULL;

  for(int i(0); i < scene_.primCount_; i++)
  {
    if(scene_.prim_[i]->intersect(ray, dist) == 1)
    {
      pPrim = scene_.prim_[i];
    }
  }

  if(pPrim != NULL)
  {
    // Intersection point with closest object
    TVector3<float> intersect = ray.getOrigin() + (ray.getDirection() * dist);
    // Normal at the objects intersection point
    TVector3<float> N = pPrim->getNormal(intersect);

    for(int light(0); light < scene_.lightCount_; light++)
    {
      // Normalized vector to light (relative to the objects intersection point)
      TVector3<float> L = scene_.light_[light]->getCenter() - intersect;
      float lightDist = L.length();
      L /= lightDist;

      // Shadow ray
      float shade = 1.0f;
      TVector3<float> origin = intersect + L * EPSILON;
      CRay rayObjToLight(origin, L);
      for(int i(0); i < scene_.primCount_; i++)
      {
        if(scene_.prim_[i]->intersect(rayObjToLight, lightDist) == 1)
        {
          shade = 0.0f;
          break;
        }
      }

      if(shade > 0.0f)
      {
        // Diffuse reflection
        if(pPrim->getMaterial().getDiffuse() > 0.0f)
        {
          float diffuse = L.dotProduct(N);
          if(diffuse > 0.0f)
          {
            color +=
              pPrim->getMaterial().getColor() *
              pPrim->getMaterial().getDiffuse() *
//              light->getMaterial().getColor() *
              shade *
              diffuse;
          }
        }

        // Specular reflection
        if(pPrim->getMaterial().getSpecular() > 0.0f)
        {
          TVector3<float> R = L.getInverted().getReflection(N);
          float specular = R.dotProduct(ray.getDirection());
          if(specular > 0)
          {
            specular = my_pow(specular, 10);

            color +=
              pPrim->getMaterial().getColor() *
              pPrim->getMaterial().getSpecular() *
//              light->getMaterial().getColor() *
              shade *
              specular;
          }
        }
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
