#ifndef TWL_GLWIDGET_H
#define TWL_GLWIDGET_H


#include "widget.h"
#include "kernel/videoManager.h"
#include "../gl/context.h"


namespace twl
{


class CGLWidget
 : public CWidget
{
public:
  CGLWidget(CWidget * parent = 0);
  virtual ~CGLWidget();

  virtual bool eventRedraw(const CEventRedraw & event);
  virtual bool eventResize(const CEventResize & event);

  virtual void initializeGL() = 0;
  virtual void resizeGL(int w, int h) = 0;
  virtual void drawGL() = 0;

private:
  CSurface * pBackBuffer_;
  I3DRenderer * p3DRenderer_;
  bool bInitialized_;
};


};


#endif
