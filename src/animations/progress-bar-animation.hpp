#pragma once

#include "../animation.hpp"

class ProgressBarAnimation : public AnimationBase {
    public:
        ProgressBarAnimation(CRGB barColor, CRGB backgroundColor) {
            this->barColor = barColor;
            this->backgroundColor = backgroundColor;
            this->maxLedIndexActive = 0;
        };
        void tick();
        void setProgress(uint8_t progress);

    private:
        CRGB barColor;
        CRGB backgroundColor;
        int maxLedIndexActive;
};