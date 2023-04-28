#pragma once

#include "../animation.hpp"

class RainbowAnimation : public AnimationBase {
    public:
        void tick() override;

    private:
        int hue = 0;
};
