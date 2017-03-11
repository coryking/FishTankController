//
// Created by Cory King on 3/11/17.
//

#ifndef FISHTANKCONTROLLER_DOSEKEEPER_H
#define FISHTANKCONTROLLER_DOSEKEEPER_H

#include<Arduino.h>
#include <stdint.h>

class DoseKeeper {
public:
    uint8_t getIntervals() const;
    float getDailyDoseMl() const;
    /**
     * Get the dose for a given, zero-based interval
     * @param interval zero-based interval
     * @return the dose to dispense in Ml
     */
    float getDoseForInterval(uint8_t interval);
    const String &getDoseName() const;
    DoseKeeper(const uint8_t intervals, const float dailyDoseMl, const String doseName);
private:
    uint8_t intervals;
    float dailyDoseMl;
    String doseName;

};


#endif //FISHTANKCONTROLLER_DOSEKEEPER_H
