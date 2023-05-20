#pragma once

#include "animation-base.hpp"
#include "const.h"
#include "rx.hpp"

#define StickReactiveAnimationRippleMaxSteps 16

class StickReactiveAnimation : public AnimationBase {
    public:
        void tick();

    private:
        uint8_t hue = 0; // controlled by yaw
        uint8_t pos = NUM_LEDS / 2; // controlled by roll
        uint8_t width = 5; // controlled by pitch
        uint8_t brightness = 255; // controlled by throttle

        void mapSticks();
        void mapThrottle();
        void mapYaw();
        void mapRoll();
        void mapPitch();
        void render();
        int trimStickPos(int originalVal);
};