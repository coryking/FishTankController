//
// Created by Cory King on 2/18/17.
//

#include <U8g2lib.h>
#include "display.h"
#include "pins.h"
#include "state.h"


U8G2* setupDisplay() {


    U8G2* display = new U8G2_SSD1306_128X64_VCOMH0_F_HW_I2C(U8G2_R0);

    // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
    //display->setI2CAddress(0x3C);

    display->begin(); //SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
    display->clearBuffer();
    display->setFont(u8g2_font_helvR08_tr);
    display->drawStr(0, 15, "Hello World");
    display->sendBuffer();
    return display;
}

void Display::OnUpdate(uint32_t deltaTime) {
    Task::OnUpdate(deltaTime);

    display->clearBuffer();
    display->setFont(u8g2_font_helvR08_tr);
    display->setCursor(0,15);
    display->print(
            GlobalState::instance()->getAquariumTempF());
    display->print(" *F");

    DateTime now = GlobalState::instance()->getTime();
    char ptime[30];
    sprintf(ptime,"%02d:%02d:%02d", now.hour(), now.minute(), now.second());
    int w = display->getStrWidth(ptime);
    display->drawStr(128-w, 15, ptime);

    if(GlobalState::instance()->getP1()->getMotorState() == MotorState::ACTIVE) {
        display->setCursor(0, 40);
        display->print(GlobalState::instance()->getP1()->getRemainingAmountMl());
        display->print(" mL");
    }
    if(GlobalState::instance()->getP2()->getMotorState() == MotorState::ACTIVE) {
        display->setCursor(0, 60);
        display->print(GlobalState::instance()->getP2()->getRemainingAmountMl());
        display->print(" mL");
    }
    display->sendBuffer();
}
