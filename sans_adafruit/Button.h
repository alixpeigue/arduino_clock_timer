#ifndef BUTTON_H
#define BUTTON_H

class Button {
    public :
  
    Button(uint8_t pin);
  
    bool read();
  
    void update();
  
    bool risingEdge();

    bool pressed();
  
  	bool fallingEdge();
  
  	bool longPress();
  
    private:
    uint8_t button;
    bool state;
    bool lastState;
  	uint64_t lastRisingEdge;
};

#endif