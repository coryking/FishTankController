//
// Created by Cory King on 2/18/17.
//

#ifndef FISHTANKCONTROLLER_PUMP_H
#define FISHTANKCONTROLLER_PUMP_H

#include <Arduino.h>
#include <Task.h>
#include "shift.h"

enum MotorState {
    IDLE,
    ACTIVE
};

class Pump : public Task, public ShiftDevice {
private:
    uint32_t requestedAmountNl;
    uint32_t NlPerMs;
    MotorState currentState = MotorState::IDLE;
public:
    Pump(float mlPerS, String pumpName);

    void dispenseAmount(float_t theRequestedAmountMl) {
        Serial.print(this->requestedAmountNl);
        Serial.print(" dispensing: ");
        Serial.print(theRequestedAmountMl * 1000 * 1000);
        this->requestedAmountNl = this->requestedAmountNl + (1000 * 1000 * theRequestedAmountMl);
        Serial.print(" remaining: ");
        Serial.println(this->requestedAmountNl);
    }

    float_t getRemainingAmountMl() {
        return this->getRemainingAmountNl() / (1000.0 * 1000.0);
    }

    uint32_t getRemainingAmountNl() {
        return this->requestedAmountNl;
    }


    MotorState getMotorState() {
        return currentState;
    }

    virtual bool getDeviceState() {
        return (getMotorState() == MotorState::ACTIVE) ? true : false;
    }
    virtual void OnUpdate(uint32_t deltaTime);
};

#endif //FISHTANKCONTROLLER_PUMP_H
