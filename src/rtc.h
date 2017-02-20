//
// Created by Cory King on 2/18/17.
//

#ifndef FISHTANKCONTROLLER_RTC_H
#define FISHTANKCONTROLLER_RTC_H

#include <Arduino.h>
#include <RTClib.h>
#include <U8g2lib.h>

RTC_DS3231* setupRtc(U8G2 *display);

#endif //FISHTANKCONTROLLER_RTC_H
