//
// Created by Cory King on 2/18/17.
//

#include <U8g2lib.h>
#include "Utils.h"
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

void Display::showMainTemp() {
    display->setFont(u8g2_font_helvB24_tf);
    display->home();
    display->setCursor(10, 50);
    display->print(
            GlobalState::instance()->getAquariumTempF());
    display->print("\xB0");
    display->print("F");
}

void Display::showLittleTemp() {
    display->setFont(u8g2_font_helvR08_tf);
    display->setCursor(0,15);
    display->print(
            GlobalState::instance()->getAquariumTempF());
    display->print("\xB0");
    display->print("F");
}

void Display::showPump(int hLine, Pump* pump, DoseKeeper *keeper) {
    if(pump->isDispensing()) {
        display->setFont(u8g2_font_helvR10_tr);
        display->setCursor(0, hLine);
        display->print(keeper->getDoseName());
        display->setCursor(60,hLine);
        display->print(Utils::NLtoML(pump->getAmountDispensedNl()));
        display->print(" mL");
    }
}

void Display::showTime() {
    display->setFont(u8g2_font_helvR08_tr);
    DateTime now = GlobalState::instance()->getTime();
    char ptime[30];
    sprintf(ptime,"%02d:%02d:%02d", now.hour(), now.minute(), now.second());
    int w = display->getStrWidth(ptime);
    display->drawStr(128-w, 15, ptime);
}

void Display::OnUpdate(uint32_t deltaTime) {
    Task::OnUpdate(deltaTime);

    display->clearBuffer();
    if(GlobalState::instance()->isDispensing()) {
        this->showLittleTemp();
    } else {
        this->showMainTemp();
    }

    this->showTime();

    this->showPump(40, GlobalState::instance()->getP1(), GlobalState::instance()->getDk1());
    this->showPump(60, GlobalState::instance()->getP2(), GlobalState::instance()->getDk2());

    display->sendBuffer();
}
