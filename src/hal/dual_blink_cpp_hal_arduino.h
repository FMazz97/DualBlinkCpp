/// @file dual_blink_cpp_hal_arduino.h
/// @brief Arduino HAL for the DualBlinkCpp core: GPIO and timing primitives.
/// @details Arduino implementation of the HAL contract declared in dual_blink_cpp.h:
///          DUAL_BLINK_CPP_PIN_OUTPUT(), DUAL_BLINK_CPP_PIN_WRITE() and
///          DUAL_BLINK_CPP_MILLIS(), mapped to pinMode(), digitalWrite() and millis().
///          The macros expand inside dual_blink_cpp member functions and read the
///          `_pin` member. No driver logic lives here — only hardware primitives.
/// @author FMazz97 (https://github.com/FMazz97)
/// @see dual_blink_cpp.h, dual_blink_cpp.cpp
/// @copyright MIT License

#ifndef DUAL_BLINK_CPP_HAL_ARDUINO_H
#define DUAL_BLINK_CPP_HAL_ARDUINO_H

#include <Arduino.h>

// -----------------------------------------------------------------------------
// Pin direction / output / timing primitives.
// -----------------------------------------------------------------------------
/// @brief HAL contract: configures `_pin` as an output.
#define DUAL_BLINK_CPP_PIN_OUTPUT() pinMode(_pin, OUTPUT)
/// @brief HAL contract: drives `_pin` HIGH or LOW.
#define DUAL_BLINK_CPP_PIN_WRITE(level) digitalWrite(_pin, level)
/// @brief HAL contract: free-running millisecond counter as `uint32_t`.
#define DUAL_BLINK_CPP_MILLIS() (static_cast<uint32_t>(millis()))

#endif /* DUAL_BLINK_CPP_HAL_ARDUINO_H */
