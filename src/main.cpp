#include <CrsfSerial.h>
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <FastLED.h>
#include <ArduinoOTA.h>

EspSoftwareSerial::UART rxSerial;

#define RX_SERIAL_RX_PIN D7
#define RX_SERIAL_TX_PIN D8
#define RX_BAUD 57600

#define CHANNEL_HIGH_MIN 1999
#define CHANNEL_LOW_MAX 1001
#define SWITCH_HIGH 2000
#define SWITCH_LOW 1000

#define NUM_LEDS 9
#define LED_PIN D4

// #define WIFI_SSID "ssid"
// #define WIFI_PASSWORD "password"

// 300 * 200ms = 1 minute
#define WIFI_MAX_CONNECT_ATTEMPTS 300
#define WIFI_HOTSPOT_SSID "CRSF Visualizer"
#define WIFI_HOTSPOT_PASSWORD "password"

CrsfSerial crsf(rxSerial, RX_BAUD);
CRGBArray<NUM_LEDS> leds;

enum LedSwitchState {
  LED_SWITCH_STATE_LOW,
  LED_SWITCH_STATE_CENTER,
  LED_SWITCH_STATE_HIGH,
};

bool isArmed = false;
int throttle = CHANNEL_LOW_MAX;
int roll = CHANNEL_LOW_MAX;
int pitch = CHANNEL_LOW_MAX;
int yaw = CHANNEL_LOW_MAX;
LedSwitchState ledSwitch = LED_SWITCH_STATE_LOW;
bool otaIsActive = false;


bool wifiEnabled = false;
int wifiConnectAttempts = 0;
bool wifiConnected = false;
bool isInHotspotMode = false;

IPAddress wifi_hotspot_local_IP(10, 0, 0, 1);
IPAddress wifi_hotspot_gateway(10, 0, 0, 2);
IPAddress wifi_hotspot_subnet(255, 0, 0, 0);

void onChannelChanged()
{
  roll = crsf.getChannel(1);
  pitch = crsf.getChannel(2);
  throttle = crsf.getChannel(3);
  yaw = crsf.getChannel(4);
  isArmed = crsf.getChannel(5) == SWITCH_HIGH;
  
  int ledSwitchValue = crsf.getChannel(7);
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

inline void setupCrsf() {
  rxSerial.begin(RX_BAUD, EspSoftwareSerial::SWSERIAL_8N1, RX_SERIAL_RX_PIN, RX_SERIAL_TX_PIN);

  crsf.onPacketChannels = &onChannelChanged;
}

inline void setupFastLED() {
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS)
    .setCorrection(TypicalLEDStrip);
}

inline void setupArduinoOTA() {
  ArduinoOTA.setPort(8266);
  ArduinoOTA.setHostname("crsf-visualizer");

  ArduinoOTA.onStart([]() {
    Serial.println("OTA Upload started");
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("\nOTA Upload finished");

    // reboot
    ESP.restart();
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

  ArduinoOTA.begin();
}

void setup()
{
  Serial.begin(115200);
  Serial.println("setup()");

  setupArduinoOTA();
  setupCrsf();
  setupFastLED();

  Serial.println("setup() done");
}

static uint8_t hue = 0;
static uint8_t brightness = 0;
static bool isIncreasing = true;
inline void ledIdleAnimation() {
  // slow breathing effect with slow color change

  EVERY_N_MILLISECONDS(200) {
    hue++;
  }

  EVERY_N_MILLISECONDS(10) {
    if (isIncreasing) {
      if (brightness < 255) {
        brightness++;
      } else {
        isIncreasing = false;
      }
    } else {
      if (brightness > 0) {
        brightness--;
      } else {
        isIncreasing = true;
      }
    }

    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CHSV(hue, 255, brightness);
    }

    FastLED.show();
  }
}

inline void ledIsArmedAnimation() {
  // fast blinking red
  EVERY_N_MILLISECONDS(100) {
    if (brightness == 0) {
      brightness = 255;
    } else {
      brightness = 0;
    }

    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CHSV(0, 255, brightness);
    }

    FastLED.show();
  }

}

inline void ledOtaWifiWaitForWifiAnimation() {
  // quick blue blinking
  EVERY_N_MILLISECONDS(100) {
    if (brightness == 0) {
      brightness = 255;
    } else {
      brightness = 0;
    }

    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CHSV(170, 255, brightness);
    }

    FastLED.show();
  }
}

inline void ledOtaWifiConnectedAnimation() {
  // blue
  int hue = 170;
  int saturation = 255;

  if (isInHotspotMode) {
    // make led pink
    hue = 200;
  }

  // quick breathing
  EVERY_N_MILLISECONDS(2) {
    if (isIncreasing) {
      if (brightness < 255) {
        brightness++;
      } else {
        isIncreasing = false;
      }
    } else {
      if (brightness > 0) {
        brightness--;
      } else {
        isIncreasing = true;
      }
    }

    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CHSV(hue, saturation, brightness);
    }

    FastLED.show();
  }
}

inline void ledLoop() {
  if (otaIsActive && wifiEnabled) {
    if (wifiConnected) {
      ledOtaWifiConnectedAnimation();
    } else {
      ledOtaWifiWaitForWifiAnimation();
    }
    return;
  }

  if (isArmed) {
    ledIsArmedAnimation();
    return;
  }

  ledIdleAnimation();
}

inline void otaLoop() {
  if (!otaIsActive) {
    if (wifiEnabled) {
      Serial.println("OTA disabled, disabling wifi");
      WiFi.forceSleepBegin();
      wifiEnabled = false;
      wifiConnected = false;
      wifiConnectAttempts = 0;
      isInHotspotMode = false;
    }
    return;
  }

  if (!wifiEnabled) {
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

  if (!wifiConnected && (wifiConnectAttempts > WIFI_MAX_CONNECT_ATTEMPTS)) {
    Serial.println("starting Hotspot");
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(wifi_hotspot_local_IP, wifi_hotspot_gateway, wifi_hotspot_subnet);
    WiFi.softAP(WIFI_HOTSPOT_SSID, WIFI_HOTSPOT_PASSWORD);
    isInHotspotMode = true;
    wifiConnected = true;
    
    Serial.print("IP address: ");
    if (wifiConnectAttempts > WIFI_MAX_CONNECT_ATTEMPTS) {
      Serial.println(WiFi.softAPIP());
    } else {
      Serial.println(WiFi.localIP());
    }

    return;
  }

  EVERY_N_MILLISECONDS(200) {
    if (WiFi.isConnected() != WL_CONNECTED) {
      wifiConnectAttempts++;
      return;
    }
    wifiConnected = true;
  }

  if (wifiConnected) {
    ArduinoOTA.handle();
  }
}

void loop()
{
    crsf.loop();
    ledLoop();
    otaLoop();
}