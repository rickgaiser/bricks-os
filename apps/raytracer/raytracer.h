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
  void trace(CRay & ray, TColor<float> & color, int depth, float rindex, float & dist);
  float calcShade(CAPrimitive * light, TVector3<float> ip, TVector3<float> & dir);

private:
  CSurface * pSurface_;
  int iWidth_;
  int iHeight_;
  uint16_t * pData_;

  CScene scene_;
};


#endif
