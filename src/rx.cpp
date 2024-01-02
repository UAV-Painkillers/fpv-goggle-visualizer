#include "rx.hpp"
#include "logger.hpp"

bool RX::isArmed = false;
bool RX::ledSwitchIsOn = false;
int RX::throttle = CHANNEL_LOW_MAX;
int RX::roll = CHANNEL_LOW_MAX;
int RX::pitch = CHANNEL_LOW_MAX;
int RX::yaw = CHANNEL_LOW_MAX;
int RX::ledBrightness = 255;
bool RX::sticksAreAtTopCenter = false;
bool RX::sticksAreAtBottomCenter = false;
bool RX::sticksAreAtTopOutside = false;
bool RX::sticksAreAtBottomInside = false;

void RX::begin() {
    Logger::getInstance().logLn("Setup RX...");
    rxSerial.begin(RX_BAUD, EspSoftwareSerial::SWSERIAL_8N1, RX_SERIAL_RX_PIN, RX_SERIAL_TX_PIN);

    crsf.onPacketChannels = &RX::onChannelChanged;
    Logger::getInstance().logLn("done");   
}

void RX::loop() {
    crsf.loop();
}

inline void updateSticksAreAtTopCenter() {
    RX::sticksAreAtTopCenter = false;

    // throttle or pitch not fully up
    if (RX::throttle < CHANNEL_HIGH_MIN || RX::pitch < CHANNEL_HIGH_MIN) {
      return;
    }

    // yaw not centered
    if (RX::yaw < CHANNEL_CENTER_START || RX::yaw > CHANNEL_CENTER_END) {
      return;
    }

    // roll not centered
    if (RX::roll < CHANNEL_CENTER_START || RX::roll > CHANNEL_CENTER_END) {
      return;
    }

    RX::sticksAreAtTopCenter = true;
}

inline void updateSticksAreAtBottomCenter() {
    RX::sticksAreAtBottomCenter = false;

    // throttle or pitch not fully down
    if (RX::throttle > CHANNEL_LOW_MAX || RX::pitch > CHANNEL_LOW_MAX) {
      return;
    }

    // yaw not centered
    if (RX::yaw < CHANNEL_CENTER_START || RX::yaw > CHANNEL_CENTER_END) {
      return;
    }

    // roll not centered
    if (RX::roll < CHANNEL_CENTER_START || RX::roll > CHANNEL_CENTER_END) {
      return;
    }

    RX::sticksAreAtBottomCenter = true;
}

inline void updateLedBrightnessState() {
    int nextLedBrightness = 255;

    #ifdef LED_BRIGHTNESS
        nextLedBrightness = LED_BRIGHTNESS;
    #endif

    #ifdef LED_BRIGHTNESS_CHANNEL
        int ledBrightnessChannelValue = crsf.getChannel(LED_BRIGHTNESS_CHANNEL);
        if (LED_BRIGHTNESS_CHANNEL_INVERTED) {
            nextLedBrightness = map(ledBrightnessChannelValue, CHANNEL_LOW_MAX, CHANNEL_HIGH_MIN, 0, 255);
        } else {
            nextLedBrightness = map(ledBrightnessChannelValue, CHANNEL_LOW_MAX, CHANNEL_HIGH_MIN, 255, 0);
        }
    #endif

    if (nextLedBrightness != RX::ledBrightness) {
        RX::ledBrightness = nextLedBrightness;
    }
}

inline void updateOtaIsActiveState() {
    RX::sticksAreAtBottomInside = false;
    RX::sticksAreAtTopOutside = false;

    // sticks to the top outside position
    if (
        RX::throttle >= CHANNEL_HIGH_MIN &&
        RX::yaw <= CHANNEL_LOW_MAX &&
        RX::roll >= CHANNEL_HIGH_MIN &&
        RX::pitch >= CHANNEL_HIGH_MIN
    ) {
        RX::sticksAreAtTopOutside = true;
    }

    // sticks to the bottom inside position
    if (
        RX::throttle <= CHANNEL_LOW_MAX &&
        RX::yaw >= CHANNEL_HIGH_MIN &&
        RX::roll <= CHANNEL_LOW_MAX &&
        RX::pitch <= CHANNEL_LOW_MAX
    ) {
        RX::sticksAreAtBottomInside = true;
    }
}

void RX::onChannelChanged() {
    roll = crsf.getChannel(ROLL_CHANNEL);
    pitch = crsf.getChannel(PITCH_CHANNEL);
    throttle = crsf.getChannel(THROTTLE_CHANNEL);
    yaw = crsf.getChannel(YAW_CHANNEL);
    isArmed = crsf.getChannel(ARM_CHANNEL) >= CHANNEL_HIGH_MIN;
    ledSwitchIsOn = crsf.getChannel(LED_CONTROL_CHANNEL) >= CHANNEL_HIGH_MIN;

    updateLedBrightnessState();
    updateOtaIsActiveState();
    updateSticksAreAtTopCenter();
    updateSticksAreAtBottomCenter();
}