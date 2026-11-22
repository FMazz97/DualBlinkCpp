/// @file DualBlinkCpp.h
/// @brief Arduino-facing facade for the DualBlinkCpp library (non-blocking GPIO blinker).
/// @details Thin camelCase wrapper over the framework-agnostic `dual_blink_cpp` core.
///          Provides an Arduino-style API where every method forwards 1:1 to the
///          underlying core driver. No logic is implemented here.
///          Method names are derived mechanically from the core ones (drop the
///          underscores, capitalise the next letter: `set_interval_ms()` becomes
///          `setIntervalMs()`), so the prefix contract of the core carries over
///          unchanged. The method documentation is a verbatim copy of the core's
///          (only names mapped to camelCase), because IDE IntelliSense does not
///          expand Doxygen's `@copydoc`: when a comment changes in the core, change
///          it here too.
/// @author FMazz97 (https://github.com/FMazz97)
/// @see dual_blink_cpp.h, dual_blink_cpp.cpp, dual_blink_cpp_hal_arduino.h,
///      Chapter 3 (Agnostic Core & HAL Architecture), Appendix C (Naming Convention)
///      of the Dual-Framework Library Guide:
///      https://github.com/FMazz97/dual-framework-library-guide
/// @copyright MIT License

#ifndef DUAL_BLINK_CPP_H
#define DUAL_BLINK_CPP_H

#include "core/dual_blink_cpp.h"

/**
 * @brief Non-blocking blinker for a single GPIO pin (Arduino facade).
 * @see dual_blink_cpp for the full method documentation (identical behavior, snake_case).
 * @note This header and the core one (`core/dual_blink_cpp.h`) differ only in case:
 *       they must never share a folder (here: `src/` and `src/core/`).
 */
class DualBlinkCpp {
  private:
    dual_blink_cpp _core;  ///< Framework-agnostic core every method forwards to.

  public:
    /// @brief Constructs a blinker bound to a pin.
    /// @param pin GPIO number to drive (Arduino pin number or ESP-IDF gpio_num_t value).
    /// @param intervalMs Time between two toggles, in milliseconds (default 500).
    /// @note No hardware access: only stores state. Call begin() before the first update().
    DualBlinkCpp(int pin, uint32_t intervalMs = 500) : _core(pin, intervalMs) {}

    /// @brief Configures the pin as output, drives it LOW and starts the interval timer.
    /// @note Touches the hardware. Name outside the prefix scheme (universal, like in Arduino).
    void begin()                        { _core.begin(); }

    /// @brief Toggles the pin if at least `intervalMs` has elapsed since the last toggle.
    /// @details Must be called periodically, as often as the desired timing accuracy requires.
    /// @note Never blocks: returns immediately when the interval has not elapsed yet.
    ///       Touches the hardware only when it toggles.
    void update()                       { _core.update(); }

    /// @brief Forces the pin to a given level and records it as the current state.
    /// @param level `true` for HIGH, `false` for LOW.
    /// @note Pushes the state to a GPIO pin, which cannot answer: there is nothing
    ///       to confirm, so it returns `void` (see the `set_*` contract).
    ///       Does not reset the interval timer: the next automatic toggle keeps its
    ///       original schedule. Counterpart: `getLevel()`.
    void setLevel(bool level)           { _core.set_level(level); }

    /// @brief Returns the last level written to the pin.
    /// @return `true` if HIGH, `false` if LOW.
    /// @note No hardware access: returns the cached value, the pin is never read back.
    ///       Always `const`, cannot fail or block.
    bool getLevel() const               { return _core.get_level(); }

    /// @brief Sets the time between two toggles.
    /// @param intervalMs New interval in milliseconds. Takes effect from the next `update()` call.
    /// @note No hardware access: only stores state. Counterpart: `getIntervalMs()`.
    void setIntervalMs(uint32_t intervalMs)  { _core.set_interval_ms(intervalMs); }

    /// @brief Returns the current time between two toggles.
    /// @return Interval in milliseconds.
    /// @note No hardware access. Always `const`, cannot fail or block.
    uint32_t getIntervalMs() const      { return _core.get_interval_ms(); }
};

#endif /* DUAL_BLINK_CPP_H */
