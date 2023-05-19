#pragma once

#include <bluefairy.h>
#include <ArduinoOTA.h>
#ifdef ESP32
    #include <WiFi.h>
#else
    #include <ESP8266WiFi.h>
#endif

#include "../basic.state.hpp"
#include "const.h"

class OTAState: public BasicState {
    public:
        OTAState(
            AppStateMachine *stateMachine,
            bluefairy::Scheduler *scheduler,
            LedController *ledController
        ): BasicState(
            stateMachine,
            scheduler,
            ledController
        ) {};

        void enter();
        void leave();
    
    private:
        bool wifiEnabled = false;
        bool wifiConnected = false;
        int wifiConnectAttempts = 0;
        bool isInHotspotMode = false;
        bool otaUploadInProgress = false;
        bool otaLibraryEnabled = false;
        bluefairy::TaskNode *_otaTask;

        void enableWifi();
        void waitForWifiConnection();
        void startHotspot();
        void enableOTALibrary();
        void terminateWifi();
        void terminateOTALibrary();
};
