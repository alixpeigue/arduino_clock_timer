#include "Button.h"

  
Button::Button(uint8_t pin) : button(pin) 
{
    pinMode(button, INPUT_PULLUP);
    state = read();
}

bool Button::read()
{
    return !digitalRead(button);
}

void Button::update()
{ 
    lastState = state;
    state = read();
    if(state)
        lastRisingEdge++;
    else
        lastRisingEdge = 0;
}

bool Button::risingEdge()
{
    return state && !lastState;
}

bool Button::pressed()
{
    return state;
}

bool Button::fallingEdge()
{
  	return !state && lastState;
}

bool Button::longPress()
{
    if(lastRisingEdge>5000){
        if(lastRisingEdge%30==0){
            return true;
        }
    }
    if(lastRisingEdge>3000){
        if(lastRisingEdge%200==0){
            return true;
        }
    }
    if(lastRisingEdge>1000){
        if(lastRisingEdge%500==0){
            return true;
        }
    }
    return false;
}