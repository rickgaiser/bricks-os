#ifndef GLDEMO1_H
#define GLDEMO1_H


#include "glWidget.h"


class CGLDemo1
 : public twl::CGLWidget
{
public:
  CGLDemo1(CWidget * parent = 0);
  virtual ~CGLDemo1();

  virtual void initializeGL();
  virtual void resizeGL(int w, int h);
  virtual void drawGL();
};


#endif
