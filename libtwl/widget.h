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

  int x();
  int y();
  int width();
  int height();

  void rect(const CRect & rect);
  void rect(int x, int y, int width, int height);
  CRect rect();
  virtual CRect rectClient();
  virtual CRect rectAbsolute();

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
  virtual bool event(const CEvent & event);
  virtual bool eventKey(const CEventKey & event);
  virtual bool eventMouse(const CEventMouse & event);
  virtual bool eventRedraw(const CEventRedraw & event);
  virtual bool eventResize(const CEventResize & event);

  virtual void redraw();

protected:
  bwm::CWindow * pWindow_;

private:
  // Tree structure
  CWidget * pParent_;
  std::vector<CWidget *> pChildren_;

  // Geometry
  CRect rectRelative_;
  CRect rectAbsolute_;

  // State
  bool bNeedRedraw_;
  bool bVisible_;
};


};


#endif
