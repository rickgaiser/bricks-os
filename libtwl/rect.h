#ifndef TWL_RECT_H
#define TWL_RECT_H


namespace twl
{


class CRect
{
public:
  CRect();
  CRect(const CRect & rect);
  CRect(int x, int y, int width, int height);
  ~CRect();

  CRect & operator=(const CRect & rect);

  // Get sizes
  int x     () const;
  int y     () const;
  int width () const;
  int height() const;

  int left  () const;
  int top   () const;
  int right () const;
  int bottom() const;

  // Set sizes
  void x     (int x);
  void y     (int y);
  void width (int width);
  void height(int height);
  void rect  (int x, int y, int width, int height);

  bool valid();
  bool contains(const CRect & rect);
  bool contains(int x, int y);
  bool intersects(const CRect & rect);
  void clip(const CRect & rect);

private:
  int iX_;
  int iY_;
  int iWidth_;
  int iHeight_;
};


};


#include "rect.inl"


#endif
