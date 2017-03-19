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

#include <ESPAsyncWebServer.h>

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

#define DISPLAY_FPS 15
#define SYNC_INTERVAL 30

GlobalState *GlobalState::s_instance = 0;

TaskManager taskManager;

DallasTemperature* sensors;
U8G2* displayModule;
RTC_DS3231* rtc;
Temp* aquariumTemp;

Display* display;

ShiftRegister reg(MsToTaskTime(10));

DoseKeeper keeper1(1, 30.0, "Macro");
DoseKeeper keeper2(1, 30.0, "Trace");

Pump pump1(0.78, "P1");
Pump pump2(0.73, "P2");

Relay relay1("R1");
Relay relay2("R2");

Button button1("B1");
Button button2("B2");

ControllerEventList eventList;

Settings settings;

DefineCalendarType(Calendar, 20)
Calendar myCalendar;

void OnDoPump(uint32_t deltaTime);
FunctionTask taskPumpStuff(OnDoPump, MsToTaskTime(30000));

void onDoRelay(uint32_t deltaTime);
FunctionTask taskRelayStuff(onDoRelay, MsToTaskTime(10000));

void onDoSchedule(uint32_t deltaTime);
FunctionTask taskSchedule(onDoSchedule, MsToTaskTime(1000));

void setDevicesFromSettings();

void setupWebServer();

time_t syncRtcTime() {
    if(rtc != NULL) {
        return rtc->now().unixtime();
    } else {
        return 0;
    }
}

void setup() {
    sprintf(hostString, "ESP_%06X", ESP.getChipId());
    Serial.begin(9600);

    setupWiFi();

    Serial.print("Waiting for Wifi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        Serial.print(WiFi.status());
    }
    Serial.println("");
    Serial.println("WiFi connected");

    SPIFFS.begin();

    setupTwoWire();
    Serial.println("two wire set up...");

    GlobalState::Builder builder;

    Serial.println("Hello world....");
    displayModule = setupDisplay();
    Serial.println("display set up...");
    delay(2000);

    sensors = setupTemp(displayModule);
    aquariumTemp = new Temp(sensors,0, MsToTaskTime(5000));
    builder.setAquariumTemp(aquariumTemp);
    taskManager.StartTask(aquariumTemp);

    rtc = setupRtc(displayModule);
    builder.setRTC(rtc);
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

    Button::ButtonCallbackFn onB1PressedHandler = [](Button *button) {
        pump1.startDispenser();
    };
    Button::ButtonCallbackFn  onB1ReleasedHandler = [](Button *button) {
        pump1.stopDispenser();
    };
    button1.setOnReleased(onB1ReleasedHandler).setOnPressed(onB1PressedHandler);
    reg.addDevice(BUTTON1_PIN, true, &button1)
       .addDevice(BUTTON2_PIN, true, &button2);
    builder.setButtons(&button1, &button2);

    taskManager.StartTask(&reg);
    Serial.println("Shift Register is set up...");
    builder.initialize();

    display = new Display(displayModule, MsToTaskTime(1000/DISPLAY_FPS));
    taskManager.StartTask(display);

    taskManager.StartTask(&taskPumpStuff);
    taskManager.StartTask(&taskRelayStuff);

    taskManager.StartTask(&taskSchedule);


    settings = loadSettings(settings);
    setDevicesFromSettings();

    setupMDNS();
    setupWebServer();

    Serial.println("Bottom of setup!!");
}

void onPostConfig(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {

    DynamicJsonBuffer jsonInBuffer;
    JsonObject& input = jsonInBuffer.parseObject(reinterpret_cast<char*>(data));

    if(input.success()) {
        settings = Settings::fromJson(input);
    }
}

void setupWebServer() {

    Serial.println("Setting up Web Server...");
    ArRequestHandlerFunction getConfig = [](AsyncWebServerRequest *request){
        AsyncResponseStream *response = request->beginResponseStream("text/json");

        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.createObject();
        JsonObject& jset = json.createNestedObject("settings");
        settings.toJson(jset);

        JsonObject& jcur = json.createNestedObject("current");
        JsonObject& kj1 = jcur.createNestedObject("keeper1");
        JsonObject& kj2 = jcur.createNestedObject("keeper2");
        keeper1.toJson(kj1);
        keeper2.toJson(kj2);

        JsonObject& pj1 = jcur.createNestedObject("pump1");
        JsonObject& pj2 = jcur.createNestedObject("pump2");
        pump1.toJson(pj1);
        pump2.toJson(pj2);

        json.prettyPrintTo(*response);

        request->send(response);
    };
    server.on("/config", HTTP_GET, getConfig);

    ArBodyHandlerFunction  onPostBodyHandler = &onPostConfig;
    AsyncCallbackWebHandler pHandler = server.on("/config", HTTP_POST, getConfig, NULL, onPostBodyHandler);

    ArRequestHandlerFunction  getCalendar = [](AsyncWebServerRequest *request){
        AsyncResponseStream *response = request->beginResponseStream("text/json");

        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.createObject();

        json["currentTime"] = Utils::printDT(Chronos::DateTime::now());

        Chronos::Event::Occurrence occurrenceList[14];
        uint8_t future = myCalendar.listNext(14, occurrenceList, Chronos::DateTime::now());
        JsonArray& jar = json.createNestedArray("events");
        for (uint8_t i=0; i<future; i++) {
            JsonObject& evnt = jar.createNestedObject();
            Utils::eventToJson(occurrenceList[i], evnt);
        }

        Chronos::Event::Occurrence ongoingList[14];
        uint8_t numOngoing = myCalendar.listOngoing(14, ongoingList, Chronos::DateTime::now());
        JsonArray& oj = json.createNestedArray("ongoing");
        for (uint8_t i=0; i<numOngoing; i++) {
            JsonObject& evnt = oj.createNestedObject();
            Utils::eventToJson(ongoingList[i], evnt);
        }
        json.prettyPrintTo(*response);

        request->send(response);
    };
    server.on("/calendar", HTTP_GET, getCalendar);

    ArRequestHandlerFunction setTheTime = [](AsyncWebServerRequest *request) {
        DateTime dt = DateTime(
                request->getParam("year", true)->value().toInt(),
                request->getParam("month", true)->value().toInt(),
                request->getParam("day", true)->value().toInt(),
                request->getParam("hour", true)->value().toInt(),
                request->getParam("minute", true)->value().toInt(),
                request->getParam("second", true)->value().toInt()
        );
        rtc->adjust(dt);
        setTime(dt.unixtime());
        AsyncResponseStream *response = request->beginResponseStream("text/json");
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.createObject();

        json["chronos"] = Utils::printDT(Chronos::DateTime::now());
        json["rtc"] = Utils::printDT(dt);
        json["now"] = now();
        json.printTo(*response);

        request->send(response);
    };
    server.on("/time", HTTP_POST, setTheTime);

    server.begin();
    Serial.println("Web Server Running...");
}


void loop() {
    taskManager.Loop();
}

void OnDoPump(uint32_t deltaTime) {
    pump1.dispenseAmount(keeper1.getDoseForInterval(0));
}
void onDoRelay(uint32_t deltaTime) {
    relay2.setDeviceState(!relay2.getDeviceState());
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
                           Chronos::Span::Hours(10)
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

