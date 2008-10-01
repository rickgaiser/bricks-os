#include "edge.h"
#include "vhl/fixedPoint.h"


// Floating Point Macros
#define DELTA_F_Y() \
  GLfloat dy  = 1.0f / (GLfloat)(to.sy - from.sy)
#define INTERPOLATE_F_X() \
  GLfloat x   = (GLfloat)from.sx; \
  GLfloat mx  = (GLfloat)(to.sx - from.sx) * dy
#define INTERPOLATE_F_Z() \
  GLfloat z   = from.ve[2]; \
  GLfloat mz  = ((to.ve[2] - from.ve[2]) * dy)
#define INTERPOLATE_F_C() \
  GLfloat r   = from.cl.r; \
  GLfloat g   = from.cl.g; \
  GLfloat b   = from.cl.b; \
  GLfloat a   = from.cl.a; \
  GLfloat mr  = (to.cl.r - from.cl.r) * dy; \
  GLfloat mg  = (to.cl.g - from.cl.g) * dy; \
  GLfloat mb  = (to.cl.b - from.cl.b) * dy; \
  GLfloat ma  = (to.cl.a - from.cl.a) * dy
#define INTERPOLATE_F_T() \
  GLfloat ts  = from.t[0]; \
  GLfloat tt  = from.t[1]; \
  GLfloat mts = (to.t[0] - from.t[0]) * dy; \
  GLfloat mtt = (to.t[1] - from.t[1]) * dy
#define STORE_F_X() \
  x_[y1] = (GLint)x
#define STORE_F_Z() \
  z_[y1] = z
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
  CFixed dy  = CFixed(1) / (to.sy - from.sy)
#define INTERPOLATE_FX_X() \
  CFixed x   = from.sx; \
  CFixed mx  = CFixed(to.sx - from.sx).ipMul(dy)
#define INTERPOLATE_FX_Z() \
  CFixed z   = from.ve[2]; \
  CFixed mz  = (to.ve[2] - from.ve[2]).ipMul(dy)
#define INTERPOLATE_FX_C() \
  CFixed r   = from.cl.r; \
  CFixed g   = from.cl.g; \
  CFixed b   = from.cl.b; \
  CFixed a   = from.cl.a; \
  CFixed mr  = (to.cl.r - from.cl.r).ipipMul(dy); \
  CFixed mg  = (to.cl.g - from.cl.g).ipipMul(dy); \
  CFixed mb  = (to.cl.b - from.cl.b).ipipMul(dy); \
  CFixed ma  = (to.cl.a - from.cl.a).ipipMul(dy)
#define INTERPOLATE_FX_T() \
  CFixed ts  = from.t[0]; \
  CFixed tt  = from.t[1]; \
  CFixed mts = (to.t[0] - from.t[0]).ipMul(dy); \
  CFixed mtt = (to.t[1] - from.t[1]).ipMul(dy)
#define STORE_FX_X() \
  x_[y1] = x
#define STORE_FX_Z() \
  z_[y1] = z
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
  z += mz
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
CEdgeF::add(const SVertexF & from, const SVertexF & to)
{
  GLint y1 = from.sy;
  GLint y2 = to.sy;

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
CEdgeF::addZ(const SVertexF & from, const SVertexF & to)
{
  GLint y1 = from.sy;
  GLint y2 = to.sy;

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
CEdgeF::addC(const SVertexF & from, const SVertexF & to)
{
  GLint y1 = from.sy;
  GLint y2 = to.sy;

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
CEdgeF::addZC(const SVertexF & from, const SVertexF & to)
{
  GLint y1 = from.sy;
  GLint y2 = to.sy;

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
CEdgeF::addT(const SVertexF & from, const SVertexF & to)
{
  GLint y1 = from.sy;
  GLint y2 = to.sy;

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
CEdgeF::addZT(const SVertexF & from, const SVertexF & to)
{
  GLint y1 = from.sy;
  GLint y2 = to.sy;

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
CEdgeFx::add(const SVertexFx & from, const SVertexFx & to)
{
  GLint y1 = from.sy;
  GLint y2 = to.sy;

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
CEdgeFx::addZ(const SVertexFx & from, const SVertexFx & to)
{
  GLint y1 = from.sy;
  GLint y2 = to.sy;

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
CEdgeFx::addC(const SVertexFx & from, const SVertexFx & to)
{
  GLint y1 = from.sy;
  GLint y2 = to.sy;

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
CEdgeFx::addZC(const SVertexFx & from, const SVertexFx & to)
{
  GLint y1 = from.sy;
  GLint y2 = to.sy;

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
CEdgeFx::addT(const SVertexFx & from, const SVertexFx & to)
{
  GLint y1 = from.sy;
  GLint y2 = to.sy;

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
CEdgeFx::addZT(const SVertexFx & from, const SVertexFx & to)
{
  GLint y1 = from.sy;
  GLint y2 = to.sy;

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
