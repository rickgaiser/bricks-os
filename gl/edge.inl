#include "edge.h"
#include "fixedPoint.h"


// Floating Point Macros
#define DELTA_F_Y() \
  GLfloat dy(1.0f / (GLfloat)(y2 - y1))
#define INTERPOLATE_F_X() \
  GLfloat x((GLfloat)x1); \
  GLfloat mx((GLfloat)(x2 - x1) * dy)
#define INTERPOLATE_F_Z() \
  GLfloat mz((z2 - z1) * dy)
#define INTERPOLATE_F_C() \
  GLfloat r(c1.r); \
  GLfloat g(c1.g); \
  GLfloat b(c1.b); \
  GLfloat a(c1.a); \
  GLfloat mr((c2.r - c1.r) * dy); \
  GLfloat mg((c2.g - c1.g) * dy); \
  GLfloat mb((c2.b - c1.b) * dy); \
  GLfloat ma((c2.a - c1.a) * dy)
#define INTERPOLATE_F_T() \
  GLfloat ts(ts1); \
  GLfloat tt(tt1); \
  GLfloat mts((ts2 - ts1) * dy); \
  GLfloat mtt((tt2 - tt1) * dy)
#define STORE_F_X() \
  x_[y1] = (GLint)x
#define STORE_F_Z() \
  z_[y1] = z1
#define STORE_F_C() \
  r_[y1] = r; \
  g_[y1] = g; \
  b_[y1] = b; \
  a_[y1] = a
#define STORE_F_T() \
  ts_[y1] = ts; \
  tt_[y1] = tt

// Fixed Point Macros
#define DELTA_FX_Y() \
  CFixed dy = CFixed(1) / (y2 - y1)
#define INTERPOLATE_FX_X() \
  CFixed x  = x1; \
  CFixed mx = CFixed(x2 - x1).ipMul(dy)
#define INTERPOLATE_FX_Z() \
  CFixed mz = (z2 - z1).ipMul(dy)
#define INTERPOLATE_FX_C() \
  CFixed r(c1.r); \
  CFixed g(c1.g); \
  CFixed b(c1.b); \
  CFixed a(c1.a); \
  CFixed mr((c2.r - c1.r).ipipMul(dy)); \
  CFixed mg((c2.g - c1.g).ipipMul(dy)); \
  CFixed mb((c2.b - c1.b).ipipMul(dy)); \
  CFixed ma((c2.a - c1.a).ipipMul(dy))
#define INTERPOLATE_FX_T() \
  CFixed ts(ts1); \
  CFixed tt(tt1); \
  CFixed mts = (ts2 - ts1).ipMul(dy); \
  CFixed mtt = (tt2 - tt1).ipMul(dy)
#define STORE_FX_X() \
  x_[y1] = x
#define STORE_FX_Z() \
  z_[y1] = z1
#define STORE_FX_C() \
  r_[y1] = r; \
  g_[y1] = g; \
  b_[y1] = b; \
  a_[y1] = a
#define STORE_FX_T() \
  ts_[y1] = ts; \
  tt_[y1] = tt

// Common Macros
#define INCREMENT_X() \
  x += mx
#define INCREMENT_Z() \
  z1 += mz
#define INCREMENT_C() \
  r += mr; \
  g += mg; \
  b += mb; \
  a += ma
#define INCREMENT_T() \
  ts += mts; \
  tt += mtt


#ifdef CONFIG_FPU
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline
CEdgeF::CEdgeF(uint32_t height)
 : iHeight_(height)
{
  x_  = new GLint[iHeight_];
  z_  = new GLfloat[iHeight_];
  ts_ = new GLfloat[iHeight_];
  tt_ = new GLfloat[iHeight_];
  r_  = new GLfloat[iHeight_];
  g_  = new GLfloat[iHeight_];
  b_  = new GLfloat[iHeight_];
  a_  = new GLfloat[iHeight_];
}

//-----------------------------------------------------------------------------
inline
CEdgeF::~CEdgeF()
{
  delete x_;
  delete z_;
  delete ts_;
  delete tt_;
  delete r_;
  delete g_;
  delete b_;
  delete a_;
}

//-----------------------------------------------------------------------------
inline void
CEdgeF::add(GLint x1, GLint y1, GLint x2, GLint y2)
{
  if(y1 < y2)
  {
    DELTA_F_Y();
    INTERPOLATE_F_X();

    for(;y1 < y2; y1++)
    {
      if(y1 >= iHeight_)
        break;

      if(y1 >= 0)
      {
        STORE_F_X();
      }
      INCREMENT_X();
    }
  }
}

//-----------------------------------------------------------------------------
inline void
CEdgeF::addZ(GLint x1, GLint y1, GLfloat z1, GLint x2, GLint y2, GLfloat z2)
{
  if(y1 < y2)
  {
    DELTA_F_Y();
    INTERPOLATE_F_X();
    INTERPOLATE_F_Z();

    for(;y1 < y2; y1++)
    {
      if(y1 >= iHeight_)
        break;

      if(y1 >= 0)
      {
        STORE_F_X();
        STORE_F_Z();
      }
      INCREMENT_X();
      INCREMENT_Z();
    }
  }
}

//-----------------------------------------------------------------------------
inline void
CEdgeF::addC(GLint x1, GLint y1, SColorF & c1, GLint x2, GLint y2, SColorF & c2)
{
  if(y1 < y2)
  {
    DELTA_F_Y();
    INTERPOLATE_F_X();
    INTERPOLATE_F_C();

    for(;y1 < y2; y1++)
    {
      if(y1 >= iHeight_)
        break;

      if(y1 >= 0)
      {
        STORE_F_X();
        STORE_F_C();
      }
      INCREMENT_X();
      INCREMENT_C();
    }
  }
}

//-----------------------------------------------------------------------------
inline void
CEdgeF::addZC(GLint x1, GLint y1, GLfloat z1, SColorF & c1, GLint x2, GLint y2, GLfloat z2, SColorF & c2)
{
  if(y1 < y2)
  {
    DELTA_F_Y();
    INTERPOLATE_F_X();
    INTERPOLATE_F_Z();
    INTERPOLATE_F_C();

    for(;y1 < y2; y1++)
    {
      if(y1 >= iHeight_)
        break;

      if(y1 >= 0)
      {
        STORE_F_X();
        STORE_F_Z();
        STORE_F_C();
      }
      INCREMENT_X();
      INCREMENT_Z();
      INCREMENT_C();
    }
  }
}

//-----------------------------------------------------------------------------
inline void
CEdgeF::addT(GLint x1, GLint y1, GLfloat ts1, GLfloat tt1, GLint x2, GLint y2, GLfloat ts2, GLfloat tt2)
{
  if(y1 < y2)
  {
    DELTA_F_Y();
    INTERPOLATE_F_X();
    INTERPOLATE_F_T();

    for(;y1 < y2; y1++)
    {
      if(y1 >= iHeight_)
        break;

      if(y1 >= 0)
      {
        STORE_F_X();
        STORE_F_T();
      }

      INCREMENT_X();
      INCREMENT_T();
    }
  }
}

//-----------------------------------------------------------------------------
inline void
CEdgeF::addZT(GLint x1, GLint y1, GLfloat z1, GLfloat ts1, GLfloat tt1, GLint x2, GLint y2, GLfloat z2, GLfloat ts2, GLfloat tt2)
{
  if(y1 < y2)
  {
    DELTA_F_Y();
    INTERPOLATE_F_X();
    INTERPOLATE_F_Z();
    INTERPOLATE_F_T();

    for(;y1 < y2; y1++)
    {
      if(y1 >= iHeight_)
        break;

      if(y1 >= 0)
      {
        STORE_F_X();
        STORE_F_Z();
        STORE_F_T();
      }

      INCREMENT_X();
      INCREMENT_Z();
      INCREMENT_T();
    }
  }
}

#else
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline
CEdgeFx::CEdgeFx(uint32_t height)
 : iHeight_(height)
{
  x_  = new GLint[iHeight_];
  z_  = new CFixed[iHeight_];
  ts_ = new CFixed[iHeight_];
  tt_ = new CFixed[iHeight_];
  r_  = new CFixed[iHeight_];
  g_  = new CFixed[iHeight_];
  b_  = new CFixed[iHeight_];
  a_  = new CFixed[iHeight_];
}

//-----------------------------------------------------------------------------
inline
CEdgeFx::~CEdgeFx()
{
  delete x_;
  delete z_;
  delete ts_;
  delete tt_;
  delete r_;
  delete g_;
  delete b_;
  delete a_;
}

//-----------------------------------------------------------------------------
inline void
CEdgeFx::add(GLint x1, GLint y1, GLint x2, GLint y2)
{
  if(y1 < y2)
  {
    DELTA_FX_Y();
    INTERPOLATE_FX_X();

    for(;y1 < y2; y1++)
    {
      if(y1 >= iHeight_)
        break;
      if(y1 >= 0)
      {
        STORE_FX_X();
      }
      INCREMENT_X();
    }
  }
}

//-----------------------------------------------------------------------------
inline void
CEdgeFx::addZ(GLint x1, GLint y1, CFixed z1, GLint x2, GLint y2, CFixed z2)
{
  if(y1 < y2)
  {
    DELTA_FX_Y();
    INTERPOLATE_FX_X();
    INTERPOLATE_FX_Z();

    for(;y1 < y2; y1++)
    {
      if(y1 >= iHeight_)
        break;
      if(y1 >= 0)
      {
        STORE_FX_X();
        STORE_FX_Z();
      }
      INCREMENT_X();
      INCREMENT_Z();
    }
  }
}

//-----------------------------------------------------------------------------
inline void
CEdgeFx::addC(GLint x1, GLint y1, SColorFx & c1, GLint x2, GLint y2, SColorFx & c2)
{
  if(y1 < y2)
  {
    DELTA_FX_Y();
    INTERPOLATE_FX_X();
    INTERPOLATE_FX_C();

    for(;y1 < y2; y1++)
    {
      if(y1 >= iHeight_)
        break;

      if(y1 >= 0)
      {
        STORE_FX_X();
        STORE_FX_C();
      }

      INCREMENT_X();
      INCREMENT_C();
    }
  }
}

//-----------------------------------------------------------------------------
inline void
CEdgeFx::addZC(GLint x1, GLint y1, CFixed z1, SColorFx & c1, GLint x2, GLint y2, CFixed z2, SColorFx & c2)
{
  if(y1 < y2)
  {
    DELTA_FX_Y();
    INTERPOLATE_FX_X();
    INTERPOLATE_FX_Z();
    INTERPOLATE_FX_C();

    for(;y1 < y2; y1++)
    {
      if(y1 >= iHeight_)
        break;

      if(y1 >= 0)
      {
        STORE_FX_X();
        STORE_FX_Z();
        STORE_FX_C();
      }

      INCREMENT_X();
      INCREMENT_Z();
      INCREMENT_C();
    }
  }
}

//-----------------------------------------------------------------------------
inline void
CEdgeFx::addT(GLint x1, GLint y1, CFixed ts1, CFixed tt1, GLint x2, GLint y2, CFixed ts2, CFixed tt2)
{
  if(y1 < y2)
  {
    DELTA_FX_Y();
    INTERPOLATE_FX_X();
    INTERPOLATE_FX_T();

    for(;y1 < y2; y1++)
    {
      if(y1 >= iHeight_)
        break;

      if(y1 >= 0)
      {
        STORE_FX_X();
        STORE_FX_T();
      }

      INCREMENT_X();
      INCREMENT_T();
    }
  }
}

//-----------------------------------------------------------------------------
inline void
CEdgeFx::addZT(GLint x1, GLint y1, CFixed z1, CFixed ts1, CFixed tt1, GLint x2, GLint y2, CFixed z2, CFixed ts2, CFixed tt2)
{
  if(y1 < y2)
  {
    DELTA_FX_Y();
    INTERPOLATE_FX_X();
    INTERPOLATE_FX_Z();
    INTERPOLATE_FX_T();

    for(;y1 < y2; y1++)
    {
      if(y1 >= iHeight_)
        break;

      if(y1 >= 0)
      {
        STORE_FX_X();
        STORE_FX_Z();
        STORE_FX_T();
      }

      INCREMENT_X();
      INCREMENT_Z();
      INCREMENT_T();
    }
  }
}

#endif // CONFIG_FPU
