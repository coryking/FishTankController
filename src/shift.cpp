//
// Created by Cory King on 2/18/17.
//

#include "shift.h"

#define SHIFT_DEBUG false

Shifty* setupShiftReg() {
    Shifty* reg = new Shifty();
    reg->setBitCount(SHIFT_PINS);
    reg->setPins(SHIFT_DATA_PIN, SHIFT_CLOCK_PIN, SHIFT_LATCH_PIN, SHIFT_READ_PIN);
    return reg;
}

void printStatus(uint8_t pin, ShiftDevice *device) {
    if(SHIFT_DEBUG) {
        Serial.print("Writing to ");
        Serial.print(pin);
        Serial.print(" ");
        Serial.println(device->getDeviceState());
    }
}

ShiftRegister& ShiftRegister::addDevice(uint8_t pin, bool isInputDevice, ShiftDevice *device) {
    ShiftPinMode mode = isInputDevice ? ShiftPinMode::READ : ShiftPinMode::WRITE;
    map[pin] = {mode, device};
    if(mode == ShiftPinMode::WRITE) {
        printStatus(pin, device);
        reg->writeBit(pin, device->getDeviceState());
    } else {
        reg->setBitMode(pin, INPUT);
        device->setDeviceState(reg->readBit(pin));
    }
    return *this;
}

void ShiftRegister::OnUpdate(uint32_t deltaTime)  {
    Task::OnUpdate(deltaTime);
    writeToPins();
    readFromPins();
}

void ShiftRegister::readFromPins() const {
    //reg->batchReadBegin();
    for (int i = 0; i < SHIFT_PINS ; ++i) {
        if(map[i].device != NULL) {
            if (map[i].mode == READ) {
                bool state = reg->readBit(i);
                Serial.print(i);
                Serial.print(" reading... ");
                Serial.println(state);
                map[i].device->setDeviceState(state);
                printStatus(i, map[i].device);
            }
        }
    }
    //reg->batchReadEnd();
}

void ShiftRegister::writeToPins() const {
    reg->batchWriteBegin();
    for (int i = 0; i < SHIFT_PINS ; ++i) {
        if(map[i].device != NULL) {
            if (map[i].mode == WRITE) {
                printStatus(i, map[i].device);
                reg->writeBit(i, map[i].device->getDeviceState());
            }
        }
    }
    reg->batchWriteEnd();
}