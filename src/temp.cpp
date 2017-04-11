//
// Created by Cory King on 2/18/17.
//

#include <U8g2lib.h>
#include "temp.h"

DallasTemperature* setupTemp(
        U8G2 *display) {

    DeviceAddress tempAddress;


    // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
    OneWire * oneWire = new OneWire(ONE_WIRE_BUS);

    // Pass our oneWire reference to Dallas Temperature.
    DallasTemperature* sensor = new DallasTemperature(oneWire);
    Serial.println("Finding probe...");
    sensor->begin();
    display->clearBuffer();
    display->setFont(u8g2_font_helvR08_tr);
    display->setCursor(0,15);
    display->print("Found ");
    display->print(sensor->getDeviceCount(), DEC);
    display->println(" devices.");
    display->sendBuffer();
    delay(2000);
    Serial.println("Found probe...");

    if(!sensor->getAddress(tempAddress, 0)) {
        display->clearBuffer();
        display->setFont(u8g2_font_helvR08_tr);

        display->setCursor(0,24);
        display->println("No Temp Probe");
        display->sendBuffer();
        delay(2000);
        return sensor;
    }
    Serial.println("Probe stuff done...");

    sensor->requestTemperaturesByAddress(tempAddress);
    display->clearBuffer();
    display->setFont(u8g2_font_helvR08_tr);
    display->setCursor(0,15);
    display->print("F ");
    display->println(sensor->getTempF(tempAddress), DEC);
    display->sendBuffer();

    Serial.println("Done done!");
    return sensor;
}


void Temp::OnUpdate(uint32_t deltaTime) {
    Task::OnUpdate(deltaTime);
    _sensors->requestTemperaturesByAddress(this->_deviceAddress);
}

float Temp::getTempF() {
    return _sensors->getTempF(this->_deviceAddress);
}

float Temp::getTempC() {
    return _sensors->getTempC(this->_deviceAddress);
}

Temp::Temp(DallasTemperature *sensors, uint8 deviceIndex, uint32_t timeInterval) : Task(timeInterval) {
    _sensors=sensors;
    _sensors->getAddress(_deviceAddress, deviceIndex);
}
