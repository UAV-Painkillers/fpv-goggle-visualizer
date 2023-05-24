#include "ota.state.hpp"
#include "../../logger.hpp"

static const IPAddress wifi_hotspot_local_IP(10, 0, 0, 1);
static const IPAddress wifi_hotspot_gateway(10, 0, 0, 1);
static const IPAddress wifi_hotspot_subnet(255, 255, 255, 0);

void OTAState::waitForWifiConnection() {
    if (WiFi.isConnected() == WL_CONNECTED) {
        Logger::getInstance().logLn("wifi connected");
        Logger::getInstance().log("IP address: ");
        Logger::getInstance().logLn(WiFi.localIP().toString());
        wifiConnected = true;
        _ledController->setAnimation(&_wifiReadyAnimation);

        enableOTALibrary();
        return;
    }

    wifiConnectAttempts++;

    if (wifiConnectAttempts > WIFI_MAX_CONNECT_ATTEMPTS) {
        Logger::getInstance().logLn("wifi connection failed, starting hotspot");
        startHotspot();
        return;
    }

    _scheduler->timeout(200, [this]() {
        waitForWifiConnection();
    });
}

void OTAState::enableWifi() {
    Logger::getInstance().logLn("OTA enabled, enabling wifi");
    wifiEnabled = true;
    #ifdef WIFI_SSID
        Logger::getInstance().logLn("connecting to wifi");
        this->_ledController->setAnimation(&_waitForWifiAnimation);
        WiFi.mode(WIFI_STA);
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        wifiConnected = false;
        waitForWifiConnection();
    #else
        // trigger start of AP directly
        Logger::getInstance().logLn("skip wifi connection as no credentials are set and start hotspot directly");
        startHotspot();
    #endif
}

void OTAState::startHotspot() {
    Logger::getInstance().logLn("starting Hotspot");
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(wifi_hotspot_local_IP, wifi_hotspot_gateway,wifi_hotspot_subnet);
    WiFi.softAP(WIFI_HOTSPOT_SSID, WIFI_HOTSPOT_PASSWORD);
    isInHotspotMode = true;
    wifiConnected = true;

    Logger::getInstance().log("IP address: ");
    Logger::getInstance().logLn(WiFi.softAPIP().toString());

    _ledController->setAnimation(&_hotspotReadyAnimation);
    enableOTALibrary();
}

void OTAState::enableOTALibrary() {
    if (otaLibraryEnabled) {
        return;
    }

    Logger::getInstance().logLn("enabling OTA library");

    ArduinoOTA.setPort(8266);
    ArduinoOTA.setHostname("crsf-visualizer");

    ArduinoOTA.onStart([this]() {
        Logger::getInstance().logLn("OTA Upload started");
        this->_ledController->setAnimation(&_otaUploadAnimation);
    });

    ArduinoOTA.onEnd([]() {
        Logger::getInstance().logLn("\nOTA Upload finished");

        // reboot
        ESP.restart();
    });

    ArduinoOTA.onProgress([this](unsigned int progress, unsigned int total) {
        uint8_t progressInPercent = (progress / (total / 100));
        otaUploadInProgress = true;
        this->_otaUploadAnimation.setProgress(progressInPercent);
        Serial.println("OTA Progress: " + progressInPercent);
    });

    ArduinoOTA.onError([this](ota_error_t error) {
        otaUploadInProgress = false;
        if (error == OTA_AUTH_ERROR) Logger::getInstance().logLn("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) Logger::getInstance().logLn("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Logger::getInstance().logLn("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Logger::getInstance().logLn("Receive Failed");
        else if (error == OTA_END_ERROR) Logger::getInstance().logLn("End Failed");
    });

    ArduinoOTA.begin();
    otaLibraryEnabled = true;

    _otaTask = this->_scheduler->every(200, [this]() {
        ArduinoOTA.handle();

        bool throttleIsLow = RX::throttle <= CHANNEL_LOW_MAX;
        bool yawIsRight = RX::yaw >= CHANNEL_HIGH_MIN;
        bool pitchIsLow = RX::pitch <= CHANNEL_LOW_MAX;
        bool rollIsLeft = RX::roll <= CHANNEL_LOW_MAX;

        if (throttleIsLow && yawIsRight && pitchIsLow && rollIsLeft) {
            Logger::getInstance().logLn("OTA disabled by RC");
            ESP.restart();
        }
    });

    this->_scheduler->timeout(WIFI_HOTSPOT_TIMEOUT, [this]() {
        ESP.restart();
    });

    Logger::getInstance().logLn("OTA library enabled");
}

void OTAState::enter() {
    Logger::getInstance().logLn("OTAState::enter() - starting WiFi");
    this->_otaUploadAnimation.setProgress(0);
    enableWifi();

    Logger::getInstance().logLn("OTAState::enter() - done");
}

void OTAState::leave() {
    Logger::getInstance().logLn("OTAState::leave() - terminating WiFi");
    terminateWifi();

    Logger::getInstance().logLn("OTAState::leave() - terminating OTA library");
    terminateOTALibrary();

    _ledController->clear();

    Logger::getInstance().logLn("OTAState::leave() - done");
}

void OTAState::terminateOTALibrary() {
    otaUploadInProgress = false;

    if (otaLibraryEnabled) {
        // no way to turn off ArduinoOTA, so we need to reboot
        Logger::getInstance().logLn("OTA disabled, rebooting");
        ESP.restart();
    }
}

void OTAState::terminateWifi() {
    wifiEnabled = false;
    wifiConnected = false;
    wifiConnectAttempts = 0;
    isInHotspotMode = false;

    if (wifiEnabled) {
        Logger::getInstance().logLn("disabling wifi");
        #ifdef ESP32
            WiFi.setSleep(true);
        #else
            WiFi.forceSleepBegin();
        #endif
    }
}