#include "../animation.hpp"

class RunningBlueDotAnimation: public AnimationBase {
    public:
        RunningBlueDotAnimation(uint32_t totalDurationMs, CRGB dotColor, CRGB backgroundColor): AnimationBase() {
            this->stepDuration = totalDurationMs / (NUM_LEDS * 2);
            this->dotColor = dotColor;
            this->backgroundColor = backgroundColor;
        }
        void tick() override;

    private:
        int currentLedIndex = 4;
        uint32_t stepDuration;
        bool reverse = false;
        CRGB dotColor;
        CRGB backgroundColor;
};