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


#ifndef TWL_WIDGET_H
#define TWL_WIDGET_H


#include "rect.h"
#include "event.h"
#include "eventKey.h"
#include "eventMouse.h"
#include "eventRedraw.h"
#include "eventResize.h"
#include "window.h"
#include "vector"


namespace twl
{


class CWidget
 : public IWindowEventHandler
{
public:
  CWidget(CWidget * parent = 0);
  virtual ~CWidget();

  void rect(const CRect & rect);
  void rect(int x, int y, int width, int height);

  const CRect & rect();
  const CRect & rectClient();
  const CRect & rectAbsolute();
  const CRect & rectAbsoluteClient();

  void insertChild(CWidget * widget);
  void removeChild(CWidget * widget);

  CWidget * locate(int x, int y);

  // Window functions
  void focus(bool focus);
  bool focus() const;
  void frame(bool frame);
  bool frame() const;

  // Widget functions
  void visible(bool visible);
  bool visible() const;

  // Events
  virtual bool event      (const CEvent       & event);
  virtual bool eventKey   (const CEventKey    & event);
  virtual bool eventMouse (const CEventMouse  & event);
  virtual bool eventRedraw(const CEventRedraw & event);
  virtual bool eventResize(const CEventResize & event);

  virtual void redraw();

protected:
  bwm::CWindow * pWindow_;

  // Geometry
  CRect rectRelative_;
  CRect rectRelativeClient_;
  CRect rectAbsolute_;
  CRect rectAbsoluteClient_;

private:
  // Tree structure
  CWidget * pParent_;
  std::vector<CWidget *> pChildren_;

  // State
  bool bNeedRedraw_;
  bool bVisible_;
};


};


#endif
