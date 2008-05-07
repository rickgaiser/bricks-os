#include "edge.h"
#include "fixedPoint.h"


#ifdef CONFIG_FPU
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CEdgeF::CEdgeF(uint32_t height)
 : iHeight_(height)
{
  x_  = new GLint[iHeight_];
  z_  = new GLfloat[iHeight_];
  ts_ = new GLfloat[iHeight_];
  tt_ = new GLfloat[iHeight_];
  c_  = new SColorF[iHeight_];
}

//-----------------------------------------------------------------------------
CEdgeF::~CEdgeF()
{
  delete x_;
  delete z_;
  delete ts_;
  delete tt_;
  delete c_;
}

#else
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CEdgeFx::CEdgeFx(uint32_t height)
 : iHeight_(height)
{
  x_  = new GLint[iHeight_];
  z_  = new GLfixed[iHeight_];
  ts_ = new CFixed[iHeight_];
  tt_ = new CFixed[iHeight_];
  c_  = new SColorFx[iHeight_];
}

//-----------------------------------------------------------------------------
CEdgeFx::~CEdgeFx()
{
  delete x_;
  delete z_;
  delete ts_;
  delete tt_;
  delete c_;
}

#endif // CONFIG_FPU
