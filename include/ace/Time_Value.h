#ifndef ACE_TIME_VALUE_H
#define ACE_TIME_VALUE_H


//#include <sys/time.h>


#ifndef timeval
struct timeval
{
  int tv_sec;
  int tv_usec;
};
#endif

class ACE_Time_Value
{
public:
  ACE_Time_Value();
  ACE_Time_Value(long sec, long usec = 0);
  ACE_Time_Value(const struct timeval & t);

  void set(long sec, long usec);
  void set(const timeval & t);

  unsigned long msec (void) const;
  void msec (long);

private:
  void normalize();

  timeval tv_;
};


#endif
