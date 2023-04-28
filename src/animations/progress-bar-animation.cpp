#include "progress-bar-animation.hpp"

void ProgressBarAnimation::tick() {
    EVERY_N_MILLISECONDS(100) {
        for(int i = 0; i < NUM_LEDS; i++) {
            if (i <= this->maxLedIndexActive) {
                this->setLed(i, this->barColor);
                continue;
            } else {
                this->setLed(i, this->backgroundColor);
            }
        }
    }
};

void ProgressBarAnimation::setProgress(uint8_t progress) {
    // map progress (0-100) to maxLedIndexActive (0-NUM_LEDS)
    int newIndex = map(progress, 0, 100, 0, NUM_LEDS);
    if (newIndex == this->maxLedIndexActive) {
        return;
    }

    this->maxLedIndexActive = newIndex;
}