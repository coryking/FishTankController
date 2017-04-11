//
// Created by Cory King on 4/10/17.
//

#ifndef FISHTANKCONTROLLER_OTA_H
#define FISHTANKCONTROLLER_OTA_H

#include <Arduino.h>
#include <ArduinoOTA.h>

void setupOTA(U8G2 *display) {
    ArduinoOTAClass::THandlerFunction onStartFn = [=]() {
        // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
        display->clearBuffer();
        display->println("Start updating...");
        display->sendBuffer();
    };

    ArduinoOTAClass::THandlerFunction onEndFn = [=]() {
        display->clearBuffer();
        display->println("Update Complete.");
        display->sendBuffer();
        delay(1000);
        ESP.restart();

    };

    ArduinoOTAClass::THandlerFunction_Error  onErrorFn = [=](ota_error_t error) {
        display->clearBuffer();
        display->printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) display->println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) display->println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) display->println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) display->println("Receive Failed");
        else if (error == OTA_END_ERROR) display->println("End Failed");
        display->sendBuffer();
        delay(10000);
        ESP.restart();
    };

    ArduinoOTA.onStart(onStartFn);
    ArduinoOTA.onEnd(onEndFn);
    ArduinoOTA.onError(onErrorFn);
    ArduinoOTA.begin();

}
#endif //FISHTANKCONTROLLER_OTA_H
