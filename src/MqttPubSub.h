//
// Created by Cory King on 4/10/17.
//

#ifndef FISHTANKCONTROLLER_MQTT_H
#define FISHTANKCONTROLLER_MQTT_H

#include <Arduino.h>

#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Task.h>
#include <MqttController.h>
#include "state.h"



#define MQTT_GET_TEMP "fishtank/sensor/temp"

#define MQTT_PUMP1_GO "fishtank/pump/1/trigger"
#define MQTT_PUMP2_GO "fishtank/pump/2/trigger"
#define MQTT_PUMP_STATUS "fishtank/pump/%d/status"

#define MQTT_LIGHT_1_STATUS "fishtank/light/1/status"
#define MQTT_LIGHT_2_STATUS "fishtank/light/2/status"
#define MQTT_LIGHT_1_BRIGHTNESS "fishtank/light/1/brightness"
#define MQTT_LIGHT_2_BRIGHTNESS "fishtank/light/2/brightness"

class MqttPubSub : public MqttController {

public:
    typedef std::function<void(int)> PumpTriggerCallback;
    typedef std::function<void(int, int)> LightBrightnessCallback;
    typedef std::function<LightStatus(int)> LightStatusCallback;

    MqttPubSub(const char* mqttHost, const uint16_t mqttPort, Syslog* logger) : MqttController(mqttHost, mqttPort, logger) {

    }

    void setPumpTriggerCallback(PumpTriggerCallback pumpTriggerCallback);


    void setLightBrightnessCallback(LightBrightnessCallback lightBrightnessCallback);

    void setLightStatusCallback(LightStatusCallback lightStatusCallback);

    void publishTemp(float aquariumTemp);
    void publishPumpStatus(int pumpNumber, bool status);
protected:
    void setSubscriptions() override;

    void mqttCallback(char *topic, byte *payload, unsigned int length) override;

private:


    PumpTriggerCallback pumpTriggerCallback;

    LightBrightnessCallback lightBrightnessCallback;
    LightStatusCallback lightStatusCallback;




    void publishLightStatus(LightStatus ls);


};


#endif //FISHTANKCONTROLLER_MQTT_H
