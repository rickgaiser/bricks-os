#ifndef EDGE_H
#define EDGE_H


#include "GLES/gl.h"
#include "context.h"
#include "inttypes.h"


//-----------------------------------------------------------------------------
class CEdgeF
{
public:
  CEdgeF(uint32_t height);
  ~CEdgeF();

  void add  (GLint x1, GLint y1, GLint x2, GLint y2);
  void addZ (GLint x1, GLint y1, GLfloat z1, GLint x2, GLint y2, GLfloat z2);
  void addC (GLint x1, GLint y1, SColorF & c1, GLint x2, GLint y2, SColorF & c2);
  void addZC(GLint x1, GLint y1, GLfloat z1, SColorF & c1, GLint x2, GLint y2, GLfloat z2, SColorF & c2);

public:
  // Edge x
  GLint * x_;
  // Edge depth
  GLfloat * z_;
  // Edge color
  SColorF * c_;

  int32_t iHeight_;
};

//-----------------------------------------------------------------------------
class CEdgeFx
{
public:
  CEdgeFx(uint32_t height);
  ~CEdgeFx();

  void add  (GLint x1, GLint y1, GLint x2, GLint y2);
  void addZ (GLint x1, GLint y1, GLfixed z1, GLint x2, GLint y2, GLfixed z2);
  void addC (GLint x1, GLint y1, SColorFx & c1, GLint x2, GLint y2, SColorFx & c2);
  void addZC(GLint x1, GLint y1, GLfixed z1, SColorFx & c1, GLint x2, GLint y2, GLfixed z2, SColorFx & c2);

public:
  // Edge x
  GLint * x_;
  // Edge depth
  GLfixed * z_;
  // Edge color
  SColorFx * c_;

  int32_t iHeight_;
};


#endif
