//
// Created by Cory King on 2/18/17.
//

#ifndef FISHTANKCONTROLLER_PINS_H
#define FISHTANKCONTROLLER_PINS_H

#include <Arduino.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS D5

#define SCL_PIN D7
#define SDA_PIN D6

#define LIGHT_1_PIN D0
#define LIGHT_2_PIN D8

#define SHIFT_CLOCK_PIN D2
#define SHIFT_LATCH_PIN D3
#define SHIFT_DATA_PIN D4
#define SHIFT_READ_PIN D1

#define SHIFT_PINS 8

#define PUMP1_PIN 4
#define PUMP2_PIN 5

#define RELAY1_PIN 6
#define RELAY2_PIN 7

#define BUTTON1_PIN 1
#define BUTTON2_PIN 2
#define BUTTON3_PIN 3

#endif //FISHTANKCONTROLLER_PINS_H
