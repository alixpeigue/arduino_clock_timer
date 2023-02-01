#ifndef CLOCK_H
#define CLOCK_H

class Clock {
	public:
  
  	virtual int getHour();
  	virtual int getMinute();
  
    Clock(int hour = 0, int minute = 0, int second = 0);

    void tick();

    int getSecond();

    void addMinutes(int minutes);

    int displayValue();

    protected:

    void setSeconds(int32_t val);

    uint32_t seconds;
};

#endif