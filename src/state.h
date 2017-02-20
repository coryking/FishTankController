//
// Created by Cory King on 2/18/17.
//

#ifndef FISHTANKCONTROLLER_STATE_H
#define FISHTANKCONTROLLER_STATE_H

#include <RTClib.h>
#include "temp.h"
#include "pump.h"

class GlobalState {
    static GlobalState *s_instance;
public:
    class Builder;
private:

    Temp* _aquariumTemp;
    RTC_DS3231* rtc;
    Pump* p1;
    Pump* p2;

    GlobalState(Temp* aquariumTemp, RTC_DS3231* rtc, Pump* p1, Pump* p2){
        this->_aquariumTemp = aquariumTemp;
        this->rtc=rtc;
        this->p1 = p1;
        this->p2 = p2;
    }

public:
    float getAquariumTempF() const {
        return this->_aquariumTemp->getTempF();
    }

    DateTime getTime() const {
        return this->rtc->now();
    }

    Pump* getP1() {
        return p1;
    }
    Pump* getP2() {
        return p2;
    }
    static GlobalState *instance() {
        return s_instance;
    }
};

class GlobalState::Builder {
private:
    Temp* _aquariumTemp;
    RTC_DS3231* rtc;
    Pump* p1;
    Pump* p2;

public:
    Builder(){}

    Builder& setAquariumTemp(Temp* aquariumTemp) {
        this->_aquariumTemp=aquariumTemp;
        return *this;
    }

    Builder& setRTC(RTC_DS3231* rtc) {
        this->rtc=rtc;
        return *this;
    }

    Builder& setPumps(Pump* p1, Pump* p2) {
        this->p1 = p1;
        this->p2 = p2;
        return *this;
    }

    void initialize() {
        GlobalState::s_instance = new GlobalState(
                this->_aquariumTemp,
                this->rtc,
                this->p1,
                this->p2
        );
    }
};

#endif //FISHTANKCONTROLLER_STATE_H
