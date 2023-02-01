#include <math.h>
#include "Timer.h"

Timer::Timer(int hour = 0, int minute = 0, int second = 0) : Clock(hour, minute, second)
{
    ended = false;
}

void Timer::tick()
{
  	if(seconds != 0){
        setSeconds(seconds-1);
      	if (seconds == 0){
        	ended = true;
        }
    }
}

bool Timer::isEnded()
{
  	return ended;
}

void Timer::reset(){
  	seconds = 0;
  	ended = false;
}

int Timer::roundedUpMinutes()
{
  	return ceil((double)seconds/60);
}

int Timer::getHour()
{
    return roundedUpMinutes()/60%24;
}

int Timer::getMinute()
{
  	return (roundedUpMinutes()-getHour()*60)%60;
}