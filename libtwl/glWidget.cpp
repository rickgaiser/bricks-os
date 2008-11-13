/*
 * Bricks-OS, Operating System for Game Consoles
 * Copyright (C) 2008 Maximus32 <Maximus32@bricks-os.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA
 */


#include "glWidget.h"
#include "GL/gl.h"
#include "GL/glu.h"


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
