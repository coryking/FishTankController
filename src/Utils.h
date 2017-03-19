//
// Created by Cory King on 3/6/17.
//

#ifndef FISHTANKCONTROLLER_UTILS_H
#define FISHTANKCONTROLLER_UTILS_H

#include <stdint.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Chronos.h>
#include <RTClib.h>

class Utils {
public:
    float static NLtoML(uint32_t nl);
    String static printDT(Chronos::DateTime dt);
    String static printDT(DateTime dt);
    static void eventToJson(Chronos::Event::Occurrence event, JsonObject& obj);
};


#endif //FISHTANKCONTROLLER_UTILS_H
