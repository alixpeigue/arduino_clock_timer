
#include "Relay.h"
  
Relay::Relay(int relayPin, bool startState = false) : pin(relayPin), state(startState)
{
    pinMode(pin, OUTPUT);
    digitalWrite(pin, state);
}

void Relay::write(bool value)
{
    if(value != state){
        state = value;
        digitalWrite(pin, state); 
    }
}