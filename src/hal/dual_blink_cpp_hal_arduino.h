#ifndef DUAL_BLINK_CPP_HAL_ARDUINO_H
#define DUAL_BLINK_CPP_HAL_ARDUINO_H

#include <Arduino.h>

#define DUAL_BLINK_CPP_PIN_OUTPUT() pinMode(_pin, OUTPUT)
#define DUAL_BLINK_CPP_PIN_WRITE(level) digitalWrite(_pin, level)
#define DUAL_BLINK_CPP_MILLIS() (static_cast<uint32_t>(millis()))

#endif /* DUAL_BLINK_CPP_HAL_ARDUINO_H */