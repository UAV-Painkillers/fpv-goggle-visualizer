#include "rx.hpp"
#include "logger.hpp"

bool RX::isArmed = false;
bool RX::ledSwitchIsOn = false;
int RX::throttle = CHANNEL_LOW_MAX;
int RX::roll = CHANNEL_LOW_MAX;
int RX::pitch = CHANNEL_LOW_MAX;
int RX::yaw = CHANNEL_LOW_MAX;
int RX::ledBrightness = 255;
bool RX::otaIsActive = false;

void RX::begin() {
    Logger::getInstance().logLn("Setup RX...");
    rxSerial.begin(RX_BAUD, EspSoftwareSerial::SWSERIAL_8N1, RX_SERIAL_RX_PIN, RX_SERIAL_TX_PIN);

    crsf.onPacketChannels = &RX::onChannelChanged;
    Logger::getInstance().logLn("done");   
}

void RX::loop() {
    crsf.loop();
}

void RX::onChannelChanged() {
    roll = crsf.getChannel(ROLL_CHANNEL);
    pitch = crsf.getChannel(PITCH_CHANNEL);
    throttle = crsf.getChannel(THROTTLE_CHANNEL);
    yaw = crsf.getChannel(YAW_CHANNEL);
    isArmed = crsf.getChannel(ARM_CHANNEL) >= CHANNEL_HIGH_MIN;
    ledSwitchIsOn = crsf.getChannel(LED_CONTROL_CHANNEL) >= CHANNEL_HIGH_MIN;

    int nextLedBrightness = 255;

    #ifdef LED_BRIGHTNESS
        nextLedBrightness = LED_BRIGHTNESS;
    #endif

    #ifdef LED_BRIGHTNESS_CHANNEL
        int ledBrightnessChannelValue = crsf.getChannel(LED_BRIGHTNESS_CHANNEL);
        if (LED_BRIGHTNESS_CHANNEL_INVERTED) {
            nextLedBrightness = map(ledBrightnessChannelValue, CHANNEL_LOW_MAX, CHANNEL_HIGH_MIN, 10, 255);
        } else {
            nextLedBrightness = map(ledBrightnessChannelValue, CHANNEL_LOW_MAX, CHANNEL_HIGH_MIN, 255, 10);
        }
    #endif

    if (nextLedBrightness != ledBrightness) {
        ledBrightness = nextLedBrightness;
    }

    // enable ota
    // disarmed, led on and sticks to the top outside position
    if (
        !isArmed &&
        throttle >= CHANNEL_HIGH_MIN &&
        yaw <= CHANNEL_LOW_MAX &&
        roll >= CHANNEL_HIGH_MIN &&
        pitch >= CHANNEL_HIGH_MIN
    ) {
        otaIsActive = true;
    }

    // disable ota
    // disarmed, led on and sticks to the bottom inside position
    if (
        !isArmed &&
        throttle <= CHANNEL_LOW_MAX &&
        yaw >= CHANNEL_HIGH_MIN &&
        roll <= CHANNEL_LOW_MAX &&
        pitch <= CHANNEL_LOW_MAX
    ) {
        otaIsActive = false;
    }
}