#ifndef RAYTRACER_H
#define RAYTRACER_H


#include "kernel/videoManager.h"
#include "vhl/color.h"
#include "scene.h"


// -----------------------------------------------------------------------------
class CRaytracer
{
public:
  CRaytracer();

  void setSurface(CSurface * surface);
  void render();

private:
  CAPrimitive * trace(CRay & ray, color4f & color, int depth, float rindex, float & dist);
  float calcShade(CAPrimitive * light, vector3f ip, vector3f & dir);
  int findNearest(CRay & ray, float & dist, CAPrimitive *& prim);

private:
  CSurface * pSurface_;
  int iWidth_;
  int iHeight_;
  uint16_t * pData_;

  float fStartX_;
  float fDeltaX_;
  float fPixelDeltaX_;
  float fStartY_;
  float fDeltaY_;
  float fPixelDeltaY_;

  CScene scene_;
};


#endif
