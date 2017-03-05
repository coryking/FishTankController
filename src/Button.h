//
// Created by Cory King on 3/5/17.
//

#ifndef FISHTANKCONTROLLER_BUTTON_H
#define FISHTANKCONTROLLER_BUTTON_H

#define DEBOUNCE_MS 20     //A debounce time of 20 milliseconds usually works well for tactile button switches.

#include <Arduino.h>
#include <functional>

#include "shift.h"

class Button : public ShiftDevice  {

public:
    typedef std::function<void(Button*)> ButtonCallbackFn;

    Button(String deviceName);

    virtual bool getDeviceState();
    virtual void setDeviceState(bool newState);

    bool isPressed();
    bool isReleased();
    bool wasPressed();
    bool wasReleased();
    /**
     * Returns true of a button was pressed longer than "ms"
     * @param ms
     * @return true if longer than "ms"
     */
    bool pressedFor(uint32_t ms);

    /**
     * Returns true if the button was released for longer than "ms"
     * @param ms
     * @return true if longer than "ms"
     */
    bool releasedFor(uint32_t ms);

    /**
     * Time of last change
     * @return Time of last change (ms)
     */
    uint32_t lastChange();

    Button& setOnPressed(ButtonCallbackFn fn) {_onPressed = fn; return *this;}
    Button& setOnReleased(ButtonCallbackFn fn) {_onReleased = fn; return *this;}


private:
    bool _invert;        //if 0, interpret high state as pressed, else interpret low state as pressed
    bool _state;         //current button state
    bool _lastState;     //previous button state
    bool _changed;       //state changed since last read
    uint32_t _time;         //time of current state (all times are in ms)
    uint32_t _lastChange;   //time of last state change
    uint32_t _dbTime;       //debounce time
    ButtonCallbackFn _onPressed;
    ButtonCallbackFn _onReleased;

};



#endif //FISHTANKCONTROLLER_BUTTON_H
