#ifndef MATH_H
#define MATH_H


#include "_math.h"
#include "inttypes.h"


extern const float sinTable[1024];
extern const float cosTable[1024];
extern const float tanTable[1024];


#define ANGLE_TO_INDEX(a) ((uint32_t)((a) * (1024.0 / (2.0 * M_PI))) & 0x000003ff)


inline double sin (double angle){return sinTable[ANGLE_TO_INDEX(angle)];}
inline double cos (double angle){return cosTable[ANGLE_TO_INDEX(angle)];}
inline double tan (double angle){return tanTable[ANGLE_TO_INDEX(angle)];}
inline double sqrt(double value){return 1.0f;}


#endif
