#include "blink-animation.hpp"

void BlinkAnimation::tick() {
    EVERY_N_MILLISECONDS(this->intervalMs) {
        CRGB color = this->on ? this->onColor : this->offColor;
        
        for (int i = 0; i < NUM_LEDS; i++) {
            this->setLed(i, color);
        }

        this->on = !this->on;
    }
}