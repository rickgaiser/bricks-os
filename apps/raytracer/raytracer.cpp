#include "raytracer.h"
#include "stdlib.h"


#define HORIZONTAL_ALIASING
#define SCREEN_WIDTH  8.0f
#define SCREEN_HEIGHT 6.0f
#define PIXEL_WIDTH   3.0f
#define PIXEL_HEIGHT  3.0f
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

  fStartX_ = -(SCREEN_WIDTH  / 2.0f);
  fDeltaX_ =  (SCREEN_WIDTH  / (float)iWidth_);
  fStartY_ =  (SCREEN_HEIGHT / 2.0f);
  fDeltaY_ = -(SCREEN_HEIGHT / (float)iHeight_);

  fPixelDeltaX_ = fDeltaX_ / PIXEL_WIDTH;
  fPixelDeltaY_ = fDeltaY_ / PIXEL_HEIGHT;

  // Move to the center of the pixels
  fStartX_ += (fDeltaX_ / 2.0f);
  fStartY_ += (fDeltaY_ / 2.0f);

  // Move to the right/bottom of the pixel for horizontal/vertical anti-aliasing
  fStartX_ += fPixelDeltaX_ * ((PIXEL_WIDTH  - 1.0f) / 2.0f);
  fStartY_ += fPixelDeltaY_ * ((PIXEL_HEIGHT - 1.0f) / 2.0f);
}

// -----------------------------------------------------------------------------
void
CRaytracer::render()
{
  unsigned int dataIndex(0);
  CAPrimitive * pPrim;
  CAPrimitive * pLastPrim;
  vector3f origin(0, 0, -5);
  vector3f dir;
  CRay     ray;
  color4f  col;
  float    dist;
  int      iRayCount;

  float yPos = fStartY_;
  for(int y(0); y < iHeight_; y++)
  {
    pLastPrim = NULL;
    float xPos = fStartX_;
    for(int x(0); x < iWidth_; x++)
    {
      iRayCount = 1;
      dir = vector3f(xPos, yPos, 0) - origin;
      dir.normalize();
      ray = CRay(origin, dir);
      col = color4f(0.0f, 0.0f, 0.0f, 0.0f);
      pPrim = trace(ray, col, 1, 1.0f, dist);

#ifdef HORIZONTAL_ALIASING
      if((pLastPrim != NULL) && (pPrim != pLastPrim))
      {
        pLastPrim = pPrim;
        for(int tx(0); tx < (PIXEL_WIDTH-1); tx++)
        {
          iRayCount++;
          dir = vector3f(xPos - (fPixelDeltaX_ * ((PIXEL_WIDTH-1) - tx)), yPos, 0) - origin;
          dir.normalize();
          ray = CRay(origin, dir);
          trace(ray, col, 1, 1.0f, dist);
        }
        col /= iRayCount;
      }
      if(pLastPrim == NULL)
        pLastPrim = pPrim;
#endif

      col.clamp();
      pData_[dataIndex++] = fColor_to_rgb565(col);

      xPos += fDeltaX_;
    }
    yPos += fDeltaY_;
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
CAPrimitive *
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
    }
  }

  return pPrim;
}

// -----------------------------------------------------------------------------
float
CRaytracer::calcShade(CAPrimitive * light, vector3f intersect, vector3f & dir)
{
  float shade = 0.0f;
  CAPrimitive * pPrim;
  CRay rayObjToLight;
  float lightDist;

  if(light->getType() == CAPrimitive::SPHERE)
  {
    dir = ((CSphere *)light)->getCenter() - intersect;
    lightDist = dir.length();
    dir /= lightDist;

    // Shadow ray
    vector3f origin = intersect + dir * EPSILON;
    rayObjToLight = CRay(origin, dir);

    findNearest(rayObjToLight, lightDist, pPrim);
    if(pPrim == light)
      shade = 1.0f;
  }
  else if(light->getType() == CAPrimitive::AABB)
  {
    CBox * box = (CBox *)light;
    dir = (box->getPos() + (box->getSize() * 0.5f)) - intersect;
    dir.normalize();

    // Regular grid version (banding)
    for(int x(0); x < 3; x++)
    {
      for(int y(0); y < 3; y++)
      {
        vector3f dir2 = vector3f(box->getPos().x + x * (2.0f / 3.0f), box->getPos().y, box->getPos().z + y * (2.0f / 3.0f)) - intersect;
        lightDist = dir2.length();
        dir2 /= lightDist;

        // Shadow ray
        vector3f origin = intersect + dir2 * EPSILON;
        rayObjToLight = CRay(origin, dir2);

        findNearest(rayObjToLight, lightDist, pPrim);
          if(pPrim == light)
            shade += (1.0f / 9);
      }
    }
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
