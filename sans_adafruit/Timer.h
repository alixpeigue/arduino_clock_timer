#ifndef TIMER_H
#define TIMER_H

#include "Clock.h"

class Timer : public Clock{
    public:
  	bool ended;

	Timer(int hour = 0, int minute = 0, int second = 0);

    void tick();
  
  	bool isEnded();
  
  	void reset();
  
  	int roundedUpMinutes();

  	int getHour();

    int getMinute();
	  
};

#endif