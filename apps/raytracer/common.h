#ifndef COMMON_H
#define COMMON_H


#include "vhl/vector.h"
#include "vhl/color.h"


// -----------------------------------------------------------------------------
#define vector3f TVector3<float>
#define vector4f TVector4<float>
//#define color3f  TColor3<float>
#define color4f  TColor<float>


// -----------------------------------------------------------------------------
class CRay
{
public:
  CRay(){};
  CRay(vector3f & origin, vector3f & direction) : origin_(origin), direction_(direction){};

  void setOrigin   (vector3f & origin)    {origin_    = origin;}
  void setDirection(vector3f & direction) {direction_ = direction;}

  vector3f & getOrigin()    {return origin_;}
  vector3f & getDirection() {return direction_;}

private:
  vector3f origin_;
  vector3f direction_;
};


#endif
