//
// Created by Cory King on 2/18/17.
//

#include <Arduino.h>

#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Time.h>
#include <Task.h>

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

DoseKeeper keeper1(20, 30.0, "Macro");
DoseKeeper keeper2(20, 30.0, "Trace");

Pump pump1(0.78, "P1");
Pump pump2(0.73, "P2");

Relay relay1("R1");
Relay relay2("R2");

Button button1("B1");
Button button2("B2");

void OnDoPump(uint32_t deltaTime);
FunctionTask taskPumpStuff(OnDoPump, MsToTaskTime(30000));

void onDoRelay(uint32_t deltaTime);
FunctionTask taskRelayStuff(onDoRelay, MsToTaskTime(10000));

time_t syncRtcTime() {
    if(rtc != NULL) {
        return rtc->now().unixtime();
    } else {
        return 0;
    }
}

void setup() {
    Serial.begin(9600);
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
    delay(2000);

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
    delay(2000);

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
    Serial.println("Bottom of setup!!");
}

void loop() {
    taskManager.Loop();
}

void OnDoPump(uint32_t deltaTime) {
    pump1.dispenseAmount(keeper1.getDoseForInterval(1));
}
void onDoRelay(uint32_t deltaTime) {
    relay1.setDeviceState(!relay1.getDeviceState());
    relay2.setDeviceState(!relay2.getDeviceState());
}