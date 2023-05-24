#include "running-dot-animation.hpp"

void RunningDotAnimation::tick() {
    EVERY_N_MILLISECONDS(this->stepDuration) {
        for(int i = 0; i < NUM_LEDS; i++) {
            this->setLed(i, this->backgroundColor);
        }

        if (!this->reverse) {
            this->currentLedIndex++;
            if (this->currentLedIndex >= NUM_LEDS) {
                this->currentLedIndex = NUM_LEDS -2;
                this->reverse = true;
                cycles++;
            }
        } else {
            this->currentLedIndex--;
            if (this->currentLedIndex < 0) {
                this->currentLedIndex = 1;
                this->reverse = false;
                cycles++;
            }
        }

        if (this->cycles >= this->maxCycles && this->maxCycles != -1) {
            if (!this->completed) {
                this->completed = true;
                this->completedCallback();
            }
            return;
        }

        this->setLed(this->currentLedIndex, this->dotColor);
    }
}

void RunningDotAnimation::onCompleted(CompletedCallback callback) {
    this->completedCallback = callback;
}