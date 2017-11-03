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

#include "state.h"


#define MQTT_GET_TEMP "fishtank/sensor/temp"

#define MQTT_PUMP1_GO "fishtank/pump/1/trigger"
#define MQTT_PUMP2_GO "fishtank/pump/2/trigger"
#define MQTT_PUMP_STATUS "fishtank/pump/%d/status"

#define MQTT_LIGHT_1_STATUS "fishtank/light/1/status"
#define MQTT_LIGHT_2_STATUS "fishtank/light/2/status"
#define MQTT_LIGHT_1_BRIGHTNESS "fishtank/light/1/brightness"
#define MQTT_LIGHT_2_BRIGHTNESS "fishtank/light/2/brightness"

#define MQTT_SERVER "mqtt.gotokingfamily.com"

class MqttPubSub : public Task {

public:
    typedef std::function<void(int)> PumpTriggerCallback;
    typedef std::function<bool(int)> PumpStatusCallback;
    typedef std::function<float(void)> GetAquariumTempCallback;
    typedef std::function<void(int, int)> LightBrightnessCallback;
    typedef std::function<LightStatus(int)> LightStatusCallback;

    MqttPubSub(WiFiClient &wifiClient) : Task(MsToTaskTime(5000)) {
        sprintf(hostString, "ESP_%06X", ESP.getChipId());

        this->client = PubSubClient(wifiClient);
        client.setServer(MQTT_SERVER, 1883);
        client.setCallback([this](char *topic, byte *payload, unsigned int length) {
            this->mqttCallback(topic, payload, length);
        });
    }

    void setPumpStatusCallback(PumpStatusCallback pumpStatusCallback);

    void setPumpTriggerCallback(PumpTriggerCallback pumpTriggerCallback);

    void setAquariumTempCallback(GetAquariumTempCallback aquariumTempCallback);

    void setLightBrightnessCallback(LightBrightnessCallback lightBrightnessCallback);

    void setLightStatusCallback(LightStatusCallback lightStatusCallback);

    virtual void OnUpdate(uint32_t deltaTime);

    void loop();

private:
    PubSubClient client;
    ulong lastReconnectAttempt = 0;

    void setSubscriptions();

    bool reconnect();

    char hostString[16] = {0};
    PumpStatusCallback pumpStatusCallback;
    PumpTriggerCallback pumpTriggerCallback;
    GetAquariumTempCallback aquariumTempCallback;

    LightBrightnessCallback lightBrightnessCallback;
    LightStatusCallback lightStatusCallback;

    void mqttCallback(char *topic, byte *payload, unsigned int length);

    void publishPumpStatus();

    void publishTemp();

    void publishLightStatus(LightStatus ls);

};


#endif //FISHTANKCONTROLLER_MQTT_H
