//
// Created by Cory King on 3/11/17.
//

#include "DoseKeeper.h"

uint8_t DoseKeeper::getIntervals() const {
    return intervals;
}

float DoseKeeper::getDailyDoseMl() const {
    return dailyDoseMl;
}

float DoseKeeper::getDoseForInterval(uint8_t interval) {
    if(interval >= this->intervals)
        return 0;

    // A simple linear way to determine the dose...
    return this->dailyDoseMl / this->intervals;
}

const String &DoseKeeper::getDoseName() const {
    return doseName;
}

DoseKeeper::DoseKeeper(const uint8_t intervals, const float dailyDoseMl, const String doseName) {
    this->intervals = intervals;
    this->dailyDoseMl = dailyDoseMl;
    this->doseName = doseName;
}

void DoseKeeper::setIntervals(uint8_t intervals) {
    DoseKeeper::intervals = intervals;
}

void DoseKeeper::setDailyDoseMl(float dailyDoseMl) {
    DoseKeeper::dailyDoseMl = dailyDoseMl;
}

void DoseKeeper::setDoseName(const String &doseName) {
    DoseKeeper::doseName = doseName;
}
