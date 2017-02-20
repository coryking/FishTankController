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
    uint32_t requestedAmountUl;
    uint32_t UlPerMs;
    MotorState currentState = MotorState::IDLE;
    String pumpName;
public:
    Pump(uint32_t ulPerMs, String pumpName);

    void dispenseAmount(uint32_t requestedAmountUl) {
        this->requestedAmountUl += requestedAmountUl;
    }

    String getPumpName() {
        return this->pumpName;
    }

    uint32_t getRemainingAmountUl() {
        return this->requestedAmountUl;
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
