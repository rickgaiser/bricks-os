#ifndef TWL_RECT_H
#define TWL_RECT_H


namespace twl
{


class CRect
{
public:
  CRect();
  CRect(int left, int top, int width, int height);
  virtual ~CRect();

  // Get sizes
  int left  () const;
  int top   () const;
  int right () const;
  int bottom() const;
  int width () const;
  int height() const;

  // Set sizes
  void left  (int left);
  void top   (int top);
  void right (int right);
  void bottom(int bottom);
  void width (int width);
  void height(int height);

  void move  (int right, int down);
  void moveUp(int pixels);
  void moveDown(int pixels);
  void moveLeft(int pixels);
  void moveRight(int pixels);
  void rect  (int left, int top, int width, int height);

  bool valid();
  bool contains(const CRect & rect);
  bool contains(int x, int y);
  bool intersects(const CRect & rect);
  void clip(const CRect & rect);

private:
  int iLeft_;
  int iTop_;
  int iWidth_;
  int iHeight_;
};


};


#endif
