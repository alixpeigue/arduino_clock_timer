#include <math.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>


enum Mode {CLOCK = 5, SETTING_TIMER = 6, TIMER = 7, SETTING_CLOCK = 8};

class Button {
    public :
  
    Button(uint8_t pin) : button(pin) 
    {
        pinMode(button, INPUT_PULLUP);
        state = read();
    }
  
    bool read()
    {
        return !digitalRead(button);
    }
  
    void update()
    { 
        lastState = state;
        state = read();
    }
  
    bool risingEdge()
    {
      	if (state && !lastState){
        	lastPress = millis();
        }
        return state && !lastState;
    }
  
  	bool fallingEdge()
    {
      	return !state && lastState;
    }
  
  	bool longPress(int length){
      	if(state && millis()-lastPress>length){
          	lastPress = millis();
        	return true;
      	}
    }
  
    private:
    uint8_t button;
    bool state;
    bool lastState;
  	uint64_t lastPress;
};

class Clock {
	public:
  
  	virtual int getHour();
  	virtual int getMinute();
  
    Clock(int hour = 0, int minute = 0, int second = 0)
    {
    	seconds = (uint32_t)3600*hour+60*minute+second;
    }

    void tick()
    {
      	setSeconds(seconds+1);
    }

    int getSecond()
    {
      	return seconds-(uint32_t)getHour()*3600-getMinute()*60;
    }

    void addMinutes(int minutes)
    {
      	setSeconds(seconds+60*minutes);
    }

    int displayValue()
    {
        return getHour()*100+getMinute();
    }

    //protected:

    void setSeconds(uint32_t val)
    {
      	uint32_t day = (uint32_t)86400;
      	seconds = (val%day+day)%day;
    }

    uint32_t seconds;
};

int Clock::getHour()
{
   return seconds/3600;
}

int Clock::getMinute()
{
   return (seconds-(uint32_t)getHour()*3600)/60;
}

class Timer : public Clock{
    public:
  	bool ended;
	Timer(int hour = 0, int minute = 0, int second = 0) : Clock(hour, minute, second)
    {
      	ended = false;
    }

    void tick()
    {
      	if(seconds != 0){
            setSeconds(seconds-1);
          	if (seconds == 0){
            	ended = true;
            }
        }
    }
  
  	bool isEnded()
    {
      	return ended;
    }
  
  	void reset(){
      	seconds = 0;
      	ended = false;
    }
  
  	int roundedUpMinutes()
    {
      	return ceil((double)seconds/60);
    }
  
  	int getHour()
    {
      	return roundedUpMinutes()/60;
    }

    int getMinute()
    {
      	return roundedUpMinutes()-getHour()*60;
    }
	  
};

class Relay {
  	public:
  	int pin;
  	bool state;
  
  	Relay(int relayPin, bool startState) : pin(relayPin), state(startState)
  	{
    	pinMode(pin, OUTPUT);
      	digitalWrite(pin, state);
    }
  
  	void write(bool value)
    {
      	if(value != state){
          	state = value;
        	digitalWrite(pin, state); 
        }
    }
};
  	

#define DISPLAY_ADDRESS   112

Relay relay(9, true);

Button plus(4);
Button minus(2);
Button mode(3);

Adafruit_7segment clockDisplay = Adafruit_7segment();

Mode currentMode = CLOCK;

uint64_t start;

bool blinkColon = false;

Clock clock(11, 55, 0);

Timer timer;

void updateButtons(){
    plus.update();
    minus.update();
    mode.update();
}

void updateMode(){
    if(mode.fallingEdge()) {
      	digitalWrite(currentMode, LOW);
        switch(currentMode){
            case CLOCK :
      	        currentMode = SETTING_TIMER;
      	        break;
            case SETTING_TIMER :
      	        currentMode = TIMER;
                break;
            case TIMER :
                currentMode = CLOCK;
          		timer.reset();
                break;
            /*case SETTING_CLOCK :
                currentMode = CLOCK;
                break;*/
        }
      	digitalWrite(currentMode, HIGH);
        display();
    }
  	if(mode.longPress(1000)) {
      	digitalWrite(currentMode, LOW);
        switch(currentMode){
          	case CLOCK :
              	currentMode = SETTING_CLOCK;
              	break;
        	case SETTING_CLOCK :
              	currentMode = CLOCK;
              	break;
        }
      	digitalWrite(currentMode, HIGH);
    }
}

void changeData(){
    if(currentMode==SETTING_TIMER){
        if(plus.risingEdge()){
            timer.addMinutes(1);
        } else if(minus.risingEdge()){
            timer.addMinutes(-1);
        }
    }else if(currentMode==SETTING_CLOCK){
        if(plus.risingEdge()){
            clock.addMinutes(1);
        } else if(minus.risingEdge()){
            clock.addMinutes(-1);
        }
    }
}
        

void display(){
    if(currentMode == CLOCK || currentMode == SETTING_CLOCK){

        clockDisplay.print(clock.displayValue(), DEC);

        if (clock.getHour() == 0) {
            // Pad hour 0.
            clockDisplay.writeDigitNum(1, 0);
            // Also pad when the 10's minute is 0 and should be padded.
            if (clock.getMinute() < 10) {
                clockDisplay.writeDigitNum(3, 0);
            }
        }
    } else if (currentMode == TIMER || currentMode == SETTING_TIMER) {

        clockDisplay.print(timer.displayValue(), DEC);
      	Serial.println(timer.seconds);

        if (timer.getHour() == 0) {
            // Pad hour 0.
            clockDisplay.writeDigitNum(1, 0);
            // Also pad when the 10's minute is 0 and should be padded.
            if (timer.getMinute()< 10) {
                clockDisplay.writeDigitNum(3, 0);
            }
        }
    }
    
    clockDisplay.drawColon(blinkColon);
    
    clockDisplay.writeDisplay();
}

void onTime(){
    blinkColon = !blinkColon;
    clock.tick();
    if(currentMode==TIMER){
        timer.tick();
    }
}


void setup() {
  
  	pinMode(CLOCK, OUTPUT);
  	pinMode(TIMER, OUTPUT);
  	pinMode(SETTING_TIMER, OUTPUT);
  	pinMode(SETTING_CLOCK, OUTPUT);
  
    // Setup Serial port to print debug output.
    Serial.begin(115200);
    Serial.println("Clock starting!");

    // Setup the display.
    clockDisplay.begin(DISPLAY_ADDRESS);
    
    start = millis();
    display();
  
  	digitalWrite(currentMode, HIGH);
  
}

void loop() {
    updateButtons();
    // Loop function runs over and over again to implement the clock logic.
    //currentMode = updateMode(currentMode);
 	relay.write(!timer.isEnded());
    updateMode();
    changeData();
    //Serial.println(currentMode);
    // Pause for a second for time to elapse.  This value is in milliseconds
    // so 1000 milliseconds = 1 second.
    if(millis()-start>10){
        start = millis();
        onTime();
        display();
      	//Serial.println(currentMode);
      	//Serial.println(timer.seconds);
    }
}