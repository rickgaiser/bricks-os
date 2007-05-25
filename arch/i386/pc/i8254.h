// Class for handling Intel 8254 Programmable Interval Timer (PIT)
#ifndef I8254_H
#define I8254_H


#define I8254_COUNTER0_OFF 0
#define I8254_COUNTER1_OFF 1
#define I8254_COUNTER2_OFF 2
#define I8254_CONTROL_OFF 0


class CI8254
{
public:
  CI8254(unsigned int baseAddr);
  virtual ~CI8254();

private:
  unsigned int iBaseAddr_;
};


#endif
