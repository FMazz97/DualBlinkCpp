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
///      | `DUAL_BLINK_CPP_PIN_READ()` | return the level present on the pin (non-zero = HIGH) |
///      | `DUAL_BLINK_CPP_MILLIS()` | return a free-running millisecond counter as `uint32_t` |
///
/// @par Naming contract
///      Method names follow the prefix scheme of the guide, and each method states
///      its promise in a `@note`: `get_*` never touches the hardware; `set_*` stores
///      state, has a `get_*` counterpart and reports success or failure when an
///      external device can confirm what was pushed to it through the `write_*` and
///      `read_*` primitives (here the pin is read back, so `set_level()` returns a `bool`).
///      The private `read_level()` and `write_level()` are the primitives that actually move
///      the GPIO: the public methods are built on top of them. Names outside the scheme
///      (`begin()`, `update()`) are universal or device-specific and carry no prefix promise;
///      here they return a `bool` too, reporting the outcome of the `set_level()` they perform.
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
    /// @return `true` if the pin reads back LOW, `false` otherwise (see `set_level()`).
    /// @note Touches the hardware. Name outside the prefix scheme (universal, like in Arduino).
    bool begin();

    /// @brief Toggles the pin if at least `interval_ms` has elapsed since the last toggle.
    /// @details Must be called periodically, as often as the desired timing accuracy requires.
    /// @return `true` if nothing had to be done or the toggle was confirmed, `false` if the
    ///         pin did not read back the toggled level (see `set_level()`).
    /// @note Never blocks: returns immediately when the interval has not elapsed yet.
    ///       Touches the hardware only when it toggles. After a failed toggle the state is
    ///       the level found on the pin, so the next toggle starts from it, one interval later.
    bool update();

    /// @brief Forces the pin to a given level and confirms it by reading the pin back.
    /// @param level `true` for HIGH, `false` for LOW.
    /// @return `true` if the pin reads back the requested level, `false` otherwise.
    /// @note Touches the hardware: writes the pin, then reads it back to confirm, so it
    ///       reports success or failure through its return value.
    ///       The state is always stored as read back from the pin: when the confirmation
    ///       fails, it is restored to the level actually found on the pin.
    ///       The readback confirms the level on the pin, not the load attached to it.
    ///       Does not reset the interval timer: the next automatic toggle keeps its
    ///       original schedule. Counterpart: `get_level()`.
    bool set_level(bool level);

    /// @brief Returns the level read back from the pin by the last `set_level()`.
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
    /// @brief Reads the level present on the pin.
    /// @return `true` if the pin is HIGH, `false` if LOW.
    /// @note Touches the hardware: reads the pin, not the cached state. Not `const`, as
    ///       for every `read_*`. It cannot block or time out, so it has no failure sentinel.
    bool read_level();

    /// @brief Drives the pin to a given level.
    /// @param level `true` for HIGH, `false` for LOW.
    /// @note Touches the hardware. Raw transfer primitive: it keeps no state, which is
    ///       the job of `set_level()`.
    void write_level(bool level);

    int _pin;                  ///< GPIO number driven by this instance (read by the HAL macros).
    uint32_t _interval_ms;     ///< Time between two toggles, in milliseconds.
    uint32_t _last_toggle_ms;  ///< Timestamp of the last toggle, in milliseconds.
    bool _level;               ///< Level read back from the pin by the last set_level().
};

#endif /* DUAL_BLINK_CPP_CORE_H */
