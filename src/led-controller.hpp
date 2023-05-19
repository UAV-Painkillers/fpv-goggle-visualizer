#pragma once

#include "const.h"
#include <FastLED.h>
#include "animations/solid-color-animation.hpp"
#include "animations/animation-base.hpp"


class LedController {
    public:
        void begin();
        void tick();
        void setAnimation(AnimationBase *animation, bool clear = true);
        void clear();
        void setBrightness(uint8_t brightness);
        void fillSolidColor(CRGB color);

    private:
        AnimationBase* currentAnimation;
        CRGBArray<NUM_LEDS> leds;
        uint8_t brightness = 255;
        SolidColorAnimation _emptyAnimation = SolidColorAnimation(CRGB::Black);
};
