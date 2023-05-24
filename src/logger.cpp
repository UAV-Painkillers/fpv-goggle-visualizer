#include "logger.hpp"

void Logger::begin() {
    #ifdef USE_SOFTWARE_SERIAL_FOR_LOGGING
        loggingSoftSerial.begin(LOGGING_BAUD, EspSoftwareSerial::SWSERIAL_8N1, LOGGING_SERIAL_RX_PIN, LOGGING_SERIAL_TX_PIN);
    #else
        Serial.begin(LOGGING_BAUD);
    #endif
};

void Logger::log(String msg) {
    #ifdef USE_SOFTWARE_SERIAL_FOR_LOGGING
        loggingSoftSerial.print(msg);
    #else
        Serial.print(msg);
    #endif
};

void Logger::logLn(String msg) {
    #ifdef USE_SOFTWARE_SERIAL_FOR_LOGGING
        loggingSoftSerial.println(msg);
    #else
        Serial.println(msg);
    #endif
};
