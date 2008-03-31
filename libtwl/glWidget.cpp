#include "glWidget.h"
#include "GLES/gl.h"
#include "GLES/gl_extra.h"
#include "GL/glu.h"
#include "../gl/fixedPoint.h"
#include "../gl/context.h"


extern IGLESRenderer * getGLESContext();
extern void glMakeCurrent(IGLESRenderer * ctx);
extern CSurface * pDisplaySurface;


namespace twl
{


//---------------------------------------------------------------------------
void
bitBlt(CSurface * dest, int dx, int dy, int w, int h, CSurface * source, int sx, int sy)
{
  for(int y(0); y < h; y++)
    for(int x(0); x < w; x++)
      ((uint16_t *)dest->p)[(dy + y) * dest->mode.xpitch + (dx + x)] = ((uint16_t *)source->p)[(sy + y) * source->mode.xpitch + (sx + x)];
}

//---------------------------------------------------------------------------
CGLWidget::CGLWidget(CWidget * parent)
 : CWidget(parent)
 , bInitialized_(false)
{
  backBuffer_.p = 0;

  p3DRenderer_ = getGLESContext();
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
    twl::bitBlt(pDisplaySurface, x(), y(), width(), height(), &backBuffer_, 0, 0);
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
