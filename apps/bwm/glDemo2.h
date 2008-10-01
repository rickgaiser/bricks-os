#ifndef GLDEMO2_H
#define GLDEMO2_H


#include "glWidget.h"
#include "vhl/fixedPoint.h"


class CGLDemo2
 : public twl::CGLWidget
{
public:
  CGLDemo2(CWidget * parent = 0);
  virtual ~CGLDemo2();

  virtual void initializeGL();
  virtual void resizeGL(int w, int h);
  virtual void drawGL();

private:
  GLfixed yrot_;
};


#endif
