//
// Created by Cory King on 2/18/17.
//

#ifndef FISHTANKCONTROLLER_SHIFT_H
#define FISHTANKCONTROLLER_SHIFT_H

#include <Arduino.h>
#include <Shifty.h>
#include <Task.h>
#include "pins.h"

Shifty* setupShiftReg();

enum ShiftPinMode {
    READ,
    WRITE
};

class ShiftDevice {
protected:
    ShiftDevice(){}
public:


    virtual bool getDeviceState();
    virtual void setDeviceState(bool newState) {}
};

typedef struct {
    ShiftPinMode mode;
    ShiftDevice* device;
} ShiftDeviceMap;

class ShiftRegister : public Task {
private:
    Shifty* reg;
    ShiftDeviceMap map[SHIFT_PINS];
public:
    ShiftRegister(uint32_t timeInterval) : Task(timeInterval) {
        this->reg = setupShiftReg();
    }
    void addDevice(uint8_t pin, bool isInputDevice, ShiftDevice* device);

protected:
    virtual void OnUpdate(uint32_t deltaTime);
};

#endif //FISHTANKCONTROLLER_SHIFT_H
