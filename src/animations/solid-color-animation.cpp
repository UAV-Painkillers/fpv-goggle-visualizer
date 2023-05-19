#include "solid-color-animation.hpp"

void SolidColorAnimation::tick() {
    EVERY_N_MILLISECONDS(500) {
        for (int i = 0; i < NUM_LEDS; i++) {
            this->setLed(i, color);
        }
    }
}