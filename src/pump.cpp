//
// Created by Cory King on 2/18/17.
//

#include <Arduino.h>
#include <Task.h>
#include "shift.h"
#include "pump.h"

void Pump::OnUpdate(uint32_t deltaTime){
    Task::OnUpdate(deltaTime);
    if(getMotorState() == MotorState::ACTIVE) {
        uint32_t amountPumped = UlPerMs * deltaTime;
        this->requestedAmountUl -= amountPumped;
        if(this->requestedAmountUl <= 0) {
            Serial.print(this->pumpName);
            Serial.println(": Setting to idle...");
            currentState = MotorState::IDLE;
        }
    }
    if(getMotorState() == MotorState::IDLE) {
        if(this->requestedAmountUl > 0) {
            Serial.print(this->pumpName);
            Serial.print(": Setting %s to active (");
            Serial.print(this->requestedAmountUl);
            Serial.println(" ML left to pump)");
            currentState = MotorState::ACTIVE;
        }
    }
}

Pump::Pump(uint32_t ulPerMs, String pumpName) : Task(MsToTaskTime(20)), ShiftDevice() {
    this->pumpName = pumpName;
    this->UlPerMs = ulPerMs;
    this->requestedAmountUl=0;
}