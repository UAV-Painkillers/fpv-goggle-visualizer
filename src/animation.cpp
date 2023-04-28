#include "animation.hpp"
#include "animations/running-blue-dot-animation.hpp"
#include "const.h"

void AnimationController::begin() {
    Serial.print("Setup LED...");
    FastLED.addLeds<WS2811, LED_PIN, GRB>(this->leds, NUM_LEDS)
        .setCorrection(TypicalLEDStrip);

    this->leds.fill_solid(CRGB::Black);
    FastLED.show();

    Serial.println("done");
};

void AnimationController::setAnimation(AnimationBase* animation) {
    if (this->currentAnimation != nullptr) {
        delete this->currentAnimation;
    }

    this->currentAnimation = animation;
    this->currentAnimation->begin(
        [this](int index, CRGB color) {
            this->leds[index] = color;
        }
    );
};

int indexTest = 0;
void AnimationController::tick() {
    if (this->currentAnimation == nullptr) {
        return;
    }

    this->currentAnimation->tick();


    EVERY_N_MILLISECONDS(1000 / LED_FPS) {
        FastLED.show();
    }
};

void AnimationController::clear() {
    this->leds.fill_solid(CRGB::Black);
    FastLED.show();
    
    if (this->currentAnimation != nullptr) {
        // TODO: how to clean up? the next line causes a crash
        this->currentAnimation = NULL;
        // delete this->currentAnimation;
    }
};

void AnimationController::setBrightness(uint8_t brightness) {
    FastLED.setBrightness(brightness);
};