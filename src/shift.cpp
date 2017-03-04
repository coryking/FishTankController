//
// Created by Cory King on 2/18/17.
//

#include "shift.h"

Shifty* setupShiftReg() {
    Shifty* reg = new Shifty();
    reg->setBitCount(SHIFT_PINS);
    reg->setPins(SHIFT_DATA_PIN, SHIFT_CLOCK_PIN, SHIFT_LATCH_PIN); //, SHIFT_READ_PIN);
    return reg;
}

void printStatus(uint8_t pin, ShiftDevice *device) {
    Serial.print("Writing to ");
    Serial.print(pin);
    Serial.print(" ");
    Serial.println(device->getDeviceState());
}

void ShiftRegister::addDevice(uint8_t pin, bool isInputDevice, ShiftDevice *device) {
    ShiftPinMode mode = isInputDevice ? ShiftPinMode::READ : ShiftPinMode::WRITE;
    map[pin] = {mode, device};
    if(mode == ShiftPinMode::WRITE) {
        printStatus(pin, device);
        reg->writeBit(pin, device->getDeviceState());
    } else {
        device->setDeviceState(reg->readBit(pin));
    }
}

void ShiftRegister::OnUpdate(uint32_t deltaTime)  {
    Task::OnUpdate(deltaTime);
    this->reg->batchWriteBegin();
    for (int i = 0; i < SHIFT_PINS ; ++i) {
        if(this->map[i].device != NULL) {
            if (this->map[i].mode == ShiftPinMode::WRITE) {
                printStatus(i, this->map[i].device);
                reg->writeBit(i, this->map[i].device->getDeviceState());
            }
        }
    }
    this->reg->batchWriteEnd();
}