#pragma once

#include <FastLED.h>

// RX Communication -------------------------------
#define RX_SERIAL_RX_PIN 19
#define RX_SERIAL_TX_PIN 18
// software serial on esp8266 is limited to 57600 baud for stable communication
#define RX_BAUD 57600

// LOGGING ---------------------------------------
// #define USE_SOFTWARE_SERIAL_FOR_LOGGING
#define LOGGING_SERIAL_TX_PIN 2
#define LOGGING_SERIAL_RX_PIN 10
#define LOGGING_BAUD 57600

// Channel Values --------------------------------
#define CHANNEL_HIGH_MIN 1999
#define CHANNEL_LOW_MAX 1001

// Channel mapping -------------------------------
#define ARM_CHANNEL 5
#define LED_CONTROL_CHANNEL 10
#define THROTTLE_CHANNEL 3
#define ROLL_CHANNEL 1
#define PITCH_CHANNEL 2
#define YAW_CHANNEL 4

// LED -------------------------------------------
#define NUM_LEDS 28
#define LED_PIN 4
#define LED_VOLTS 5
#define LED_MAX_MILLIAMPS 500
#define LED_TYPE WS2812
#define LED_COLOR_ORDER GRB
// https://github.com/FastLED/FastLED/wiki/Pixel-reference#setting-hsv-colors-
#define BOOT_ANIMATION_DURATION_MS 3000
#define LED_BRIGHTNESS 255
// comment out if you want to use a fixed brightness value from LED_BRIGHTNESS (above)
#define LED_BRIGHTNESS_CHANNEL 9
#define LED_BRIGHTNESS_CHANNEL_INVERTED true
#define LED_FPS 120

// WiFi OTA --------------------------------------

// uncomment and set these if you want to connect to your home wifi
// #define WIFI_SSID "ssid"
// #define WIFI_PASSWORD "password"

// how many times shall we try to connect to the wifi before we create a hotspot? (each attempt is 200ms)
#define WIFI_MAX_CONNECT_ATTEMPTS 300 // 300 * 200ms = 1 minute
#define WIFI_HOTSPOT_SSID "CRSF Visualizer"
#define WIFI_HOTSPOT_PASSWORD "blinkyblink"
// wifi will automatically shut down after 3 minutes of waiting for OTA connections
#define WIFI_HOTSPOT_TIMEOUT 180000 // 3 minutes in ms (3 * 60 * 1000)