#pragma once

#include <Arduino.h>

class HeadTracker {
    public:
        void begin();
        void tick();
        void setOrigin();
        void enableFeedback();
        void disableFeedback();

    private:
        uint8_t fifoBuffer[64]; // FIFO storage buffer
        bool feedbackEnabled = false;

        // orientation/motion vars
        double currentDegree[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
        double originDegree[3];   // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

        void giveFeedback(uint16_t feedbackDuration, uint16_t pauseDuration, uint16_t feedbackCount, bool force);
        void updateOrientation();
};
