#include "led-controller.hpp"
#include "const.h"
#include "logger.hpp"

void LedController::begin() {
    Logger::getInstance().logLn("Setup LED...");
    FastLED.addLeds<LED_TYPE, LED_PIN, LED_COLOR_ORDER>(this->leds, NUM_LEDS);
    // FastLED.setCorrection(TypicalLEDStrip);
    // FastLED.setMaxPowerInVoltsAndMilliamps(LED_VOLTS, LED_MAX_MILLIAMPS);
    
    // make up for the blue case
    FastLED.setCorrection(CRGB(255, 255, 200));

    Logger::getInstance().logLn("done");
};

void LedController::setAnimation(AnimationBase* animation, bool clear) {
    if (clear) {
        this->clear();
    }

    this->currentAnimation = animation;
    this->currentAnimation->begin(
        [this](int index, CRGB color) {
            this->leds[NUM_LEDS - 1 - index] = color;
        },
        [this](int index) {
            return this->leds[NUM_LEDS - 1 - index];
        },
        [this](int fadeAmount) {
            fadeToBlackBy(this->leds, NUM_LEDS, fadeAmount);
        }
    );
};

int indexTest = 0;
void LedController::tick() {
    if (this->currentAnimation == nullptr) {
        return;
    }

    this->currentAnimation->tick();


    EVERY_N_MILLISECONDS(1000 / LED_FPS) {
        FastLED.show();
    }
};

void LedController::clear() {
    if (this->currentAnimation != nullptr) {
        // TODO: how to clean up? the next line causes a crash
        this->currentAnimation = NULL;
        // delete this->currentAnimation;
    }

    setAnimation(&_emptyAnimation, false);
};

void LedController::setBrightness(uint8_t newInputBrightness) {
    int newBrightness = map(newInputBrightness, 0, 255, LED_MIN_DIMMED_BRIGHTNESS, 255);

    if (newBrightness == brightness) {
        return;
    }

    brightness = newBrightness;
    FastLED.setBrightness(brightness);
    FastLED.show();
};

void LedController::fillSolidColor(CRGB color) {
    for (int i = 0; i < NUM_LEDS; i++) {
        this->leds[i] = color;
    }
    FastLED.show();
};