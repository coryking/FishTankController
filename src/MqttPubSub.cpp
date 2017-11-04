//
// Created by Cory King on 4/10/17.
//

#include "MqttPubSub.h"

void MqttPubSub::mqttCallback(char *topic, byte *payload, unsigned int length) {

    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
    }
    Serial.println();

    String theTopic = String(topic);

    int pumpNumber = 0;

    if (theTopic == MQTT_PUMP1_GO) {
        pumpNumber = 1;
    }
    if (theTopic == MQTT_PUMP2_GO) {
        pumpNumber = 2;
    }

    if(this->lightStatusCallback != NULL) {

        if(theTopic == MQTT_LIGHT_1_STATUS) {
            auto ls = this->lightStatusCallback(1);
            publishLightStatus(ls);
        }
        if(theTopic == MQTT_LIGHT_2_STATUS) {
            auto ls = this->lightStatusCallback(2);
            publishLightStatus(ls);
        }
    }

    if((theTopic == MQTT_LIGHT_1_BRIGHTNESS || theTopic == MQTT_LIGHT_2_BRIGHTNESS) && this->lightBrightnessCallback != NULL) {
        char message_buff[100] = {0};
        int i = 0;
        // create character buffer with ending null terminator (string)
        for(i=0; i<length; i++) {
            message_buff[i] = payload[i];
        }
        message_buff[i] = '\0';

        this->lightBrightnessCallback(theTopic == MQTT_LIGHT_1_BRIGHTNESS ? 1 : 2, atoi(message_buff));
    }

    if (pumpNumber > 0) {
        if ((char) payload[0] == 't' && this->pumpTriggerCallback != NULL) {
            this->pumpTriggerCallback(pumpNumber);
        }
        this->publishPumpStatus();
    }
}
void MqttPubSub::publishLightStatus(LightStatus ls) {
    auto br = String(ls.brightness);
    if(ls.lightNumber ==1) {
        this->publish(MQTT_LIGHT_1_STATUS, br.c_str());
    } else {
        this->publish(MQTT_LIGHT_2_STATUS, br.c_str());
    }
}
void MqttPubSub::publishTemp() {
    Serial.println("Sending temp over MQTT");
    if (this->aquariumTempCallback != NULL) {
        String tempStr = String(this->aquariumTempCallback());
        this->publish(MQTT_GET_TEMP, tempStr.c_str());
    }
}

void MqttPubSub::publishPumpStatus() {
    if (this->pumpStatusCallback != NULL) {
        for(int pumpNumber=1; pumpNumber<=2; pumpNumber++) {
            char statusTopic[20] = {0};
            sprintf(statusTopic, MQTT_PUMP_STATUS, pumpNumber);
            bool theStatus = this->pumpStatusCallback(pumpNumber);
            if (theStatus) {
                this->publish(statusTopic, "true");
            } else {
                this->publish(statusTopic, "false");
            }
        }
    }
}

void MqttPubSub::setSubscriptions() {
    Serial.println("Setting subscriptions...");
    // nothing to subscribe to...
    this->subscribe(MQTT_PUMP1_GO);
    this->subscribe(MQTT_PUMP2_GO);
    this->subscribe(MQTT_LIGHT_1_BRIGHTNESS);
    this->subscribe(MQTT_LIGHT_2_BRIGHTNESS);
    //this->client.subscribe(MQTT_LIGHT_1_STATUS);
    //this->client.subscribe(MQTT_LIGHT_1_STATUS);
}

void MqttPubSub::OnDoUpdate(uint32_t deltaTime) {
    this->publishTemp();
    this->publishPumpStatus();
}

void MqttPubSub::setPumpStatusCallback(MqttPubSub::PumpStatusCallback pumpStatusCallback) {
    this->pumpStatusCallback = pumpStatusCallback;
}


void MqttPubSub::setPumpTriggerCallback(MqttPubSub::PumpTriggerCallback pumpTriggerCallback) {
    this->pumpTriggerCallback = pumpTriggerCallback;
}

void MqttPubSub::setAquariumTempCallback(MqttPubSub::GetAquariumTempCallback aquariumTempCallback) {
    this->aquariumTempCallback = aquariumTempCallback;
}


void MqttPubSub::setLightBrightnessCallback(MqttPubSub::LightBrightnessCallback lightBrightnessCallback) {
    MqttPubSub::lightBrightnessCallback = lightBrightnessCallback;
}

void MqttPubSub::setLightStatusCallback(MqttPubSub::LightStatusCallback lightStatusCallback) {
    MqttPubSub::lightStatusCallback = lightStatusCallback;
}

