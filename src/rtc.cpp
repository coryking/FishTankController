//
// Created by Cory King on 2/18/17.
//

#include <RTClib.h>
#include <Task.h>
#include "rtc.h"

RTC_DS3231 *setupRtc(U8G2 *display) {
    RTC_DS3231* rtc = new RTC_DS3231();

    if (! rtc->begin()) {
        display->clearBuffer();
        display->println("Couldn't find RTC");
        display->sendBuffer();
        while (1);
    }

    if (rtc->lostPower()) {
        // following line sets the RTC to the date & time this sketch was compiled
        rtc->adjust(DateTime(F(__DATE__), F(__TIME__)));
        // This line sets the RTC with an explicit date & time, for example to set
        // January 21, 2014 at 3am you would call:
        // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));

        delay(2000);
    }
    display->clearBuffer();
    display->print("Found RTC Device");
    display->sendBuffer();

    return rtc;

}
