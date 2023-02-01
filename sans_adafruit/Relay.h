#ifndef RELAY_H
#define RELAY_H

class Relay {
    public:
    int pin;
    bool state;
  
    Relay(int relayPin, bool startState = false);
  
    void write(bool value);
};

#endif