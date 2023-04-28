#pragma once

#include "../animation.hpp"

class SingleColorBreathingAnimation : public AnimationBase {
    public:
        SingleColorBreathingAnimation(CHSV color, uint8_t minBrightness, int fullCycleDuration) {
            this->color = color;
            this->reverse = false;
            this->minBrightness = minBrightness;
            this->brightness = minBrightness;
            uint8_t distanceToMaxBrightness = 255 - minBrightness;
            this->frameDuration = fullCycleDuration / (distanceToMaxBrightness * 2);
        }
        void tick();

    private:
        CHSV color;
        int frameDuration;
        bool reverse;
        uint8_t brightness;
        uint8_t minBrightness;
};