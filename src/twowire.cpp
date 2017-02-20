//
// Created by Cory King on 2/18/17.
//

#include <Arduino.h>
#include <Wire.h>

#include "twowire.h"
#include "pins.h"

void setupTwoWire() {
    Wire.begin(SDA_PIN, SCL_PIN);
}