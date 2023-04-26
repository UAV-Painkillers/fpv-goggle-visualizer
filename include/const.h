#pragma once

// RX Communication -------------------------------
#define RX_SERIAL_RX_PIN D7
#define RX_SERIAL_TX_PIN D8
// software serial on esp8266 is limited to 57600 baud for stable communication
#define RX_BAUD 57600

// Channel Values --------------------------------
#define CHANNEL_HIGH_MIN 1999
#define CHANNEL_LOW_MAX 1001
#define SWITCH_HIGH 2000
#define SWITCH_LOW 1000

// Channel mapping -------------------------------
#define ARM_CHANNEL 5
#define LED_CONTROL_CHANNEL 7
#define THROTTLE_CHANNEL 3
#define ROLL_CHANNEL 1
#define PITCH_CHANNEL 2
#define YAW_CHANNEL 4

// LED -------------------------------------------
#define NUM_LEDS 9
#define LED_PIN D4
// https://github.com/FastLED/FastLED/wiki/Pixel-reference#setting-hsv-colors-
#define LED_IDLE_BREATHING_COLOR_HUE 96

// WiFi OTA --------------------------------------

// uncomment and set these if you want to connect to your home wifi
// #define WIFI_SSID "ssid"
// #define WIFI_PASSWORD "password"

// how many times shall we try to connect to the wifi before we create a hotspot? (each attempt is 200ms)
#define WIFI_MAX_CONNECT_ATTEMPTS 300 // 300 * 200ms = 1 minute
#define WIFI_HOTSPOT_SSID "CRSF Visualizer"
#define WIFI_HOTSPOT_PASSWORD "jappy is awesome"
// wifi will automatically shut down after 3 minutes of waiting for OTA connections
#define WIFI_HOTSPOT_TIMEOUT 180000 // 3 minutes in ms (3 * 60 * 1000)