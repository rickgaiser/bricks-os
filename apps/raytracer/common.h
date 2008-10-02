#ifndef COMMON_H
#define COMMON_H


#include "vhl/vector.h"


// -----------------------------------------------------------------------------
class CRay
{
public:
  CRay(){};
  CRay(TVector3<float> & origin, TVector3<float> & direction) : origin_(origin), direction_(direction){};

  void setOrigin   (TVector3<float> & origin)    {origin_    = origin;}
  void setDirection(TVector3<float> & direction) {direction_ = direction;}

  TVector3<float> & getOrigin()    {return origin_;}
  TVector3<float> & getDirection() {return direction_;}

private:
  TVector3<float> origin_;
  TVector3<float> direction_;
};


#endif
