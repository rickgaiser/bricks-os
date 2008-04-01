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
 , bInitialized_(false)
{
  backBuffer_.p = 0;

  pDisplayDevice->get3DRenderer(&p3DRenderer_);
  p3DRenderer_->setSurface(&backBuffer_);
}

//---------------------------------------------------------------------------
CGLWidget::~CGLWidget()
{
  if(backBuffer_.p != 0)
    delete (uint8_t *)backBuffer_.p;
  if(p3DRenderer_ != 0)
    delete p3DRenderer_;
}

//---------------------------------------------------------------------------
bool
CGLWidget::eventRedraw(const CEventRedraw & event)
{
  if(bInitialized_ == false)
  {
    pWindow_->fill(clRed);
  }
  else
  {
    glMakeCurrent(p3DRenderer_);
    this->drawGL();
    videoManager.bitBlt(pDisplaySurface, x(), y(), width(), height(), &backBuffer_, 0, 0);
  }

  // Redraw every time!
  this->redraw();

  return true;
}

//---------------------------------------------------------------------------
bool
CGLWidget::eventResize(const CEventResize & event)
{
  if(backBuffer_.p != 0)
    delete (uint8_t *)backBuffer_.p;

  backBuffer_.p = new uint8_t[width() * height() * 2];
  backBuffer_.mode.xpitch = width();
  backBuffer_.mode.ypitch = height();
  backBuffer_.mode.width  = width();
  backBuffer_.mode.height = height();
  backBuffer_.mode.bpp    = 16;         // FIXME
  backBuffer_.mode.format = cfX1R5G5B5; // FIXME

  glMakeCurrent(p3DRenderer_);
  if(bInitialized_ == false)
  {
    this->initializeGL();
    bInitialized_ = true;
  }
  this->resizeGL(width(), height());

  return true;
}


};
