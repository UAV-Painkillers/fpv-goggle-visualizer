#pragma once

#include "animation-base.hpp"
#include "const.h"

class SolidColorAnimation : public AnimationBase {
    public:
        SolidColorAnimation(CRGB color) {
            this->color = color;
        }
        void tick();

    private:
        CRGB color;
};