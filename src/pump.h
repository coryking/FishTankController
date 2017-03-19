//
// Created by Cory King on 2/18/17.
//

#ifndef FISHTANKCONTROLLER_PUMP_H
#define FISHTANKCONTROLLER_PUMP_H

#include <Arduino.h>
#include <Task.h>
#include "shift.h"
#include "Utils.h"

enum MotorState {
    IDLE,
    ACTIVE,
    FORCE_ACTIVE
};

class Pump : public Task, public ShiftDevice {
private:
    uint32_t dispensedAmountNl;
    uint32_t requestedAmountNl;
    uint32_t NlPerMs;
    MotorState currentState = MotorState::IDLE;

public:
    Pump(float mlPerS, String pumpName);

    void startDispenser();
    void stopDispenser();
    bool isDispensing();

    void setMlPerS(float_t mlPerS);

    void toJson(JsonObject& obj);

    void dispenseAmount(float_t theRequestedAmountMl) {
        Serial.print(this->requestedAmountNl);
        Serial.print(" dispensing: ");
        Serial.print(theRequestedAmountMl * 1000 * 1000);
        this->requestedAmountNl = this->requestedAmountNl + (1000 * 1000 * theRequestedAmountMl);
        Serial.print(" remaining: ");
        Serial.println(this->requestedAmountNl);
    }

    /**
     * Get the amount dispensed since the pump started
     * @return the mount of nanoliters dispensed since the pump started
     */
    uint32_t getAmountDispensedNl() {
        return this->dispensedAmountNl;
    }

    uint32_t getRemainingAmountNl() {
        return this->requestedAmountNl;
    }


    MotorState getMotorState() {
        return currentState;
    }

    virtual bool getDeviceState() {
        return (getMotorState() != MotorState::IDLE);
    }
    virtual void OnUpdate(uint32_t deltaTime);
};

#endif //FISHTANKCONTROLLER_PUMP_H
