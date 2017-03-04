//
// Created by Cory King on 3/4/17.
//

#ifndef FISHTANKCONTROLLER_RELAY_H
#define FISHTANKCONTROLLER_RELAY_H

#include <Arduino.h>
#include "shift.h"

class Relay : public ShiftDevice {
private:
    bool relayState = false;
public:
    virtual bool getDeviceState();
    virtual void setDeviceState(bool newState);

    Relay(String deviceName);
};

#endif //FISHTANKCONTROLLER_RELAY_H
