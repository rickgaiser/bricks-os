#include "matrix.h"


//-----------------------------------------------------------------------------
void
CPS2Matrix::transform(const GLfloat * v1, GLfloat * v2)
{
  asm __volatile__
  (
    "lqc2 vf4,0x0(%0)\n"
    "lqc2 vf5,0x10(%0)\n"
    "lqc2 vf6,0x20(%0)\n"
    "lqc2 vf7,0x30(%0)\n"
    "lqc2 vf8,0x0(%2)\n"
    "vmulax.xyzw ACC, vf4,vf8\n"
    "vmadday.xyzw ACC, vf5,vf8\n"
    "vmaddaz.xyzw ACC, vf6,vf8\n"
    "vmaddw.xyzw vf9,vf7,vf8\n"
    "sqc2 vf9,0x0(%1)\n"
    :
    : "r" (matrix), "r" (v1), "r" (v2)
  );
}
