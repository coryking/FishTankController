//
// Created by Cory King on 3/4/17.
//

#include <Arduino.h>
#include "relay.h"

bool Relay::getDeviceState() {
    return this->relayState;
}

void Relay::setDeviceState(bool newState) {
    this->relayState=newState;
}

Relay::Relay(String deviceName) : ShiftDevice(deviceName) {

}
