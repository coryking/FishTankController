//
// Created by Cory King on 2/18/17.
//

#include "shift.h"

Shifty* setupShiftReg() {
    Shifty* reg = new Shifty();
    reg->setBitCount(SHIFT_PINS);
    reg->setPins(SHIFT_DATA_PIN, SHIFT_CLOCK_PIN, SHIFT_LATCH_PIN, SHIFT_READ_PIN);
    return reg;
}

void ShiftRegister::addDevice(uint8_t pin, bool isInputDevice, ShiftDevice *device) {
    ShiftPinMode mode = isInputDevice ? ShiftPinMode::READ : ShiftPinMode::WRITE;
    map[pin] = {mode, device};
    if(mode == ShiftPinMode::WRITE) {
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
            if (!this->map[i].mode == ShiftPinMode::WRITE) {
                reg->writeBit(i, this->map[i].device->getDeviceState());
            }
        }
    }
    this->reg->batchWriteEnd();
}