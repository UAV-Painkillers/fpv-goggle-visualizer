#include "stick-reactive-animation.hpp"

void StickReactiveAnimation::tick() {
    EVERY_N_MILLISECONDS(50) {
        mapSticks();
        render();
    };
}

void StickReactiveAnimation::mapThrottle() {
    // stick is from 1000 to 2000
    // brightness is from 0 to 255

    // throttle maps to brightness
    int throttle = trimStickPos(RX::throttle);
    
    this->brightness = map(throttle, 1000, 2000, 0, 255);
}

void StickReactiveAnimation::mapYaw() {
    // hue is from 0 to 255
    // stick is from 1000 to 2000
    // stick center is at 1500
    // dont move in a range of +/- 50 around the middle

    // yaw moves the hue
    // the further away from the middle, the faster it moves
    // hue cannot go below 0 or above 255 and needs to wrap around
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

void StickReactiveAnimation::mapRoll() {
    // pos is from 0 to NUM_LEDS
    // stick is from 1000 to 2000
    // dont move in a range of +/- 50 around the middle


    // roll moves the pos
    // the further away from the middle, the faster it moves
    // pos cannot go below 0 or above NUM_LEDS and needs to wrap around
    int roll = trimStickPos(RX::roll) - 1500;

    if (abs(roll) < 50) {
        return;
    }

    int rollSpeed = map(abs(roll), 0, 500, 1, 5);
    int nextPos = this->pos;
    nextPos += rollSpeed * (roll < 0 ? -1 : 1);

    if (nextPos < 0) {
        nextPos = NUM_LEDS + nextPos;
    } else if (nextPos > NUM_LEDS) {
        nextPos = nextPos - NUM_LEDS;
    }

    this->pos = nextPos;
}

void StickReactiveAnimation::mapPitch() {
    // width is from 30% to 80% of NUM_LEDS
    // stick is from 1000 to 2000 with center at 1500

    int pitch = trimStickPos(RX::pitch);
    int center = 1500;
    int distanceFromCenter = abs(pitch - center);
    
    int minWidth = NUM_LEDS * 0.3;
    int maxWidth = NUM_LEDS * 0.8;
    this->width = map(distanceFromCenter, 0, 500, minWidth, maxWidth);
}

void StickReactiveAnimation::mapSticks() {
    mapThrottle();
    mapYaw();
    mapRoll();
    mapPitch();
}

void StickReactiveAnimation::render() {
    // fade last frame
    // this->fadeToBlack(20);
    for (int i = 0; i < NUM_LEDS; i++) {
        this->setLed(i, CRGB::Black);
    }

    // render a rainbow stripe that fades away to the sides
    // its center is at pos
    // its center hue is hue
    // its center brightness is brightness
    // it should wrap around if it goes beyond the end of the strip

    // first create an array that represents the rainbow (size equals NUM_LEDS)
    // then fade it to the sides
    // then shift it by pos
    // then apply it to the strip

    CHSV rainbow[NUM_LEDS];

    for (int i = 0; i < NUM_LEDS; i++) {
        if (i >= this->width) {
            rainbow[i] = CHSV(0, 0, 0);
            continue;
        }

        int hue = this->hue + map(i, 0, NUM_LEDS, 0, 255);
        if (hue > 255) {
            hue = hue - 255;
        }
        rainbow[i] = CHSV(hue, 255, this->brightness);
    }

    // fade to the sides
    /*for (int i = 0; i < NUM_LEDS; i++) {
        int fade = map(i, 0, NUM_LEDS / 2, 255, 0);
        rainbow[i].v = (rainbow[i].v * fade) / 255;
        rainbow[NUM_LEDS - i - 1].v = (rainbow[NUM_LEDS - i - 1].v * fade) / 255;
    }*/

    // shift by pos
    CHSV shiftedRainbow[NUM_LEDS];

    for (int i = 0; i < NUM_LEDS; i++) {
        int shiftedIndex = i + this->pos;
        if (shiftedIndex >= NUM_LEDS) {
            shiftedIndex = shiftedIndex - NUM_LEDS;
        }
        shiftedRainbow[shiftedIndex] = rainbow[i];
    }

    // apply to strip
    for (int i = 0; i < NUM_LEDS; i++) {
        this->setLed(i, shiftedRainbow[i]);
    }
}

int StickReactiveAnimation::trimStickPos(int originalVal) {
    if (originalVal < 1000) {
        return 1000;
    } else if (originalVal > 2000) {
        return 2000;
    } else {
        return originalVal;
    }
}