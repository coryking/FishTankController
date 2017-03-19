//
// Created by Cory King on 3/18/17.
//

#ifndef FISHTANKCONTROLLER_SETTINGS_H
#define FISHTANKCONTROLLER_SETTINGS_H

#include <Arduino.h>
#include <FS.h>
#include <Chronos.h>
#include <ArduinoJson.h>

#include <memory>

#define JSON_FILE "/config.json"

#define JSON_PUMP1 "pump1"
#define JSON_PUMP2 "pump2"
#define JSON_RELAY1 "relay1"

#define JSON_HOUR "hour"
#define JSON_MINUTE "minute"
#define JSON_SECOND "second"
#define JSON_DURATION_HOURS "duration_hours"
#define JSON_NAME "name"
#define JSON_DOSENAME "doseName"
#define JSON_ON "on"
#define JSON_AMOUNT_ML "amountMl"
#define JSON_PUMP_RATE "pumpRateMlPerSec"

class PumpSetting {
private:
    String Name;
    String doseName;
    Chronos::Mark::Event *doseTime;
    float doseAmount;
    float doseRate;
public:
    PumpSetting() {}

    const String &getDoseName() const {
        return doseName;
    }

    void setDoseName(const String &DoseName) {
        this->doseName = DoseName;
    }

    const String &getName() const {
        return Name;
    }

    void setName(const String &Name) {
        PumpSetting::Name = Name;
    }

    Chronos::Mark::Event &getDoseTime() const {
        return *doseTime;
    }

    void setDoseTime(Chronos::Mark::Event *doseTime) {
        PumpSetting::doseTime = doseTime;
    }

    float getDoseAmount() const {
        return doseAmount;
    }

    void setDoseAmount(float doseAmount) {
        PumpSetting::doseAmount = doseAmount;
    }

    float getDoseRate() const {
        return doseRate;
    }

    void setDoseRate(float doseRate) {
        PumpSetting::doseRate = doseRate;
    }

    static PumpSetting *fromJson(const JsonObject &obj) {
        PumpSetting *pump = new PumpSetting();
        pump->Name = obj[JSON_NAME].as<String>();
        pump->doseName = obj[JSON_DOSENAME].as<String>();
        pump->setDoseTime(new Chronos::Mark::Daily(
                obj[JSON_ON][JSON_HOUR].as<Chronos::Hours>(),
                obj[JSON_ON][JSON_MINUTE].as<Chronos::Minutes >(),
                obj[JSON_ON][JSON_SECOND].as<Chronos::Seconds>()
        ));
        pump->setDoseAmount(obj[JSON_AMOUNT_ML]);
        pump->setDoseRate(obj[JSON_PUMP_RATE]);
        return pump;
    }

    void toJson(JsonObject &obj) {
        obj.set(JSON_NAME, this->Name);
        obj.set(JSON_PUMP_RATE, this->doseRate);
        obj.set(JSON_AMOUNT_ML, this->doseAmount);
        obj.set(JSON_DOSENAME, this->doseName);
        JsonObject &schedule = obj.createNestedObject(JSON_ON);
        schedule.set(JSON_HOUR, this->doseTime->next(Chronos::DateTime::now()).hour());
        schedule.set(JSON_MINUTE, this->doseTime->next(Chronos::DateTime::now()).minute());
        schedule.set(JSON_SECOND, this->doseTime->next(Chronos::DateTime::now()).second());
    }
};

class RelaySetting {
private:
    Chronos::Mark::Event *onTime;
    uint8_t hours;
    String Name;

public:
    RelaySetting() {

    }

    Chronos::Mark::Event &getOnTime() const {
        return *onTime;
    }

    void setOnTime(Chronos::Mark::Event *onTime) {
        RelaySetting::onTime = onTime;
    }

    Chronos::Span::Delta getDuration() const {
        return Chronos::Span::Hours(hours);
    }

    void setHours(uint8_t hours) {
        this->hours = hours;
    }

    const String &getName() const {
        return Name;
    }

    void setName(const String &Name) {
        this->Name = Name;
    }

    static RelaySetting *fromJson(const JsonObject &obj) {
        RelaySetting* relay = new RelaySetting();
        relay->Name = obj[JSON_NAME].as<String>();
        relay->setHours(obj[JSON_DURATION_HOURS].as<uint8_t>());
        relay->setOnTime(new Chronos::Mark::Daily(
                obj[JSON_ON][JSON_HOUR].as<Chronos::Hours>(),
                obj[JSON_ON][JSON_MINUTE].as<Chronos::Minutes>(),
                obj[JSON_ON][JSON_SECOND].as<Chronos::Seconds>()
        ));
        return relay;
    }

    void toJson(JsonObject &obj) {
        obj.set(JSON_NAME, this->Name);
        obj.set(JSON_DURATION_HOURS, this->hours);
        JsonObject &schedule = obj.createNestedObject("on");
        schedule.set(JSON_HOUR, this->onTime->next(Chronos::DateTime::now()).hour());
        schedule.set(JSON_MINUTE, this->onTime->next(Chronos::DateTime::now()).minute());
        schedule.set(JSON_SECOND, this->onTime->next(Chronos::DateTime::now()).second());
    }

};

class Settings {
public:
    PumpSetting *getPump1() const {
        return pump1;
    }

    Settings &setPump1(PumpSetting *pump1) {
        Settings::pump1 = pump1;
        return *this;
    }

    PumpSetting *getPump2() const {
        return pump2;
    }

    Settings setPump2(PumpSetting *pump2) {
        Settings::pump2 = pump2;
        return *this;
    }

    RelaySetting *getRelay1() const {
        return relay1;
    }

    Settings &setRelay1(RelaySetting *relay1) {
        Settings::relay1 = relay1;
        return *this;
    }

    Settings() {
    }

    static Settings &fromJson(const JsonObject &obj) {
        Settings settings;

        PumpSetting *pump1 = PumpSetting::fromJson(obj[JSON_PUMP1]);
        PumpSetting *pump2 = PumpSetting::fromJson(obj[JSON_PUMP2]);
        RelaySetting *relay1 = RelaySetting::fromJson(obj[JSON_RELAY1]);

        return settings.setPump1(pump1).setPump2(pump2).setRelay1(relay1);
    }

    void toJson(JsonObject &obj) {
        pump1->toJson(obj.createNestedObject(JSON_PUMP1));
        pump2->toJson(obj.createNestedObject(JSON_PUMP2));
        relay1->toJson(obj.createNestedObject(JSON_RELAY1));
    }

private:
    PumpSetting *pump1;
    PumpSetting *pump2;
    RelaySetting *relay1;
};

Settings getDefaultSettings(Settings &settings) {
    PumpSetting *pump1 = new PumpSetting();
    pump1->setName("Pump 1");
    pump1->setDoseName("Trace");
    pump1->setDoseAmount(15);
    pump1->setDoseRate(0.78);
    pump1->setDoseTime(new Chronos::Mark::Daily(7,0,0));
    PumpSetting *pump2 = new PumpSetting();
    pump2->setName("Pump 2");
    pump2->setDoseName("Macro");
    pump2->setDoseAmount(15);
    pump2->setDoseRate(0.73);
    pump2->setDoseTime(new Chronos::Mark::Daily(8,0,0));

    RelaySetting *relay1 = new RelaySetting();
    relay1->setName("Relay 1");
    relay1->setHours(10);
    relay1->setOnTime(new Chronos::Mark::Daily(7,30,0));

    return settings.setPump1(pump1).setPump2(pump2).setRelay1(relay1);
}

Settings loadSettings(Settings &settings) {
    File configFile = SPIFFS.open(JSON_FILE, "r");
    if(!configFile) {
        Serial.println("Could Not Load Config File");
        return getDefaultSettings(settings);
    }


    size_t size = configFile.size();
    if (size > 1024) {
        Serial.println("Config file size is too large");
        return getDefaultSettings(settings);
    }

    // Allocate a buffer to store contents of the file.
    std::unique_ptr<char[]> buf(new char[size]);

    // We don't use String here because ArduinoJson library requires the input
    // buffer to be mutable. If you don't use ArduinoJson, you may as well
    // use configFile.readString instead.
    configFile.readBytes(buf.get(), size);

    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.parseObject(buf.get());

    if (!json.success()) {
        Serial.println("Failed to parse config file");
        return getDefaultSettings(settings);
    }

    return Settings::fromJson(json);
}

Settings saveSettings(Settings &settings) {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    settings.toJson(json);


    File configFile = SPIFFS.open(JSON_FILE, "w");
    if (!configFile) {
        Serial.println("Failed to open config file for writing");
    }

    json.printTo(configFile);

    return settings;

}

#endif //FISHTANKCONTROLLER_SETTINGS_H
