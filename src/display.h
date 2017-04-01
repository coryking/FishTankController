//
// Created by Cory King on 2/18/17.
//

#ifndef FISHTANKCONTROLLER_DISPLAY_H
#define FISHTANKCONTROLLER_DISPLAY_H


#include <Task.h>
#include <U8g2lib.h>
#include "state.h"

U8G2* setupDisplay();

class Display : public Task {
private:
    U8G2* display;
    void showPump(int hLine, Pump* pump, DoseKeeper* keeper);
public:
    Display(
            U8G2* display,
            uint32_t timeInterval) : Task(timeInterval) {
        this->display=display;
    }
    virtual void OnUpdate(uint32_t deltaTime);
};

#endif //FISHTANKCONTROLLER_DISPLAY_H
