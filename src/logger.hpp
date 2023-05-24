#pragma once

#include "const.h"

#include <SoftwareSerial.h>
#include <Arduino.h>

#ifdef USE_SOFTWARE_SERIAL_FOR_LOGGING
    static EspSoftwareSerial::UART loggingSoftSerial;
#endif

class Logger  {
    public:
        static Logger& getInstance() {
          static Logger instance;
          return instance;
        };

    private:
        Logger() {};
        Logger(Logger const&);
        void operator=(Logger const&);

    public:
        void begin();
        void log(String msg);
        void logLn(String msg);
};
