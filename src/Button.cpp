//
// Created by Cory King on 3/5/17.
//

#include "Button.h"

Button::Button(String deviceName)  : ShiftDevice(deviceName) {
    _dbTime=DEBOUNCE_MS;
    _invert=false;
    _state=false;
    _changed=false;
    _lastState=_state;
    _time = millis();
    _lastChange=_time;

}

bool Button::getDeviceState() {
    return _state;
}

void Button::setDeviceState(bool newState) {
    ShiftDevice::setDeviceState(newState);

    uint32_t ms = millis();
    bool pinVal = newState;

    if (_invert != 0) pinVal = !pinVal;
    if (ms - _lastChange < _dbTime) {
        _time = ms;
        _changed = 0;
    }
    else {
        _lastState = _state;
        _state = pinVal;
        _time = ms;
        if (_state != _lastState)   {
            _lastChange = ms;
            _changed = 1;
            if(_state && _onPressed != NULL) {
                _onPressed(this);
            }
            if(!_state && _onReleased != NULL) {
                _onReleased(this);
            }
        }
        else {
            _changed = 0;
        }
    }
}

bool Button::isPressed() {
    return _state;
}

bool Button::isReleased() {
    return !_state;
}

bool Button::wasPressed() {
    return _state && _changed;
}

bool Button::wasReleased() {
    return !_state && _changed;
}

bool Button::pressedFor(uint32_t ms)
{
    return (_state == 1 && _time - _lastChange >= ms) ? 1 : 0;
}

bool Button::releasedFor(uint32_t ms)
{
    return (_state == 0 && _time - _lastChange >= ms) ? 1 : 0;
}

/*----------------------------------------------------------------------*
 * lastChange() returns the time the button last changed state,         *
 * in milliseconds.                                                     *
 *----------------------------------------------------------------------*/
uint32_t Button::lastChange(void) {
    return _lastChange;
}