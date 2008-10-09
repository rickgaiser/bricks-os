#include "raytracer.h"
#include "stdlib.h"


#define EPSILON 0.0001f
#define TRACEDEPTH 6
#define fColor_to_rgb555(c) \
  (0x8000 | \
  (((uint16_t)(c.b*31.0f) << 10) & 0x7c00) | \
  (((uint16_t)(c.g*31.0f) <<  5) & 0x03e0) | \
  (((uint16_t)(c.r*31.0f)      ) & 0x001f))
#define fColor_to_rgb565(c) \
 ((((uint16_t)(c.r*31.0f) << 11) & 0xf800) | \
  (((uint16_t)(c.g*63.0f) <<  5) & 0x07e0) | \
  (((uint16_t)(c.b*31.0f)      ) & 0x001f))


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
  vector3f origin(0, 0, -5);
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
      vector3f dir = vector3f(xPos, yPos, 0) - origin;
      dir.normalize();
      CRay ray(origin, dir);
      color4f col(0.0f, 0.0f, 0.0f, 0.0f);
      float dist;

      trace(ray, col, 1, 1.0f, dist);

      pData_[dataIndex++] = fColor_to_rgb565(col);
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
CRaytracer::trace(CRay & ray, color4f & color, int depth, float rindex, float & dist)
{
  dist = 1000000.0f;
  CAPrimitive * pPrim = NULL;

  findNearest(ray, dist, pPrim);
  if(pPrim != NULL)
  {
    if(pPrim->getLight() == true)
    {
      color += pPrim->getMaterial().getColor();
    }
    else
    {
      color4f clAmbient   (0.2f, 0.2f, 0.2f, 0.0f);
      color4f clDiffuse   (0.0f, 0.0f, 0.0f, 0.0f);
      color4f clSpecular  (0.0f, 0.0f, 0.0f, 0.0f);
      color4f clReflection(0.0f, 0.0f, 0.0f, 0.0f);

      // Intersection point with closest object
      vector3f intersect = ray.getOrigin() + (ray.getDirection() * dist);
      // Normal at the objects intersection point
      vector3f N = pPrim->getNormal(intersect);

      for(int light(0); light < scene_.lightCount_; light++)
      {
        // Normalized vector to light (relative to the objects intersection point)
        vector3f L;
        float shade;

        shade = calcShade(scene_.light_[light], intersect, L);

        if(shade > 0.0f)
        {
          // Diffuse reflection
          if(pPrim->getMaterial().getDiffuse() > 0.0f)
          {
            float diffuse = L.dotProduct(N);
            if(diffuse > 0.0f)
            {
              clDiffuse +=
//                light->getMaterial().getColor() *
                shade *
                diffuse;
            }
          }

          // Specular reflection
          if(pPrim->getMaterial().getSpecular() > 0.0f)
          {
            vector3f R = L.getInverted().getReflection(N);
            float specular = R.dotProduct(ray.getDirection());
            if(specular > 0)
            {
              specular = my_pow(specular, 10);

              clSpecular +=
//                light->getMaterial().getColor() *
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
          vector3f R = ray.getDirection().getInverted().getReflection(N);
          color4f   rcolor(0.0f, 0.0f, 0.0f, 0.0f);
          vector3f origin = intersect + R * EPSILON;
          CRay rray(origin, R);
          float rdist;
          trace(rray, rcolor, depth + 1, rindex, rdist);
          clReflection += rcolor;
        }
      }
      // Refraction
      // ...

      clDiffuse += clAmbient;
      clDiffuse.clamp();
      clSpecular.clamp();
      clReflection.clamp();
      color +=
        clDiffuse    * pPrim->getMaterial().getDiffuse()  * pPrim->getMaterial().getColor() +
        clSpecular   * pPrim->getMaterial().getSpecular() +
        clReflection * pPrim->getMaterial().getReflection();
      color.clamp();
    }
  }
}

// -----------------------------------------------------------------------------
float
CRaytracer::calcShade(CAPrimitive * light, vector3f intersect, vector3f & dir)
{
  float shade = 1.0f;

  if(light->getType() == CAPrimitive::SPHERE)
  {
    dir = ((CSphere *)light)->getCenter() - intersect;
    float lightDist = dir.length();
    dir /= lightDist;

    // Shadow ray
    vector3f origin = intersect + dir * EPSILON;
    CRay rayObjToLight(origin, dir);

    CAPrimitive * pPrim;
    findNearest(rayObjToLight, lightDist, pPrim);
    if(pPrim != light)
      shade = 0.0f;
  }

  return shade;
}

// -----------------------------------------------------------------------------
int
CRaytracer::findNearest(CRay & ray, float & dist, CAPrimitive *& prim)
{
  for(int i(0); i < scene_.primCount_; i++)
  {
    if(scene_.prim_[i]->intersect(ray, dist) == 1)
    {
      prim = scene_.prim_[i];
    }
  }

  return 0;
}
