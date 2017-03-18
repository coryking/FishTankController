//
// Created by Cory King on 3/18/17.
//

#ifndef FISHTANKCONTROLLER_WIFI_H
#define FISHTANKCONTROLLER_WIFI_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <NtpClientLib.h>


#define YOUR_WIFI_SSID "Cats Drinking Wine"
#define YOUR_WIFI_PASSWD "catsmeow"

AsyncWebServer server(80);
char hostString[16] = {0};

void onSTAGotIP(WiFiEventStationModeGotIP ipInfo) {
    Serial.printf("Got IP: %s\r\n", ipInfo.ip.toString().c_str());
    NTP.begin("pool.ntp.org", 1, true);
    NTP.setInterval(63, 6300);
}

void onSTADisconnected(WiFiEventStationModeDisconnected event_info) {
    Serial.printf("Disconnected from SSID: %s\n", event_info.ssid.c_str());
    Serial.printf("Reason: %d\n", event_info.reason);
}

void onNTPSyncEvent(NTPSyncEvent_t ntpEvent) {
    if (ntpEvent) {
        Serial.print("Time Sync error: ");
        if (ntpEvent == noResponse)
            Serial.println("NTP server not reachable");
        else if (ntpEvent == invalidAddress)
            Serial.println("Invalid NTP server address");
    }
    else {
        Serial.print("Got NTP time: ");
        Serial.println(NTP.getTimeDateString(NTP.getLastNTPSync()));
    }
}

void setupWiFi() {
    static WiFiEventHandler e1, e2;
    WiFi.mode(WIFI_STA);
    WiFi.begin(YOUR_WIFI_SSID, YOUR_WIFI_PASSWD);

    //onSyncEvent_t onSyncHandler = onNTPSyncEvent;
    //NTP.onNTPSyncEvent(onSyncHandler);
    WiFi.onEvent([](WiFiEvent_t e) {
        Serial.printf("Event wifi -----> %d\n", e);
    });
    //e1 = WiFi.onStationModeGotIP(onSTAGotIP);// As soon WiFi is connected, start NTP Client
    //e2 = WiFi.onStationModeDisconnected(onSTADisconnected);

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
