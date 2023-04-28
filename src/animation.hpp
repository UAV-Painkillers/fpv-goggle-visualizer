#pragma once

#include "const.h"
#include <FastLED.h>
#include <functional>

enum AnimationType {
    AnimationRunningBlueDot,
};

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

class AnimationController {
    public:
        void begin();
        void tick();
        void setAnimation(AnimationBase *animation);
        void clear();
        void setBrightness(uint8_t brightness);

    private:
        AnimationBase* currentAnimation;
        CRGBArray<NUM_LEDS> leds;
};
