#include <CrsfSerial.h>
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <FastLED.h>
#include <ArduinoOTA.h>
#include "const.h"

EspSoftwareSerial::UART rxSerial;

CrsfSerial crsf(rxSerial, RX_BAUD);
CRGBArray<NUM_LEDS> leds;

enum LedSwitchState {
  LED_SWITCH_STATE_LOW,
  LED_SWITCH_STATE_CENTER,
  LED_SWITCH_STATE_HIGH,
};

static bool isArmed = false;
static int throttle = CHANNEL_LOW_MAX;
static int roll = CHANNEL_LOW_MAX;
static int pitch = CHANNEL_LOW_MAX;
static int yaw = CHANNEL_LOW_MAX;
static LedSwitchState ledSwitch = LED_SWITCH_STATE_LOW;
static bool otaIsActive = false;


static bool wifiEnabled = false;
static int wifiConnectAttempts = 0;
static bool wifiConnected = false;
static bool isInHotspotMode = false;
static unsigned long wifiConnectionStartTime = 0;
static bool otaUploadInProgress = false;
static bool otaLibraryEnabled = false;

IPAddress wifi_hotspot_local_IP(10, 0, 0, 1);
IPAddress wifi_hotspot_gateway(10, 0, 0, 2);
IPAddress wifi_hotspot_subnet(255, 0, 0, 0);

void onChannelChanged()
{
  roll = crsf.getChannel(ROLL_CHANNEL);
  pitch = crsf.getChannel(PITCH_CHANNEL);
  throttle = crsf.getChannel(THROTTLE_CHANNEL);
  yaw = crsf.getChannel(YAW_CHANNEL);
  isArmed = crsf.getChannel(ARM_CHANNEL) == SWITCH_HIGH;
  int ledSwitchValue = crsf.getChannel(LED_CONTROL_CHANNEL);

  if (ledSwitchValue == SWITCH_HIGH) {
    ledSwitch = LED_SWITCH_STATE_HIGH;
  } else if (ledSwitchValue == SWITCH_LOW) {
    ledSwitch = LED_SWITCH_STATE_LOW;
  } else {
    ledSwitch = LED_SWITCH_STATE_CENTER;
  }

  // enable ota
  // disarmed, led on and sticks to the top outside position
  if (
    !isArmed &&
    ledSwitch == LED_SWITCH_STATE_HIGH &&
    throttle >= CHANNEL_HIGH_MIN &&
    yaw <= CHANNEL_LOW_MAX &&
    roll >= CHANNEL_HIGH_MIN &&
    pitch >= CHANNEL_HIGH_MIN
  ) {
    if (!otaIsActive) {
      Serial.println("OTA activated");
    }
    otaIsActive = true;
  }

  // disable ota
  // disarmed, led on and sticks to the bottom inside position
  if (
    !isArmed &&
    ledSwitch == LED_SWITCH_STATE_HIGH &&
    throttle <= CHANNEL_LOW_MAX &&
    yaw >= CHANNEL_HIGH_MIN &&
    roll <= CHANNEL_LOW_MAX &&
    pitch <= CHANNEL_LOW_MAX
  ) {
    if (otaIsActive) {
      Serial.println("OTA deactivated");
    }
    otaIsActive = false;
  }
}

inline void setupRX() {
  Serial.print("Setup RX...");
  rxSerial.begin(RX_BAUD, EspSoftwareSerial::SWSERIAL_8N1, RX_SERIAL_RX_PIN, RX_SERIAL_TX_PIN);

  crsf.onPacketChannels = &onChannelChanged;
  Serial.println("done");
}

inline void setupLED() {
  Serial.print("Setup LED...");
  FastLED.addLeds<WS2811, LED_PIN, GRB>(leds, NUM_LEDS)
    .setCorrection(TypicalLEDStrip);

  // all leds Black
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
    FastLED.show();
  }

  // move blue led from left to right and back
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Blue;
    FastLED.show();
    leds[i] = CRGB::Black;
    delay(50);
  }
  for (int i = NUM_LEDS - 1; i >= 0; i--) {
    leds[i] = CRGB::Blue;
    FastLED.show();
    leds[i] = CRGB::Black;
    delay(50);
  }

  Serial.println("done");
}

inline void setupArduinoOTA() {
  Serial.print("Setup OTA...");

  ArduinoOTA.setPort(8266);
  ArduinoOTA.setHostname("crsf-visualizer");

  ArduinoOTA.onStart([]() {
    Serial.println("OTA Upload started");
    otaUploadInProgress = true;
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("\nOTA Upload finished");
    otaUploadInProgress = false;

    // reboot
    ESP.restart();
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error) {
    otaUploadInProgress = false;
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

  Serial.println("done");
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Welcome to CRSF Visualizer");

  setupArduinoOTA();
  setupRX();
  setupLED();

  Serial.println("setup() done, starting loop()");
}

static uint8_t rainbow_hue = 0;
inline void ledRainbowSlowKeyframe() {
  rainbow_hue++;

  for (int i = 0; i < NUM_LEDS; i++) {
    int ledHue = rainbow_hue + (i * 20);
    if (ledHue > 255) {
      ledHue -= 255;
    }
    leds[i] = CHSV(ledHue, 255, 255);
  }

  FastLED.show();
}
inline void ledRainbowSlowAnimation() {
  EVERY_N_MILLISECONDS(15) {
    ledRainbowSlowKeyframe();
  }
}

static uint8_t breathing_is_increasing = true;
static uint8_t breathing_brightness = 0;
inline void ledBreathingKeyframe(uint8_t hue) {
  if (breathing_is_increasing) {
    breathing_brightness++;
    if (breathing_brightness >= 255) {
      breathing_is_increasing = false;
    }
  } else {
    breathing_brightness--;
    if (breathing_brightness <= 0) {
      breathing_is_increasing = true;
    }
  }

  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(hue, 255, breathing_brightness);
  }

  FastLED.show();
}

inline void ledBreathingAnimation(uint8_t hue) {
  EVERY_N_MILLISECONDS(10) {
    ledBreathingKeyframe(hue);
  };
}

static uint8_t blinking_is_on = false;
inline void ledBlinkingAnimationFrame(uint8_t hue) {
  if (blinking_is_on) {
    blinking_is_on = false;
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CHSV(hue, 255, 0);
    }
  } else {
    blinking_is_on = true;
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CHSV(hue, 255, 255);
    }
  }

  FastLED.show();
}
inline void ledBlinkingAnimation(uint8_t hue, int blinkInterval) {
  EVERY_N_MILLISECONDS(blinkInterval) {
    ledBreathingKeyframe(hue);
  };
}

static int ledIsArmedBlinkInterval = 0;
static int ledIsArmedMillisCounter = 0;
inline void ledIsArmedKeyframe() {
  int cappedThrottle = throttle;
  if (cappedThrottle > 2000) {
    cappedThrottle = 2000;
  }
  if (cappedThrottle < 1000) {
    cappedThrottle = 1000;
  }

  int ledIsArmedBlinkInterval = map(cappedThrottle, 1000, 2000, 200, 25);

  ledIsArmedMillisCounter++;

  if (ledIsArmedMillisCounter > ledIsArmedBlinkInterval) {
    ledIsArmedMillisCounter = 0;
    // 0 = red https://github.com/FastLED/FastLED/wiki/Pixel-reference#setting-hsv-colors-
    ledBlinkingAnimationFrame(0);
  }
}
inline void ledIsArmedAnimation() {
  // blink red, speed is defined by throttle position (1000 - 2000) which is mapped to 25 - 1000, more throttle = faster blinking
  EVERY_N_MILLISECONDS(1) {
    ledIsArmedKeyframe();
  }
}

inline void ledMaximumAnimation() {
  if (!isArmed) {
    ledBreathingAnimation(LED_IDLE_BREATHING_COLOR_HUE);
  } else {
    ledIsArmedAnimation();
  }
}

inline void ledMinimumAnimation() {
  ledRainbowSlowAnimation();
}

inline void ledOtaWifiWaitForWifiAnimation() {
  // quick blue blinking
  uint8_t blue_hue = 170;
  uint8_t blinkInterval = 100;
  ledBlinkingAnimation(blue_hue, blinkInterval);
}

inline void ledOtaWifiConnectedAnimation() {
  // blue
  int hue = 170;

  if (isInHotspotMode) {
    // make led pink
    hue = 200;
  }

  ledBreathingAnimation(hue);
}

static bool ledWasOn = true;
inline void ledLoop() {
  if (otaIsActive && wifiEnabled) {
    if (wifiConnected) {
      ledOtaWifiConnectedAnimation();
    } else {
      ledOtaWifiWaitForWifiAnimation();
    }
    return;
  }

  if (ledSwitch == LED_SWITCH_STATE_LOW) {
    if (ledWasOn) { 
      // led off
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB::Black;
      }
      FastLED.show();
      ledWasOn = false;
    }
    return;
  } else {
    ledWasOn = true;
  }

  if (ledSwitch == LED_SWITCH_STATE_HIGH) {
    ledMaximumAnimation();
    return;
  }

  ledMinimumAnimation();
}

inline void otaEnableWifi() {
  Serial.println("OTA enabled, enabling wifi");
  wifiEnabled = true;
  #ifdef WIFI_SSID
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    wifiConnected = false;
  #else
    // trigger start of AP directly
    Serial.println("skip wifi connection as no credentials are set");
    wifiConnectAttempts = WIFI_MAX_CONNECT_ATTEMPTS;
  #endif
}

inline void otaStartHotspot() {
  Serial.println("starting Hotspot");
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(wifi_hotspot_local_IP, wifi_hotspot_gateway, wifi_hotspot_subnet);
  WiFi.softAP(WIFI_HOTSPOT_SSID, WIFI_HOTSPOT_PASSWORD);
  isInHotspotMode = true;
  wifiConnected = true;
  wifiConnectionStartTime = millis();
  
  Serial.print("IP address: ");
  if (wifiConnectAttempts > WIFI_MAX_CONNECT_ATTEMPTS) {
    Serial.println(WiFi.softAPIP());
  } else {
    Serial.println(WiFi.localIP());
  }
}

inline void otaCheckWifiConnection() {
  if (WiFi.isConnected() != WL_CONNECTED) {
    wifiConnectAttempts++;
    return;
  }
  wifiConnected = true;
}

inline void otaTerminate() {
  wifiEnabled = false;
  wifiConnected = false;
  wifiConnectAttempts = 0;
  isInHotspotMode = false;
  wifiConnectionStartTime = 0;
  otaUploadInProgress = false;

  if (wifiEnabled) {
    Serial.println("OTA disabled, disabling wifi");
    WiFi.forceSleepBegin();
  }

  if (otaLibraryEnabled) {
    // no way to turn off ArduinoOTA, so we need to reboot
    Serial.println("OTA disabled, rebooting");
    ESP.restart();
  }
}

inline void otaLoop() {
  if (!otaIsActive) {
    otaTerminate();
    return;
  }

  if (!wifiEnabled) {
    otaEnableWifi();
  }

  if (!wifiConnected && (wifiConnectAttempts > WIFI_MAX_CONNECT_ATTEMPTS)) {
    otaStartHotspot();

    return;
  }

  bool timeoutReached = (millis() - wifiConnectionStartTime) > WIFI_HOTSPOT_TIMEOUT;
  if (!otaUploadInProgress && timeoutReached) {
    otaIsActive = false;
    return;
  }

  EVERY_N_MILLISECONDS(200) {
    otaCheckWifiConnection();
  };

  if (wifiConnected) {
    if (!otaLibraryEnabled) {
      ArduinoOTA.begin();
      otaLibraryEnabled = true;
    }

    EVERY_N_MILLISECONDS(200) {
      ArduinoOTA.handle();
    };
  }
}

void loop()
{
    crsf.loop();
    ledLoop();
    otaLoop();
}