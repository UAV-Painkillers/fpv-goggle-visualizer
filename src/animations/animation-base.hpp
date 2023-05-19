#pragma once

#include <FastLED.h>
#include <functional>

// function to set led at certain index
typedef std::function<void(int, CRGB)> SetLedFunc;

class AnimationBase {
    public:
        AnimationBase() {
            // set dummy as default
            this->setLed = [](int index, CRGB color) {};
        };
        void begin(SetLedFunc setLed) {
            this->setLed = setLed;
        };
        virtual void tick() = 0;
    
    protected:
        SetLedFunc setLed;
};