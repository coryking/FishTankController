//
// Created by Cory King on 11/2/17.
//

#ifndef FISHTANKCONTROLLER_LIGHT_H
#define FISHTANKCONTROLLER_LIGHT_H


class Light {
public:
    Light(uint8 lightNum, int pin) : lightNum(lightNum), pin(pin) {
        pinMode(pin, OUTPUT);
        this->SetBrightness(0);
    }

    void SetBrightness(uint8 brightness) {
        this->brightness = brightness > 100 ? 100 : brightness;
        analogWrite(pin, this->getActualBrightness());
    }

    uint8 GetBrightness() {
        return this->brightness;
    }
    uint8 GetLightNumber() {
        return this->lightNum;
    }
private:
    uint16 getActualBrightness() {
        return map(brightness, 0,100, 0, 1023);
    }

    uint8 brightness;
    int pin;
    uint8 lightNum;

};

#endif //FISHTANKCONTROLLER_LIGHT_H
