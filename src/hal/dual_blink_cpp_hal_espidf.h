/// @file dual_blink_cpp_hal_espidf.h
/// @brief ESP-IDF HAL for the DualBlinkCpp core: GPIO and timing primitives
///        using native ESP-IDF APIs.
/// @details ESP-IDF implementation of the HAL contract declared in dual_blink_cpp.h:
///          DUAL_BLINK_CPP_PIN_OUTPUT(), DUAL_BLINK_CPP_PIN_WRITE() and
///          DUAL_BLINK_CPP_MILLIS(), mapped to the IDF gpio driver and esp_timer.
///          The macros expand inside dual_blink_cpp member functions and read the
///          `_pin` member. No driver logic lives here — only hardware primitives.
/// @author FMazz97 (https://github.com/FMazz97)
/// @see dual_blink_cpp.h, dual_blink_cpp.cpp
/// @copyright MIT License

#ifndef DUAL_BLINK_CPP_HAL_ESPIDF_H
#define DUAL_BLINK_CPP_HAL_ESPIDF_H

#include "driver/gpio.h"
#include "esp_timer.h"

// -----------------------------------------------------------------------------
// Pin direction / output primitives.
// -----------------------------------------------------------------------------
/// @brief HAL contract: configures `_pin` as an output.
#define DUAL_BLINK_CPP_PIN_OUTPUT() gpio_set_direction(static_cast<gpio_num_t>(_pin), GPIO_MODE_OUTPUT)
/// @brief HAL contract: drives `_pin` HIGH (`true`) or LOW (`false`).
#define DUAL_BLINK_CPP_PIN_WRITE(level) gpio_set_level(static_cast<gpio_num_t>(_pin), level)

// -----------------------------------------------------------------------------
// Timing primitive: esp_timer_get_time() returns microseconds since boot as
// int64_t; dividing by 1000 and truncating to uint32_t gives a millisecond
// counter that wraps every ~49.7 days, same as Arduino's millis(). The core's
// unsigned-difference logic is rollover-safe.
// -----------------------------------------------------------------------------
/// @brief HAL contract: free-running millisecond counter as `uint32_t`.
#define DUAL_BLINK_CPP_MILLIS() (static_cast<uint32_t>(esp_timer_get_time() / 1000))

#endif /* DUAL_BLINK_CPP_HAL_ESPIDF_H */
