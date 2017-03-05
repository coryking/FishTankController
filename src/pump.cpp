//
// Created by Cory King on 2/18/17.
//

#include <Arduino.h>
#include <Task.h>
#include "shift.h"
#include "pump.h"

#define PUMP_DEBUG false

void Pump::OnUpdate(uint32_t deltaTime){
    Task::OnUpdate(deltaTime);
    if(getMotorState() == MotorState::ACTIVE) {
        uint32_t amountPumped = NlPerMs * deltaTime;
        this->requestedAmountNl = max(0, (int32_t)(this->requestedAmountNl - amountPumped));
        if(PUMP_DEBUG) {
            Serial.print(this->requestedAmountNl);
            Serial.print(" ");
            Serial.print(deltaTime);
            Serial.print(" ");
            Serial.println(amountPumped);
        }
        if(this->requestedAmountNl <= 0) {
            Serial.print(this->getDeviceName());
            Serial.println(": Setting to idle...");
            currentState = MotorState::IDLE;
        }
    }
    if(getMotorState() == MotorState::IDLE) {
        if(this->requestedAmountNl > 0) {
            Serial.print(this->getDeviceName());
            Serial.print(": Setting %s to active (");
            Serial.print(this->requestedAmountNl);
            Serial.println(" nL left to pump)");
            currentState = MotorState::ACTIVE;
        }
    }
}

Pump::Pump(float_t mlPerS, String deviceName) : Task(MsToTaskTime(20)), ShiftDevice(deviceName) {
    this->NlPerMs = mlPerS * 1000.0; // mL / S * 1S / 1000mS * 1000uL/1mL * 1000nL/1uL = nL/mS
    this->requestedAmountNl=0;
}