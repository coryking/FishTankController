//
// Created by Cory King on 4/10/17.
//

#include "MqttPubSub.h"

void MqttPubSub::mqttCallback(char *topic, byte *payload, unsigned int length) {

}

void MqttPubSub::setSubscriptions() {
    // nothing to subscribe to...
}

void MqttPubSub::OnUpdate(uint32_t deltaTime) {
    Task::OnUpdate(deltaTime);
    Serial.println("Sending temp over MQTT");
    String blah = String(GlobalState::instance()->getAquariumTempC());

    this->client.publish(MQTT_GET_TEMP, blah.c_str());
}

void MqttPubSub::loop() {

    if (!this->client.connected()) {
        long now = millis();
        if (now - lastReconnectAttempt > 5000) {
            lastReconnectAttempt = now;
            // Attempt to reconnect
            if (this->reconnect()) {
                lastReconnectAttempt = 0;
            }
        }
    } else {
        // Client connected
        this->client.loop();
    }

}

bool MqttPubSub::reconnect() {
    if (client.connect(MQTT_SERVER)) {
        // ... and resubscribe
        this->setSubscriptions();
    }
    return client.connected();
}
