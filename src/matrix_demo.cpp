/*
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

#define MATRIX_PIN 8
#define MATRIX_WIDTH 5
#define MATRIX_HEIGHT 5

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_RIGHT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE,
  NEO_GRB            + NEO_KHZ800);

void setup() {
  matrix.begin();
  matrix.setBrightness(40);
  
  matrix.fillScreen(0);
  matrix.drawTriangle(0, 0, 2, 2, 4, 0, matrix.Color(255, 0, 0));
  matrix.show();
}

void loop() {
}
*/