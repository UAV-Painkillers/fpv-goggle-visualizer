#include "running-blue-dot-animation.hpp"

void RunningBlueDotAnimation::tick() {
    EVERY_N_MILLISECONDS(this->stepDuration) {
        for(int i = 0; i < NUM_LEDS; i++) {
            this->setLed(i, this->backgroundColor);
        }

        if (!this->reverse) {
            this->currentLedIndex++;
            if (this->currentLedIndex >= NUM_LEDS) {
                this->currentLedIndex = NUM_LEDS -2;
                this->reverse = true;
            }
        } else {
            this->currentLedIndex--;
            if (this->currentLedIndex < 0) {
                this->currentLedIndex = 1;
                this->reverse = false;
            }
        }

        this->setLed(this->currentLedIndex, this->dotColor);
    }
}