#pragma once

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <CrsfSerial.h>

#include "const.h"

static EspSoftwareSerial::UART rxSerial;
static CrsfSerial crsf(rxSerial, RX_BAUD);

class RX {
    public:
        static bool isArmed;
        static bool ledSwitchIsOn;
        static int throttle;
        static int roll;
        static int pitch;
        static int yaw;
        static int ledBrightness;
        static bool otaIsActive;
        static bool lampMode;
    
        static void begin();
        static void loop();
        static void onChannelChanged();
};
