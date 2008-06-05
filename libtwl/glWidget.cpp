#include "glWidget.h"
#include "GLES/gl.h"
#include "GLES/gl_extra.h"
#include "GL/glu.h"
#include "kernel/videoManager.h"
#include "../gl/fixedPoint.h"
#include "../gl/context.h"


extern void glMakeCurrent(I3DRenderer * ctx);
extern CAVideoDevice * pDisplayDevice;
extern CSurface      * pDisplaySurface;


namespace twl
{


//---------------------------------------------------------------------------
CGLWidget::CGLWidget(CWidget * parent)
 : CWidget(parent)
 , pBackBuffer_(0)
 , bInitialized_(false)
{
  pDisplayDevice->get3DRenderer(&p3DRenderer_);
}

//---------------------------------------------------------------------------
CGLWidget::~CGLWidget()
{
  if(pBackBuffer_ != 0)
    delete pBackBuffer_;
  if(p3DRenderer_ != 0)
    delete p3DRenderer_;
}

//---------------------------------------------------------------------------
bool
CGLWidget::eventRedraw(const CEventRedraw & event)
{
  if(bInitialized_ == false)
  {
    pWindow_->setColor(clRed);
    pWindow_->fill();
  }
  else
  {
    glMakeCurrent(p3DRenderer_);
    this->drawGL();
    videoManager.bitBlt(pDisplaySurface, rect().x(), rect().y(), rect().width(), rect().height(), pBackBuffer_, 0, 0);
  }

  // Redraw every time!
  this->redraw();

  return true;
}

//---------------------------------------------------------------------------
bool
CGLWidget::eventResize(const CEventResize & event)
{
  if(pBackBuffer_ != 0)
    delete pBackBuffer_;

  pDisplayDevice->getSurface(&pBackBuffer_, rect().width(), rect().height());
  p3DRenderer_->setSurface(pBackBuffer_);

  glMakeCurrent(p3DRenderer_);
  if(bInitialized_ == false)
  {
    this->initializeGL();
    bInitialized_ = true;
  }
  this->resizeGL(rect().width(), rect().height());

  return true;
}


};
