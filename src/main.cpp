//
// Created by Cory King on 2/18/17.
//

#include <vector>

#include <Arduino.h>
#include <Hash.h>
#include <FS.h>
#include <SPI.h>
#include <OneWire.h>
#include <Chronos.h>
#include <DallasTemperature.h>
#include <Time.h>
#include <Task.h>

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <Syslog.h>

#include <ArduinoOTA.h>

#include "Wifi.h"

#include "Settings.h"
#include "ControllerEvent.h"
#include "DoseKeeper.h"
#include "rtc.h"
#include "temp.h"
#include "twowire.h"
#include "shift.h"
#include "pump.h"
#include "relay.h"
#include "button.h"
#include "state.h"
#include "display.h"
#include "OTA.h"
#include "MqttPubSub.h"
#include "Light.h"

#define DISPLAY_FPS 15
#define SYNC_INTERVAL 30

#define MQTT_SERVER "mqtt.gotokingfamily.com"
#define MQTT_PORT 1883

// Syslog server connection info
#define SYSLOG_SERVER "192.168.1.85"
#define SYSLOG_PORT 514

// This device info
#define APP_NAME "fishtank"

GlobalState *GlobalState::s_instance = 0;

TaskManager taskManager;

DallasTemperature* sensors;
U8G2* displayModule;
RTC_DS3231* rtc;
Temp* aquariumTemp;

Display* display;

ShiftRegister reg(MsToTaskTime(10));

DoseKeeper keeper1(1, 5.0, "Macro");
DoseKeeper keeper2(1, 5.0, "Trace");

Pump pump1(0.78, "P1");
Pump pump2(0.73, "P2");

Relay relay1("R1");
Relay relay2("R2");

Button button1("B1");
Button button2("B2");

Light light1(1, LIGHT_1_PIN);
Light light2(2, LIGHT_2_PIN);

ControllerEventList eventList;

Settings settings;

WiFiUDP udpClient;
Syslog syslog(udpClient, SYSLOG_PROTO_BSD);


DefineCalendarType(Calendar, 20)
Calendar myCalendar;
MqttPubSub mqttPubSub(MQTT_SERVER, MQTT_PORT, &syslog);

long lastWifiReconnectAttempt = 0;

void onPublishTemp(uint32_t deltaTime);
FunctionTask taskPublishTemp(onPublishTemp, MsToTaskTime(30 * 1000));
void onPublishPumpStatus(uint32_t deltaTime);
FunctionTask taskPublishPumpStatus(onPublishPumpStatus, MsToTaskTime(1000));

void onDoSchedule(uint32_t deltaTime);
FunctionTask taskSchedule(onDoSchedule, MsToTaskTime(1000));

void setDevicesFromSettings();

AmountDispensedFn makeAmounDispensedCb(Button* button);
Button::ButtonCallbackFn makePressedButtonCb(Pump* pump, DoseKeeper* doseKeeper);
Button::ButtonCallbackFn makeReleasedButtonCb(Pump* pump);

float aquariumTempCb();
void triggerPumpCb(int pumpNum);
bool getPumpStatusCb(int pumpNum);

time_t syncRtcTime() {
    static unsigned long last_sync = millis();
    Serial.println("Sync RTC Time");

    if (rtc != NULL) {
        if(millis() - last_sync >= 5 * 60 * 1000) {
            Serial.println("setting based on NTP");
            last_sync = millis();
            rtc->adjust(DateTime(timeClient.getEpochTime()));
            setTime(timeClient.getEpochTime());
        }

        return rtc->now().unixtime();
    } else {
        return 0;
    }
}

LightStatus getLightStatus(int light) {
    if(light == 1) {
        return {1, light1.GetBrightness()};
    } else {
        return {2, light2.GetBrightness()};
    }
}

void setup() {
    sprintf(hostString, "ESP_%06X", ESP.getChipId());
    Serial.begin(9600);
    setupWiFi();
    syslog.server(SYSLOG_SERVER, SYSLOG_PORT);
    syslog.deviceHostname(hostString);
    syslog.appName(APP_NAME);
    syslog.defaultPriority(LOG_DAEMON);
    syslog.logf(LOG_WARNING, "Hello from [%s]\n", hostString);

    SPIFFS.begin();

    setupTwoWire();
    Serial.println("two wire set up...");

    GlobalState::Builder builder;

    Serial.println("Hello world....");
    displayModule = setupDisplay();
    Serial.println("display set up...");
    delay(2000);

    sensors = setupTemp(displayModule);
    aquariumTemp = new Temp(sensors, 0, MsToTaskTime(5000));
    builder.setAquariumTemp(aquariumTemp);
    taskManager.StartTask(aquariumTemp);

    rtc = setupRtc(displayModule);
    builder.setRTC(rtc);
    rtc->adjust(DateTime(timeClient.getEpochTime()));
    setTime(timeClient.getEpochTime());
    setSyncProvider(syncRtcTime);
    setSyncInterval(SYNC_INTERVAL);
    if(timeStatus() != timeSet) {
        Serial.println("Unable to sync with the RTC");
    } else {
        Serial.println("RTC has set the system time");
    }
    Serial.println("RTC is set up...");

    reg.addDevice(PUMP1_PIN, false, &pump1)
       .addDevice(PUMP2_PIN, false, &pump2);
    taskManager.StartTask(&pump1);
    taskManager.StartTask(&pump2);
    builder.setPumps(&pump1, &pump2);
    Serial.println("Pumps are set up...");

    reg.addDevice(RELAY1_PIN, false, &relay1)
       .addDevice(RELAY2_PIN, false, &relay2);
    relay1.setDeviceState(true);
    builder.setRelays(&relay1, &relay2);
    Serial.println("Relays are set up...");

    button1.setOnReleased(makeReleasedButtonCb(&pump1)).setOnPressed(makePressedButtonCb(&pump1, &keeper1));
    button2.setOnReleased(makeReleasedButtonCb(&pump2)).setOnPressed(makePressedButtonCb(&pump2, &keeper2));
    pump1.setAmountDispensedFn(makeAmounDispensedCb(&button1));
    pump2.setAmountDispensedFn(makeAmounDispensedCb(&button2));

    reg.addDevice(BUTTON1_PIN, true, &button1)
       .addDevice(BUTTON2_PIN, true, &button2);
    builder.setButtons(&button1, &button2);

    builder.setDoseKeepers(&keeper1, &keeper2);

    taskManager.StartTask(&reg);
    Serial.println("Shift Register is set up...");
    builder.initialize();

    display = new Display(displayModule, MsToTaskTime(1000/DISPLAY_FPS));
    taskManager.StartTask(display);

    // for the MVP we are gonna leave out the scheduling aspect of this...
    settings = loadSettings(settings);
    setDevicesFromSettings();

    taskManager.StartTask(&taskPublishTemp);
    taskManager.StartTask(&taskPublishPumpStatus);

    setupMDNS();
    setupOTA(displayModule);

    mqttPubSub.setPumpTriggerCallback(triggerPumpCb);

    mqttPubSub.setLightBrightnessCallback([](int light, uint8 brightness) {
        if(light==1) {
            light1.SetBrightness(brightness);
        } else {
            light2.SetBrightness(brightness);
        }
    });
    mqttPubSub.setLightStatusCallback(getLightStatus);
    taskManager.StartTask(&mqttPubSub);

    Serial.println("Bottom of setup!!");
}

void triggerPumpCb(int pumpNum) {
    Pump* thePump = (pumpNum == 1) ? &pump1 : &pump2;
    DoseKeeper* theDk = (pumpNum == 1) ? &keeper1 : &keeper2;

    if(thePump->getMotorState() == MotorState::IDLE) {
        thePump->dispenseAmount(theDk->getDoseForInterval(0));
        mqttPubSub.publishPumpStatus(pumpNum, thePump->isDispensing());
    }
}

void onPublishTemp(uint32_t deltaTime) {
    auto temp = aquariumTemp->getTempC();
    mqttPubSub.publishTemp(temp);
}

void onPublishPumpStatus(uint32_t deltaTime) {
    mqttPubSub.publishPumpStatus(1, pump1.isDispensing());
    mqttPubSub.publishPumpStatus(2, pump2.isDispensing());
}

AmountDispensedFn makeAmounDispensedCb(Button* button) {
    AmountDispensedFn fn = [=](Pump* pump) {
        if(button->isPressed()) {
            pump->startDispenser();
        }
    };
    return fn;
}

Button::ButtonCallbackFn makeReleasedButtonCb(Pump* pump) {
    Button::ButtonCallbackFn fn = [=](Button *button) {
        if(pump->getMotorState() == FORCE_ACTIVE)
            pump->stopDispenser();
    };
    return fn;
};

Button::ButtonCallbackFn makePressedButtonCb(Pump* pump, DoseKeeper* doseKeeper) {
    Button::ButtonCallbackFn fn = [=](Button* button){
        if(pump->getMotorState() == MotorState::IDLE) {
            pump->dispenseAmount(doseKeeper->getDoseForInterval(0));
        }
    };
    return fn;
}


void loop() {
    taskManager.Loop();
    delay(0);
    timeClient.update();
    ArduinoOTA.handle();
    if(WiFi.status() != WL_CONNECTED) {
        long now = millis();
        if (now - lastWifiReconnectAttempt > 5000) {
            lastWifiReconnectAttempt = now;
            // Attempt to reconnect
            reconnectWiFi();

            if (WiFi.status() == WL_CONNECTED) {
                lastWifiReconnectAttempt = 0;
            }
        }
    }



}

/*
 * // Dont think we will need this
EventStates initEventStates(ControllerEvents m) {
    EventStates items;
    for(ControllerEvents::iterator it = m.begin(); it != m.end(); ++it) {
        items[it->first] = false;
    }
    return items;
}
*/

// Sync the events that should be active or inactive with the list of controller events
void syncEventStates(EventStates states) {
    for(ControllerEvents::iterator it = eventList.begin(); it != eventList.end(); ++it) {
        if(eventList[it->first] != NULL) {
            eventList[it->first]->setActiveState(states[it->first]);
        }
    }
}

void onDoSchedule(uint32_t deltaTime) {
    Chronos::Event::Occurrence occurrenceList[14];
    uint8_t numOngoing = myCalendar.listOngoing(14, occurrenceList, Chronos::DateTime::now());

    EventStates states;
    for (uint8_t i=0; i<numOngoing; i++) {
        states[occurrenceList[i].id] = true;
    }

    syncEventStates(states);
}


void setPumpFromSettings(Chronos::EventID eventID, PumpSetting *ps, DoseKeeper &dk, Pump &p) {
    dk.setDoseName(ps->getDoseName());
    dk.setDailyDoseMl(ps->getDoseAmount());
    p.setDeviceName(ps->getName());
    p.setMlPerS(ps->getDoseRate());

    myCalendar.add(
            Chronos::Event(eventID,
                           ps->getDoseTime(),
                           Chronos::Span::Minutes(1)
            )
    );
}

void setRelayFromSettings(Chronos::EventID eventID, RelaySetting *rs, Relay *r) {
    r->setDeviceName(rs->getName());
    myCalendar.add(
        Chronos::Event(eventID,
            rs->getOnTime(), rs->getDuration())
    );
}

void setDevicesFromSettings() {
    setPumpFromSettings(P1_EVENT, settings.getPump1(), keeper1, pump1);
    eventList.setEvent(P1_EVENT, [](){
        pump1.dispenseAmount(keeper1.getDoseForInterval(0));
    }, [](){

    });

    setPumpFromSettings(P2_EVENT, settings.getPump2(), keeper2, pump2);
    eventList.setEvent(P2_EVENT, [](){
        pump2.dispenseAmount(keeper2.getDoseForInterval(0));
    }, [](){

    });

    setRelayFromSettings(RELAY1_EVENT, settings.getRelay1(), &relay1);
    eventList.setEvent(RELAY1_EVENT, [](){
        relay1.setDeviceState(true);
    }, [](){
        relay1.setDeviceState(false);
    });
}

void onDoLighting(uint32_t deltaTime) {
    static uint16_t count=0;
    analogWrite(LIGHT_1_PIN, count % 1024);
    analogWrite(LIGHT_2_PIN, count % 1024);
    count++;
}
