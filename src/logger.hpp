#pragma once

#include "const.h"

#include <SoftwareSerial.h>
#include <Arduino.h>

#ifdef USE_SOFTWARE_SERIAL_FOR_LOGGING
    static EspSoftwareSerial::UART loggingSoftSerial;
#endif

class Logger {
    public:
        static void begin();
        static void log(String msg);
        static void logLn(String msg);
};
