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
#define MQTT_SERVER "192.168.3.3"


class MqttPubSub : public Task {
private:
    PubSubClient client;
    ulong lastReconnectAttempt = 0;
    void setSubscriptions();
    bool reconnect();
    char hostString[16] = {0};


    void mqttCallback(char* topic, byte* payload, unsigned int length);
public:
    MqttPubSub(WiFiClient& wifiClient) : Task(MsToTaskTime(5000)) {
        sprintf(hostString, "ESP_%06X", ESP.getChipId());

        this->client = PubSubClient(wifiClient);
        client.setServer(MQTT_SERVER, 1883);
        client.setCallback([this](char* topic, byte* payload, unsigned int length) {
            this->mqttCallback(topic,payload,length);
        });
    }

    virtual void OnUpdate(uint32_t deltaTime);
    void loop();

};


#endif //FISHTANKCONTROLLER_MQTT_H
