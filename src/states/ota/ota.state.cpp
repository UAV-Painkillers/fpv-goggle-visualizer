#include "ota.state.hpp"
#include "../../logger.hpp"

static const IPAddress wifi_hotspot_local_IP(10, 0, 0, 1);
static const IPAddress wifi_hotspot_gateway(10, 0, 0, 2);
static const IPAddress wifi_hotspot_subnet(255, 0, 0, 0);

void OTAState::waitForWifiConnection() {
    if (WiFi.isConnected() != WL_CONNECTED) {
        wifiConnectAttempts++;
    }

    if (wifiConnectAttempts > WIFI_MAX_CONNECT_ATTEMPTS) {
        Logger::logLn("wifi connection failed, starting hotspot");
        startHotspot();
        return;
    }

    _scheduler->timeout(200, [this]() {
        waitForWifiConnection();
    });
}

void OTAState::enableWifi() {
    Logger::logLn("OTA enabled, enabling wifi");
    wifiEnabled = true;
    #ifdef WIFI_SSID
        Logger::logLn("connecting to wifi");
        WiFi.mode(WIFI_STA);
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        wifiConnected = false;
        waitForWifiConnection();
    #else
        // trigger start of AP directly
        Logger::logLn("skip wifi connection as no credentials are set and start hotspot directly");
        startHotspot();
    #endif
}

void OTAState::startHotspot() {
    Logger::logLn("starting Hotspot");
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(wifi_hotspot_local_IP, wifi_hotspot_gateway, wifi_hotspot_subnet);
    WiFi.softAP(WIFI_HOTSPOT_SSID, WIFI_HOTSPOT_PASSWORD);
    isInHotspotMode = true;
    wifiConnected = true;

    Logger::log("IP address: ");
    if (wifiConnectAttempts > WIFI_MAX_CONNECT_ATTEMPTS) {
        Logger::logLn(WiFi.softAPIP().toString());
    } else {
        Logger::logLn(WiFi.localIP().toString());
    }
}

void OTAState::enableOTALibrary() {
    if (otaLibraryEnabled) {
        return;
    }

    Logger::logLn("enabling OTA library");

    ArduinoOTA.setPort(8266);
    ArduinoOTA.setHostname("crsf-visualizer");

    ArduinoOTA.onStart([]() {
        Logger::logLn("OTA Upload started");
    });

    ArduinoOTA.onEnd([]() {
        Logger::logLn("\nOTA Upload finished");

        // reboot
        ESP.restart();
    });

    ArduinoOTA.onProgress([this](unsigned int progress, unsigned int total) {
        uint8_t progressInPercent = (progress / (total / 100));
        otaUploadInProgress = true;
        // TODO: why is it 100% green all the time??
        // otaUploadProgressAnimation.setProgress(progressInPercent);
    });

    ArduinoOTA.onError([this](ota_error_t error) {
        otaUploadInProgress = false;
        if (error == OTA_AUTH_ERROR) Logger::logLn("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) Logger::logLn("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Logger::logLn("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Logger::logLn("Receive Failed");
        else if (error == OTA_END_ERROR) Logger::logLn("End Failed");
    });

    ArduinoOTA.begin();
    otaLibraryEnabled = true;

    _otaTask = this->_scheduler->every(200, [this]() {
        ArduinoOTA.handle();
    });

    Logger::logLn("OTA library enabled");
}

void OTAState::enter() {
    Logger::logLn("OTAState::enter() - starting WiFi");
    enableWifi();

    Logger::logLn("OTAState::enter() - starting OTA library");
    enableOTALibrary();

    Logger::logLn("OTAState::enter() - done");
}

void OTAState::leave() {
    Logger::logLn("OTAState::leave() - terminating WiFi");
    terminateWifi();

    Logger::logLn("OTAState::leave() - terminating OTA library");
    terminateOTALibrary();

    Logger::logLn("OTAState::leave() - done");
}

void OTAState::terminateOTALibrary() {
    otaUploadInProgress = false;

    if (otaLibraryEnabled) {
        // no way to turn off ArduinoOTA, so we need to reboot
        Logger::logLn("OTA disabled, rebooting");
        ESP.restart();
    }
}

void OTAState::terminateWifi() {
    wifiEnabled = false;
    wifiConnected = false;
    wifiConnectAttempts = 0;
    isInHotspotMode = false;

    if (wifiEnabled) {
        Logger::logLn("disabling wifi");
        #ifdef ESP32
            WiFi.setSleep(true);
        #else
            WiFi.forceSleepBegin();
        #endif
    }
}