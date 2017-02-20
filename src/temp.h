//
// Created by Cory King on 2/18/17.
//

#ifndef FISHTANKCONTROLLER_TEMP_H
#define FISHTANKCONTROLLER_TEMP_H

#include <Arduino.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#include <Task.h>

#include "pins.h"

DallasTemperature* setupTemp(
        U8G2 *display);

class Temp : public Task
{
private:
    DallasTemperature* _sensors;
    DeviceAddress _deviceAddress;
public:
    Temp(DallasTemperature* sensors, uint8 deviceIndex, uint32_t timeInterval);

    virtual void OnUpdate(uint32_t deltaTime);

    float getTempF();
    float getTempC();
};


#endif //FISHTANKCONTROLLER_TEMP_H
