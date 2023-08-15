#pragma once

#include "animation-base.hpp"
#include "const.h"
#include "../rx.hpp"
#include "../logger.hpp"

#define WalkingRainbowStickReactiveAnimationRippleMaxSteps 16

class WalkingRainbowStickReactiveAnimation : public AnimationBase {
    public:
        void tick();

    private:
        uint8_t hue = 0; // controlled by yaw
        uint8_t pos = NUM_LEDS / 2; // controlled by roll
        uint8_t width = 5; // controlled by pitch
        uint8_t brightness = 255; // controlled by throttle
        const uint8_t highThrottle = 90; // percent
        int highThrottleBlinkInterval = 100;
        bool highThrottleBlinkState = false;

        void mapSticks();
        void mapBrightness();
        void mapHue();
        void mapPos();
        void mapWidth();
        void render();
        int trimStickPos(int originalVal);
        void renderHighThrottle();
};