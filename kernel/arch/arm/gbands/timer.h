#ifndef DSTIMER_H
#define DSTIMER_H


void setTimerFrequency(int timerId, float hz, bool reload = false);
void setTimerInterval(int timerId, unsigned int us, bool reload = false);
void resetTimer(int timerId);


#endif
