#include <Wire.h>

#include <SevSeg.h>

#include "Button.h"
#include "Clock.h"
#include "Timer.h"
#include "Relay.h"

enum Mode {CLOCK = 5, SETTING_TIMER = 6, TIMER = 7, SETTING_CLOCK = 8, TIMER_ENDED = 9};
  	
SevSeg sevseg;

Button plus(2);
Button minus(4);
Button mode(3);

Mode currentMode = CLOCK;

uint64_t start;

bool blinkColon = false;

Clock clock;

Timer timer;

Relay relay(A5, false);

void displayTime(int16_t time, bool point = false){
    if(point){
        sevseg.setNumberF((float)time/100, 2);
    } else {
        sevseg.setNumber(time);
    }
}

void updateButtons(){
    plus.update();
    minus.update();
    mode.update();
}

void updateMode(){
    if(mode.risingEdge()) {
      	digitalWrite(currentMode, LOW);
        switch(currentMode){
            case CLOCK :
      	        currentMode = TIMER /*SETTING_TIMER*/;
      	        break;
            // case SETTING_TIMER :
      	    //     currentMode = TIMER;
            //     break;
            case TIMER :
                currentMode = CLOCK;
                break;
            case SETTING_CLOCK :
                currentMode = CLOCK;
                break;
            case TIMER_ENDED :
                currentMode = CLOCK;
                break;
        }
      	digitalWrite(currentMode, HIGH);
        display();
    } else if (currentMode == CLOCK && plus.pressed() && minus.pressed()){
        currentMode = SETTING_CLOCK;
    } else if (timer.isEnded()) {
        currentMode = TIMER_ENDED;
    } else if (currentMode == TIMER_ENDED){
        if(plus.risingEdge() || minus.risingEdge()){
            currentMode = TIMER;
            Serial.println("Ici");
        }
    }
    //Serial.println(currentMode);
}

void changeData(){
    if(currentMode==TIMER /*SETTING_TIMER*/){
        if(plus.risingEdge() || plus.longPress()){
            timer.addMinutes(1);
        } else if(minus.risingEdge() || minus.longPress()){
            timer.addMinutes(-1);
        }
    }else if(currentMode==SETTING_CLOCK){
        if(plus.risingEdge() || plus.longPress()){
            clock.addMinutes(1);
        } else if(minus.risingEdge() || minus.longPress()){
            clock.addMinutes(-1);
        }
    } else if(currentMode==TIMER_ENDED){
        timer.reset();
        if(plus.risingEdge() || plus.longPress()){
            timer.addMinutes(1);
        } else if(minus.risingEdge() || minus.longPress()){
            timer.addMinutes(-1);
        }
    }
    relay.write(currentMode!=TIMER_ENDED);
}
        

void display(){
    if(currentMode == CLOCK || currentMode == SETTING_CLOCK){

        displayTime(clock.displayValue(), blinkColon);

    } else if (currentMode == TIMER || currentMode == TIMER_ENDED /*|| currentMode == SETTING_TIMER*/) {

        displayTime(timer.displayValue(), blinkColon);
    }
    
    //clockDisplay.drawColon(blinkColon);
    
    //clockDisplay.writeDisplay();

}

void onTime(){
    blinkColon = !blinkColon;
    clock.tick();
    if(currentMode==TIMER){
        timer.tick();
    }
}


void setup() {
  
  	// pinMode(CLOCK, OUTPUT);
  	// pinMode(TIMER, OUTPUT);
  	// pinMode(SETTING_TIMER, OUTPUT);
  	// pinMode(SETTING_CLOCK, OUTPUT);
  
    // Setup Serial port to print debug output.
    Serial.begin(115200);
    Serial.println("Clock starting!");

    uint8_t numDigits = 4;
    uint8_t digitPins[] = {13, 12, 11, 10};
    uint8_t segmentPins[] = {7, 5, 9, A1, A0, 6, 8, A2};

    bool resistorsOnSegments = true;

    int8_t hardwareConfig = COMMON_CATHODE;

    sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments, false, true);

    sevseg.setBrightness(255);


    start = millis();
    display();
  
  	digitalWrite(currentMode, HIGH);
  
}

void loop() {
    updateButtons();
    // Loop function runs over and over again to implement the clock logic.
    //currentMode = updateMode(currentMode);
 	  relay.write(!timer.isEnded());
    changeData();
    updateMode();
    display();
    //sevseg.refreshDisplay();
    //Serial.println(currentMode);
    // Pause for a second for time to elapse.  This value is in milliseconds
    // so 1000 milliseconds = 1 second.
    if(millis()-start>1000){
        start = millis();
        onTime();
      	//Serial.println(currentMode);
      	//Serial.println(timer.seconds);
        //Serial.println(relay.state);
    }
    sevseg.refreshDisplay();
    
}