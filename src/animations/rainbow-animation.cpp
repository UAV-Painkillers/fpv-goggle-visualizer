#include "rainbow-animation.hpp"

void RainbowAnimation::tick() {
    EVERY_N_MILLISECONDS(15) {
        for (int i = 0; i < NUM_LEDS; i++) {
            int ledHue = this->hue + (i * 20);
            if (ledHue > 255) {
                ledHue -= 255;
            }
            this->setLed(i, CHSV(ledHue, 255, 255));
        }
        this->hue++;
    }
}