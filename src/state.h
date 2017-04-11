//
// Created by Cory King on 2/18/17.
//

#ifndef FISHTANKCONTROLLER_STATE_H
#define FISHTANKCONTROLLER_STATE_H

#include <RTClib.h>
#include <Chronos.h>
#include "temp.h"
#include "pump.h"
#include "relay.h"
#include "Button.h"
#include "DoseKeeper.h"


class GlobalState {
    static GlobalState *s_instance;
public:
    class Builder;
private:

    Temp* _aquariumTemp;
    RTC_DS3231* rtc;
    Pump* p1;
    Pump* p2;
    Relay* r1;
    Relay* r2;
    Button *b1;
    Button *b2;
    DoseKeeper *dk1;
    DoseKeeper *dk2;

    GlobalState(Temp* aquariumTemp, RTC_DS3231* rtc, Pump* p1, Pump* p2, Relay* r1, Relay* r2, Button* b1, Button* b2, DoseKeeper* dk1, DoseKeeper *dk2){
        this->_aquariumTemp = aquariumTemp;
        this->rtc=rtc;
        this->p1 = p1;
        this->p2 = p2;
        this->r1 = r1;
        this->r2 = r2;
        this->b1 = b1;
        this->b2 = b2;
        this->dk1 = dk1;
        this->dk2 = dk2;
    }

public:
    float getAquariumTempF() const {
        return this->_aquariumTemp->getTempF();
    }
    float getAquariumTempC() const {
        return this->_aquariumTemp->getTempC();
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
    Relay* getR1() {
        return r1;
    }
    Relay* getR2() {
        return r2;
    }
    Button* getB1() {
        return b1;
    }

    Button* getB2() {
        return  b2;
    }

    DoseKeeper* getDk1() {
        return dk1;
    }

    DoseKeeper* getDk2() {
        return dk2;
    }

    bool isDispensing() {
        return p1->isDispensing() || p2->isDispensing();
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
    Relay* r1;
    Relay* r2;
    Button *b1;
    Button *b2;
    DoseKeeper *dk1;
    DoseKeeper *dk2;

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

    Builder& setRelays(Relay* r1, Relay* r2) {
        this->r1=r1;
        this->r2=r2;
        return *this;
    }

    Builder& setButtons(Button* b1, Button* b2) {
        this->b1=b1;
        this->b2=b2;
        return *this;
    }

    Builder& setDoseKeepers(DoseKeeper* dk1, DoseKeeper *dk2) {
        this->dk1 = dk1;
        this->dk2 = dk2;
    }

    void initialize() {
        GlobalState::s_instance = new GlobalState(
                this->_aquariumTemp,
                this->rtc,
                this->p1,
                this->p2,
                this->r1,
                this->r2,
                this->b1,
                this->b2,
                this->dk1,
                this->dk2
        );
    }
};

#endif //FISHTANKCONTROLLER_STATE_H
