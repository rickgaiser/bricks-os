#ifndef TIMEVALUE_H
#define TIMEVALUE_H


/**
 * \brief Time Value class
 *
 * mostly copied from ACE_Time_Value
 */
class CTimeValue
{
public:
  CTimeValue();
  CTimeValue(long sec, long usec = 0);

  void set(long sec, long usec);

  long msec() const;
  void msec(long msec);

private:
  void normalize();

  long iSec_;
  long iUSec_;
};


#endif
