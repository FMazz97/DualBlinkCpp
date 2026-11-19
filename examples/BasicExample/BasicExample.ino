#include <Arduino.h>

#include <DualBlinkCpp.h>

// Test pin configuration (adjust according to your hardware setup)
#ifndef BLINK_PIN
#define BLINK_PIN 4
#endif

DualBlinkCpp led(BLINK_PIN, 500);

void setup() {
  led.begin();
}

void loop() {
  led.update();
}