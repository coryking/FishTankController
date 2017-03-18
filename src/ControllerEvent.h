//
// Created by Cory King on 3/18/17.
//

#ifndef FISHTANKCONTROLLER_CONTROLLEREVENT_H
#define FISHTANKCONTROLLER_CONTROLLEREVENT_H


#include <Chronos.h>
#include <map>

#define P1_EVENT 0
#define P2_EVENT 1
#define RELAY1_EVENT 10
#define RELAY2_EVENT 11

typedef void (*eventAction)(void) ;
typedef std::map<Chronos::EventID, bool> EventStates;

class ControllerEvent {
public:
    void setActiveState(bool isActive);

    bool isEventActive();
    ControllerEvent(
            const eventAction onEventActive,
            const eventAction onEventInactive
    ) {
        this->onEventActive = onEventActive;
        this->onEventInactive = onEventInactive;
    }


private:
    eventAction onEventActive;
    eventAction onEventInactive;
    bool eventActive;

};

typedef std::map<Chronos::EventID, ControllerEvent*> ControllerEvents;


class ControllerEventList : public ControllerEvents {
public:
    ControllerEventList() : ControllerEvents() {

    }
    ControllerEventList& setEvent(Chronos::EventID eventID, eventAction onEventActive, eventAction onEventInactive);
    ControllerEvent* getEvent(Chronos::EventID);
};


#endif //FISHTANKCONTROLLER_CONTROLLEREVENT_H
