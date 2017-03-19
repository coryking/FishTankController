//
// Created by Cory King on 3/6/17.
//

#include "Utils.h"

float Utils::NLtoML(uint32_t nl) {
    return nl / (1000.0 * 1000.0);
}

String Utils::printDT(Chronos::DateTime dt) {
    char dtString[30];
    sprintf(dtString, "%04i-%02i-%02iT%02i:%02i:%02iZ", dt.year(), dt.month(), dt.day(), dt.hour(), dt.minute(), dt.second());
    return String(dtString);
}

String Utils::printDT(DateTime dt) {
    char dtString[30];
    sprintf(dtString, "%04i-%02i-%02iT%02i:%02i:%02iZ", dt.year(), dt.month(), dt.day(), dt.hour(), dt.minute(), dt.second());
    return String(dtString);
}

void Utils::eventToJson(Chronos::Event::Occurrence event, JsonObject &obj) {
    obj["start"] = Utils::printDT(event.start);
    obj["end"] = Utils::printDT(event.finish);
    obj["id"] = event.id;
    obj["ongoing"] = event.isOngoing;
}
