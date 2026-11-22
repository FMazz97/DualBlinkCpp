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
    /// @return `true` if the pin reads back LOW, `false` otherwise (see `setLevel()`).
    /// @note Touches the hardware. Name outside the prefix scheme (universal, like in Arduino).
    bool begin() { return _core.begin(); }

    /// @brief Toggles the pin if at least `intervalMs` has elapsed since the last toggle.
    /// @details Must be called periodically, as often as the desired timing accuracy requires.
    /// @return `true` if nothing had to be done or the toggle was confirmed, `false` if the
    ///         pin did not read back the toggled level (see `setLevel()`).
    /// @note Never blocks: returns immediately when the interval has not elapsed yet.
    ///       Touches the hardware only when it toggles. After a failed toggle the state is
    ///       the level found on the pin, so the next toggle starts from it, one interval later.
    bool update() { return _core.update(); }

    /// @brief Forces the pin to a given level and confirms it by reading the pin back.
    /// @param level `true` for HIGH, `false` for LOW.
    /// @return `true` if the pin reads back the requested level, `false` otherwise.
    /// @note Touches the hardware: writes the pin, then reads it back to confirm, so it
    ///       reports success or failure through its return value (see the `set_*` contract).
    ///       The state is always stored as read back from the pin: when the confirmation
    ///       fails, it is restored to the level actually found on the pin.
    ///       The readback confirms the level on the pin, not the load attached to it.
    ///       Does not reset the interval timer: the next automatic toggle keeps its
    ///       original schedule. Counterpart: `getLevel()`.
    bool setLevel(bool level) { return _core.set_level(level); }

    /// @brief Returns the level read back from the pin by the last `setLevel()`.
    /// @return `true` if HIGH, `false` if LOW.
    /// @note No hardware access: returns the cached value, the pin is never read back.
    ///       Always `const`, cannot fail or block.
    bool getLevel() const { return _core.get_level(); }

    /// @brief Sets the time between two toggles.
    /// @param intervalMs New interval in milliseconds. Takes effect from the next `update()` call.
    /// @note No hardware access: only stores state. Counterpart: `getIntervalMs()`.
    void setIntervalMs(uint32_t intervalMs) { _core.set_interval_ms(intervalMs); }

    /// @brief Returns the current time between two toggles.
    /// @return Interval in milliseconds.
    /// @note No hardware access. Always `const`, cannot fail or block.
    uint32_t getIntervalMs() const { return _core.get_interval_ms(); }
};

#endif /* DUAL_BLINK_CPP_H */
