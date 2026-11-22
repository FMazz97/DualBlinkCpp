/// @file dual_blink_cpp.h
/// @brief Framework-agnostic core of the DualBlinkCpp library: non-blocking GPIO blinker.
/// @details Reference example for the dual framework guide: it shows, on the smallest
///          possible library, the three-layer architecture (Arduino facade, agnostic
///          core, HAL macros) and the naming convention of the guide.
///          This class IS the public API, also used directly by ESP-IDF. It contains
///          all the logic of the library and calls only DUAL_BLINK_CPP_* macros,
///          never Arduino or ESP-IDF APIs. Timing is millisecond-based and fully
///          non-blocking: the caller must invoke update() periodically (e.g. from
///          loop() or a task).
///
/// @par HAL contract
///      The core knows nothing but the following macros. Every HAL header
///      (`hal/dual_blink_cpp_hal_<platform>.h`) must define all of them:
///
///      | Macro | Must expand to |
///      | :--- | :--- |
///      | `DUAL_BLINK_CPP_PIN_OUTPUT()` | configure the pin as an output |
///      | `DUAL_BLINK_CPP_PIN_WRITE(level)` | drive the pin, `level` being a `bool` |
///      | `DUAL_BLINK_CPP_MILLIS()` | return a free-running millisecond counter as `uint32_t` |
///
/// @par Naming contract
///      Method names follow the prefix scheme of the guide, and each method states
///      its promise in a `@note`: `get_*` never touches the hardware; `set_*` stores
///      state, has a `get_*` counterpart and reports success or failure only when
///      an external device can confirm what was pushed to it (a GPIO pin cannot, so
///      `set_level()` returns `void`). Names outside the scheme (`begin()`,
///      `update()`) are universal or device-specific and carry no prefix promise.
///
/// @author FMazz97 (https://github.com/FMazz97)
/// @see dual_blink_cpp.cpp, dual_blink_cpp_hal_arduino.h, dual_blink_cpp_hal_espidf.h,
///      Chapter 3 (Agnostic Core & HAL Architecture), Appendix C (Naming Convention)
///      of the Dual-Framework Library Guide:
///      https://github.com/FMazz97/dual-framework-library-guide
/// @copyright MIT License

#ifndef DUAL_BLINK_CPP_CORE_H
#define DUAL_BLINK_CPP_CORE_H

#include <stdint.h>

// -----------------------------------------------------------------------------
// HAL selection: resolved at compile time. ARDUINO takes precedence over
// ESP_PLATFORM: Arduino-ESP32 defines both, and must use the Arduino HAL.
// -----------------------------------------------------------------------------
#if defined(ARDUINO)
#include "../hal/dual_blink_cpp_hal_arduino.h"
#elif defined(ESP_PLATFORM)
#include "../hal/dual_blink_cpp_hal_espidf.h"
#else
#error "DualBlinkCpp core: unsupported platform (expected ARDUINO or ESP_PLATFORM)"
#endif

/**
 * @brief Non-blocking blinker for a single GPIO pin (framework-agnostic core).
 * @details Toggles the configured pin every `interval_ms` milliseconds when
 *          update() is called. Elapsed time is computed as an unsigned difference,
 *          so the logic stays correct across millisecond counter rollover.
 * @see DualBlinkCpp for the Arduino facade (identical behavior, camelCase).
 */
class dual_blink_cpp {
   public:
    /// @brief Constructs a blinker bound to a pin.
    /// @param pin GPIO number to drive (Arduino pin number or ESP-IDF gpio_num_t value).
    /// @param interval_ms Time between two toggles, in milliseconds (default 500).
    /// @note No hardware access: only stores state. Call begin() before the first update().
    explicit dual_blink_cpp(int pin, uint32_t interval_ms = 500);

    /// @brief Configures the pin as output, drives it LOW and starts the interval timer.
    /// @note Touches the hardware. Name outside the prefix scheme (universal, like in Arduino).
    void begin();

    /// @brief Toggles the pin if at least `interval_ms` has elapsed since the last toggle.
    /// @details Must be called periodically, as often as the desired timing accuracy requires.
    /// @note Never blocks: returns immediately when the interval has not elapsed yet.
    ///       Touches the hardware only when it toggles.
    void update();

    /// @brief Forces the pin to a given level and records it as the current state.
    /// @param level `true` for HIGH, `false` for LOW.
    /// @note Pushes the state to a GPIO pin, which cannot answer: there is nothing
    ///       to confirm, so it returns `void` (see the `set_*` contract).
    ///       Does not reset the interval timer: the next automatic toggle keeps its
    ///       original schedule. Counterpart: `get_level()`.
    void set_level(bool level);

    /// @brief Returns the last level written to the pin.
    /// @return `true` if HIGH, `false` if LOW.
    /// @note No hardware access: returns the cached value, the pin is never read back.
    ///       Always `const`, cannot fail or block.
    bool get_level() const;

    /// @brief Sets the time between two toggles.
    /// @param interval_ms New interval in milliseconds. Takes effect from the next `update()` call.
    /// @note No hardware access: only stores state. Counterpart: `get_interval_ms()`.
    void set_interval_ms(uint32_t interval_ms);

    /// @brief Returns the current time between two toggles.
    /// @return Interval in milliseconds.
    /// @note No hardware access. Always `const`, cannot fail or block.
    uint32_t get_interval_ms() const;

   private:
    int _pin;                   ///< GPIO number driven by this instance (read by the HAL macros).
    uint32_t _interval_ms;      ///< Time between two toggles, in milliseconds.
    uint32_t _last_toggle_ms;   ///< Timestamp of the last toggle, in milliseconds.
    bool _level;                ///< Last level written to the pin.
};

#endif /* DUAL_BLINK_CPP_CORE_H */
