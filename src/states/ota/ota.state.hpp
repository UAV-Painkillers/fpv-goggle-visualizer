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
#include "../../animations/progress-bar-animation.hpp"
#include "../../animations/blink-animation.hpp"
#include "../../rx.hpp"

class OTAState: public BasicState {
    public:
        OTAState(
            AppStateMachine *stateMachine,
            bluefairy::Scheduler *scheduler,
            LedController *ledController
        ):
            BasicState(
                stateMachine,
                scheduler,
                ledController
            ),
            _waitForWifiAnimation(CRGB::Blue, CRGB::Black, 250),
            _wifiReadyAnimation(CRGB::Blue, CRGB::Black, 500),
            _hotspotReadyAnimation(CRGB(255, 0, 255), CRGB::Black, 500),
            _otaUploadAnimation(CRGB::Green, CRGB::Black)
        {};

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
        BlinkAnimation _waitForWifiAnimation;
        BlinkAnimation _wifiReadyAnimation;
        BlinkAnimation _hotspotReadyAnimation;
        ProgressBarAnimation _otaUploadAnimation;


        void enableWifi();
        void waitForWifiConnection();
        void startHotspot();
        void enableOTALibrary();
        void terminateWifi();
        void terminateOTALibrary();
};
