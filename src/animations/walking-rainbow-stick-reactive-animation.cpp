#include "walking-rainbow-stick-reactive-animation.hpp"

void WalkingRainbowStickReactiveAnimation::tick() {
    mapSticks();
    render();
}

void WalkingRainbowStickReactiveAnimation::mapBrightness() {
    // stick is from 1000 to 2000
    // brightness is from 0 to 255

    // throttle maps to brightness
    int throttle = trimStickPos(RX::throttle);
    
    this->brightness = map(throttle, 1000, 2000, 50, 255);
}

void WalkingRainbowStickReactiveAnimation::mapHue() {
    // hue is from 0 to 255
    // stick is from 1000 to 2000
    // stick center is at 1500
    // dont move in a range of +/- 50 around the middle

    // yaw moves the hue
    // the further away from the middle, the faster it moves
    // hue cannot go below 0 or above 255 and needs to wrap around
    EVERY_N_MILLISECONDS(50) {
        int yaw = trimStickPos(RX::yaw) - 1500;

        if (abs(yaw) < 50) {
            return;
        }

        int yawSpeed = map(abs(yaw), 0, 500, 1, 50);
        int nextHue = this->hue;
        nextHue += yawSpeed * (yaw < 0 ? -1 : 1);

        if (nextHue < 0) {
            nextHue = 255 + nextHue;
        } else if (nextHue > 255) {
            nextHue = nextHue - 255;
        }

        this->hue = nextHue;
    }
}

void WalkingRainbowStickReactiveAnimation::mapPos() {
    EVERY_N_MILLISECONDS(100) {
        int roll = trimStickPos(RX::roll) - 1500;

        roll *= 1.6;

        if (roll < -500) {
            roll = -500;
        } else if (roll > 500) {
            roll = 500;
        }

        this->pos = map(roll, 500, -500, 0, NUM_LEDS);
    }
}

void WalkingRainbowStickReactiveAnimation::mapWidth() {
    // width is from 30% to 80% of NUM_LEDS
    // stick is from 1000 to 2000 with center at 1500

    int pitch = trimStickPos(RX::pitch);

    int center = 1500;
    int distanceFromCenter = abs(pitch - center);

    distanceFromCenter *= 1.6;
    
    int minWidth = NUM_LEDS * 0.4;
    int maxWidth = NUM_LEDS * 0.9;
    this->width = map(distanceFromCenter, 0, 500, minWidth, maxWidth);
}

void WalkingRainbowStickReactiveAnimation::mapSticks() {
    mapBrightness();
    mapHue();
    mapPos();
    mapWidth();
}

void WalkingRainbowStickReactiveAnimation::render() {
    for (int i = 0; i < NUM_LEDS; i++) {
        this->setLed(i, CRGB::Black);
    }

    if (RX::throttle > (CHANNEL_HIGH_MIN * this->highThrottleThresholdPercentage)) {
        EVERY_N_MILLISECONDS(this->highThrottleBlinkInterval) {
            this->highThrottleBlinkState = !this->highThrottleBlinkState;
        }

        if (!this->highThrottleBlinkState) {
            return;
        }
    }

    int start = this->pos - this->width / 2;
    if (start < 0) {
        start = 0;
    }
    int end = this->pos + this->width / 2;
    if (end > NUM_LEDS) {
        end = NUM_LEDS;
    }

    for (int i = start; i < end; i++) {
        int hue = this->hue + map(i, start, end, 0, 255);
        hue = hue % 255;
        this->setLed(i, CHSV(hue, 255, this->brightness));
    }
}

int WalkingRainbowStickReactiveAnimation::trimStickPos(int originalVal) {
    if (originalVal < 1000) {
        return 1000;
    } else if (originalVal > 2000) {
        return 2000;
    } else {
        return originalVal;
    }
}