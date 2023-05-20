#pragma once

#include <FastLED.h>
#include <functional>

/**
 * Sets the led at a certain index to a certain color
 * @param index index of led
*/
typedef std::function<void(int, CRGB)> SetLedFunc;

/**
 * Gets the led at a certain index
 * @param index index of led
*/
typedef std::function<CRGB(int)> GetLedFunc;

/**
 * Fades all leds by a certain amount to black
 * @param fadeAmount amount to fade by
*/
typedef std::function<void(int)> FadeToBlackFunc;

class AnimationBase {
    public:
        AnimationBase() {
            // set dummy as default
            this->setLed = [](int index, CRGB color) {};
            this->getLed = [](int index) { return CRGB::Black; };
            this->fadeToBlack = [](int index) {};
        };
        void begin(SetLedFunc setLed, GetLedFunc getLed, FadeToBlackFunc fadeToBlack) {
            this->setLed = setLed;
            this->getLed = getLed;
            this->fadeToBlack = fadeToBlack;
        };
        virtual void tick() = 0;
    
    protected:
        SetLedFunc setLed;
        GetLedFunc getLed;
        FadeToBlackFunc fadeToBlack;
};