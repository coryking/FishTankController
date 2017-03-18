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
    uint32_t amountPumped = 0;
    if(getMotorState() == MotorState::ACTIVE) {
        amountPumped = NlPerMs * deltaTime;
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
    if(getMotorState() == MotorState::FORCE_ACTIVE) {
        amountPumped = NlPerMs * deltaTime;
    }
    if(getMotorState() == MotorState::IDLE) {
        if(this->requestedAmountNl > 0) {
            Serial.print(this->getDeviceName());
            Serial.print(": Setting %s to active (");
            Serial.print(this->requestedAmountNl);
            Serial.println(" nL left to pump)");
            currentState = MotorState::ACTIVE;
            dispensedAmountNl = 0;
        }
    }
    dispensedAmountNl += amountPumped;
}

Pump::Pump(float_t mlPerS, String deviceName) : Task(MsToTaskTime(20)), ShiftDevice(deviceName) {
    this->NlPerMs = mlPerS * 1000.0; // mL / S * 1S / 1000mS * 1000uL/1mL * 1000nL/1uL = nL/mS
    this->requestedAmountNl=0;
}

void Pump::setMlPerS(float_t mlPerS) {
    this->NlPerMs = mlPerS * 1000.0;
}

void Pump::startDispenser() {
    if(!isDispensing()) {
        dispensedAmountNl = 0;
    }
    currentState = MotorState::FORCE_ACTIVE;
}

void Pump::stopDispenser() {
    currentState = MotorState::IDLE;
    this->requestedAmountNl=0;
}

bool Pump::isDispensing() {
    return this->getDeviceState();
}
