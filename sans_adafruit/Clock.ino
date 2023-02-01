#include "Clock.h"

Clock::Clock(int hour = 0, int minute = 0, int second = 0)
{
	seconds = (uint32_t)3600*hour+60*minute+second;
}

void Clock::tick()
{
  	setSeconds(seconds+1);
}

int Clock::getSecond()
{
  	return seconds-(uint32_t)getHour()*3600-getMinute()*60;
}

void Clock::addMinutes(int minutes)
{
  	setSeconds(seconds+60*minutes);
}

int Clock::displayValue()
{
    return getHour()*100+getMinute();
}


void Clock::setSeconds(int32_t val)
{
  	int32_t day = (int32_t)86400;
    Serial.println("Calculs !");
    Serial.println(seconds);
    Serial.println(val);
    Serial.println(val%day);
  	seconds = (val%day+day)%day;
    Serial.println(seconds);
}

int Clock::getHour()
{
   return seconds/3600;
}

int Clock::getMinute()
{
   return (seconds-(uint32_t)getHour()*3600)/60;
}
