#ifndef MATH_H
#define MATH_H


#include "_math.h"
#include "inttypes.h"


#ifdef __cplusplus
extern "C"
{
#endif


extern const float sinTable[1024];
extern const float cosTable[1024];
extern const float tanTable[1024];


#define ANGLE_TO_INDEX(a) ((uint32_t)((a) * (1024.0 / (2.0 * M_PI))) & 0x000003ff)


inline double sin (double angle){return sinTable[ANGLE_TO_INDEX(angle)];}
inline float  sinf(float  angle){return sinTable[ANGLE_TO_INDEX(angle)];}
inline double cos (double angle){return cosTable[ANGLE_TO_INDEX(angle)];}
inline float  cosf(float  angle){return cosTable[ANGLE_TO_INDEX(angle)];}
inline double tan (double angle){return tanTable[ANGLE_TO_INDEX(angle)];}
inline float  tanf(float  angle){return tanTable[ANGLE_TO_INDEX(angle)];}
double sqrt (double x);
float  sqrtf(float  x);


#ifdef __cplusplus
}
#endif


#endif
