#include "edge.h"
#include "fixedPoint.h"


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

//-----------------------------------------------------------------------------
void
CEdgeF::add(GLint x1, GLint y1, GLint x2, GLint y2)
{
  if(y1 < y2)
  {
    GLfloat x(x1);
    GLfloat mx((GLfloat)(x2 - x1) / (GLfloat)(y2 - y1));

    while(y1 < y2)
    {
      if(y1 >= iHeight_)
        break;

      if(y1 >= 0)
        x_[y1] = (GLint)x;

      x += mx;

      y1++;
    }
  }
}

//-----------------------------------------------------------------------------
void
CEdgeF::addZ(GLint x1, GLint y1, GLfloat z1, GLint x2, GLint y2, GLfloat z2)
{
  if(y1 < y2)
  {
    GLfloat x(x1);
    GLfloat mx((GLfloat)(x2 - x1) / (GLfloat)(y2 - y1));

    GLfloat mz((z2 - z1) / (GLfloat)(y2 - y1));

    while(y1 < y2)
    {
      if(y1 >= iHeight_)
        break;

      if(y1 >= 0)
      {
        x_[y1] = (GLint)x;
        z_[y1] = z1;
      }

      x  += mx;
      z1 += mz;

      y1++;
    }
  }
}

//-----------------------------------------------------------------------------
void
CEdgeF::addC(GLint x1, GLint y1, SColorF & c1, GLint x2, GLint y2, SColorF & c2)
{
  if(y1 < y2)
  {
    GLfloat x(x1);
    GLfloat mx((GLfloat)(x2 - x1) / (GLfloat)(y2 - y1));

    GLfloat r(c1.r);
    GLfloat g(c1.g);
    GLfloat b(c1.b);
    GLfloat a(c1.a);
    GLfloat mr((c2.r - c1.r) / (GLfloat)(y2 - y1));
    GLfloat mg((c2.g - c1.g) / (GLfloat)(y2 - y1));
    GLfloat mb((c2.b - c1.b) / (GLfloat)(y2 - y1));
    GLfloat ma((c2.a - c1.a) / (GLfloat)(y2 - y1));

    while(y1 < y2)
    {
      if(y1 >= iHeight_)
        break;

      if(y1 >= 0)
      {
        x_[y1]   = (GLint)x;
        c_[y1].r = r;
        c_[y1].g = g;
        c_[y1].b = b;
        c_[y1].a = a;
      }

      x += mx;
      r += mr;
      g += mg;
      b += mb;
      a += ma;

      y1++;
    }
  }
}

//-----------------------------------------------------------------------------
void
CEdgeF::addZT(GLint x1, GLint y1, GLfloat z1, GLfloat ts1, GLfloat tt1, GLint x2, GLint y2, GLfloat z2, GLfloat ts2, GLfloat tt2)
{
  if(y1 < y2)
  {
    GLfloat x(x1);
    GLfloat mx((GLfloat)(x2 - x1) / (GLfloat)(y2 - y1));

    GLfloat mz((z2 - z1) / (GLfloat)(y2 - y1));

    GLfloat ts(ts1);
    GLfloat tt(tt1);
    GLfloat mts((ts2 - ts1) / (GLfloat)(y2 - y1));
    GLfloat mtt((tt2 - tt1) / (GLfloat)(y2 - y1));

    while(y1 < y2)
    {
      if(y1 >= iHeight_)
        break;

      if(y1 >= 0)
      {
        x_[y1]  = (GLint)x;
        z_[y1]  = z1;
        ts_[y1] = ts;
        tt_[y1] = tt;
      }

      x  += mx;
      z1 += mz;
      ts += mts;
      tt += mtt;

      y1++;
    }
  }
}

//-----------------------------------------------------------------------------
void
CEdgeF::addZC(GLint x1, GLint y1, GLfloat z1, SColorF & c1, GLint x2, GLint y2, GLfloat z2, SColorF & c2)
{
  if(y1 < y2)
  {
    GLfloat x(x1);
    GLfloat mx((GLfloat)(x2 - x1) / (GLfloat)(y2 - y1));

    GLfloat mz((z2 - z1) / (GLfloat)(y2 - y1));

    GLfloat r(c1.r);
    GLfloat g(c1.g);
    GLfloat b(c1.b);
    GLfloat a(c1.a);
    GLfloat mr((c2.r - c1.r) / (GLfloat)(y2 - y1));
    GLfloat mg((c2.g - c1.g) / (GLfloat)(y2 - y1));
    GLfloat mb((c2.b - c1.b) / (GLfloat)(y2 - y1));
    GLfloat ma((c2.a - c1.a) / (GLfloat)(y2 - y1));

    while(y1 < y2)
    {
      if(y1 >= iHeight_)
        break;

      if(y1 >= 0)
      {
        x_[y1]   = (GLint)x;
        z_[y1]   = z1;
        c_[y1].r = r;
        c_[y1].g = g;
        c_[y1].b = b;
        c_[y1].a = a;
      }

      x  += mx;
      z1 += mz;
      r  += mr;
      g  += mg;
      b  += mb;
      a  += ma;

      y1++;
    }
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CEdgeFx::CEdgeFx(uint32_t height)
 : iHeight_(height)
{
  x_  = new GLint[iHeight_];
  z_  = new GLfixed[iHeight_];
  ts_ = new GLfixed[iHeight_];
  tt_ = new GLfixed[iHeight_];
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

//-----------------------------------------------------------------------------
void
CEdgeFx::add(GLint x1, GLint y1, GLint x2, GLint y2)
{
  if(y1 < y2)
  {
    GLfixed x(gl_fpfromi(x1));
    GLfixed mx(gl_fpfromi(x2 - x1) / (y2 - y1));

    while(y1 < y2)
    {
      if(y1 >= iHeight_)
        break;

      if(y1 >= 0)
        x_[y1] = gl_fptoi(x);

      x += mx;

      y1++;
    }
  }
}

//-----------------------------------------------------------------------------
void
CEdgeFx::addZ(GLint x1, GLint y1, GLfixed z1, GLint x2, GLint y2, GLfixed z2)
{
  if(y1 < y2)
  {
    GLfixed x(gl_fpfromi(x1));
    GLfixed mx(gl_fpfromi(x2 - x1) / (y2 - y1));

    GLfixed mz((z2 - z1) / (y2 - y1));

    while(y1 < y2)
    {
      if(y1 >= iHeight_)
        break;

      if(y1 >= 0)
      {
        x_[y1] = gl_fptoi(x);
        z_[y1] = z1;
      }

      x  += mx;
      z1 += mz;

      y1++;
    }
  }
}

//-----------------------------------------------------------------------------
void
CEdgeFx::addC(GLint x1, GLint y1, SColorFx & c1, GLint x2, GLint y2, SColorFx & c2)
{
  if(y1 < y2)
  {
    GLfixed x(gl_fpfromi(x1));
    GLfixed mx(gl_fpfromi(x2 - x1) / (y2 - y1));

    GLfixed r(c1.r);
    GLfixed g(c1.g);
    GLfixed b(c1.b);
    GLfixed a(c1.a);
    GLfixed mr((c2.r - c1.r) / (y2 - y1));
    GLfixed mg((c2.g - c1.g) / (y2 - y1));
    GLfixed mb((c2.b - c1.b) / (y2 - y1));
    GLfixed ma((c2.a - c1.a) / (y2 - y1));

    while(y1 < y2)
    {
      if(y1 >= iHeight_)
        break;

      if(y1 >= 0)
      {
        x_[y1]   = gl_fptoi(x);
        c_[y1].r = r;
        c_[y1].g = g;
        c_[y1].b = b;
        c_[y1].a = a;
      }

      x += mx;
      r += mr;
      g += mg;
      b += mb;
      a += ma;

      y1++;
    }
  }
}

//-----------------------------------------------------------------------------
void
CEdgeFx::addZT(GLint x1, GLint y1, GLfixed z1, GLfixed ts1, GLfixed tt1, GLint x2, GLint y2, GLfixed z2, GLfixed ts2, GLfixed tt2)
{
  if(y1 < y2)
  {
    GLfixed x(gl_fpfromi(x1));
    GLfixed mx(gl_fpfromi(x2 - x1) / (y2 - y1));

    GLfixed mz((z2 - z1) / (y2 - y1));

    GLfixed ts(ts1);
    GLfixed tt(tt1);
    GLfixed mts((ts2 - ts1) / (y2 - y1));
    GLfixed mtt((tt2 - tt1) / (y2 - y1));

    while(y1 < y2)
    {
      if(y1 >= iHeight_)
        break;

      if(y1 >= 0)
      {
        x_[y1]  = gl_fptoi(x);
        z_[y1]  = z1;
        ts_[y1] = ts;
        tt_[y1] = tt;
      }

      x  += mx;
      z1 += mz;
      ts += mts;
      tt += mtt;

      y1++;
    }
  }
}

//-----------------------------------------------------------------------------
void
CEdgeFx::addZC(GLint x1, GLint y1, GLfixed z1, SColorFx & c1, GLint x2, GLint y2, GLfixed z2, SColorFx & c2)
{
  if(y1 < y2)
  {
    GLfixed x(gl_fpfromi(x1));
    GLfixed mx(gl_fpfromi(x2 - x1) / (y2 - y1));

    GLfixed mz((z2 - z1) / (y2 - y1));

    GLfixed r(c1.r);
    GLfixed g(c1.g);
    GLfixed b(c1.b);
    GLfixed a(c1.a);
    GLfixed mr((c2.r - c1.r) / (y2 - y1));
    GLfixed mg((c2.g - c1.g) / (y2 - y1));
    GLfixed mb((c2.b - c1.b) / (y2 - y1));
    GLfixed ma((c2.a - c1.a) / (y2 - y1));

    while(y1 < y2)
    {
      if(y1 >= iHeight_)
        break;

      if(y1 >= 0)
      {
        x_[y1]   = gl_fptoi(x);
        z_[y1]   = z1;
        c_[y1].r = r;
        c_[y1].g = g;
        c_[y1].b = b;
        c_[y1].a = a;
      }

      x  += mx;
      z1 += mz;
      r  += mr;
      g  += mg;
      b  += mb;
      a  += ma;

      y1++;
    }
  }
}
