//
// Created by Cory King on 3/18/17.
//

#ifndef FISHTANKCONTROLLER_WIFI_H
#define FISHTANKCONTROLLER_WIFI_H

#include <functional>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define YOUR_WIFI_SSID "Cats Drinking Wine"
#define YOUR_WIFI_PASSWD "catsmeow"

WiFiUDP ntpUDP;
AsyncWebServer server(80);
char hostString[16] = {0};

NTPClient timeClient(ntpUDP, "pool.ntp.org", -7 * 60 * 60, 60000);

void setupWiFi() {
    static WiFiEventHandler e1, e2;

    WiFi.mode(WIFI_STA);

    //onSyncEvent_t onSyncHandler = onNTPSyncEvent;
    //NTP.onNTPSyncEvent(onSyncHandler);
    std::function<void(const WiFiEventStationModeGotIP&)> handler = [](WiFiEventStationModeGotIP ipInfo) {
        Serial.printf("Got IP: %s\r\n", ipInfo.ip.toString().c_str());
        //NTP.begin("pool.ntp.org", 1, true);
        timeClient.begin();
    };
    e1 = WiFi.onStationModeGotIP(handler);// As soon WiFi is connected, start NTP Client
    WiFi.begin(YOUR_WIFI_SSID, YOUR_WIFI_PASSWD);

    Serial.print("Waiting for Wifi");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    WiFi.printDiag(Serial);
    Serial.println("");
    Serial.println("WiFi connected");
}

void setupMDNS() {
    if (!MDNS.begin(hostString)) {
        Serial.println("Error setting up MDNS responder!");
    }
    Serial.println("mDNS responder started");

    char instanceName[30] = {0};
    sprintf(instanceName, "Fishtank Controller@%06X", ESP.getChipId());
    MDNS.setInstanceName(instanceName);
    MDNS.addService("http","tcp",80);
    MDNS.addServiceTxt("http", "tcp", "type", "strip_led");

}

#endif //FISHTANKCONTROLLER_WIFI_H
