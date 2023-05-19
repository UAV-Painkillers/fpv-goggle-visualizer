#pragma once

#include "animation-base.hpp"
#include "const.h"

class RainbowAnimation : public AnimationBase {
    public:
        void tick() override;

    private:
        int hue = 0;
};
