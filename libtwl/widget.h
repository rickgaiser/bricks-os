#ifndef TWL_WIDGET_H
#define TWL_WIDGET_H


#include "rect.h"
#include "event.h"
#include "eventKey.h"
#include "eventMouse.h"
#include "eventRedraw.h"
#include "windowImpl.h"
//#include <list>
#include <vector>


namespace twl
{


class CWidget
 : public IWindowEventHandler
{
public:
  CWidget(CWidget * parent = 0);
  virtual ~CWidget();

  int left();
  int top();
  int width();
  int height();

  void rect(const CRect & rect);
  void rect(int left, int top, int width, int height);
  CRect rect();
  virtual CRect rectClient();
  virtual CRect rectAbsolute();

  void insertChild(CWidget * widget);
  void removeChild(CWidget * widget);

  CWidget * locate(int x, int y);

  // Window functions
  void frame(bool frame);
  bool frame() const;

  // Events
  virtual bool event(const CEvent & event);
  virtual bool eventKey(const CEventKey & event);
  virtual bool eventMouse(const CEventMouse & event);
  virtual bool eventRedraw(const CEventRedraw & event);

  virtual void redraw();

//protected:
  CWindowImpl * pWindowImpl_;

private:
  // Tree structure
  CWidget * pParent_;
  //std::list<CWidget *> pChildren_;
  std::vector<CWidget *> pChildren_;

  // Geometry
  CRect rectRelative_;
  CRect rectAbsolute_;

  // State
  bool bNeedRedraw_;
};


};


#endif
