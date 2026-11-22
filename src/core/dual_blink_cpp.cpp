/// @file dual_blink_cpp.cpp
/// @brief Implementation of the framework-agnostic DualBlinkCpp core.
/// @details All hardware access goes through the DUAL_BLINK_CPP_* macros of the HAL
///          contract, and only inside read_level() and write_level() (plus the pin
///          setup in begin()). Method documentation (including the prefix contract
///          of each method) lives in dual_blink_cpp.h.
/// @author FMazz97 (https://github.com/FMazz97)
/// @see dual_blink_cpp.h, dual_blink_cpp_hal_arduino.h, dual_blink_cpp_hal_espidf.h
/// @copyright MIT License

#include "dual_blink_cpp.h"

dual_blink_cpp::dual_blink_cpp(int pin, uint32_t interval_ms)
    : _pin(pin), _interval_ms(interval_ms), _last_toggle_ms(0), _level(false) {}

bool dual_blink_cpp::begin() {
    DUAL_BLINK_CPP_PIN_OUTPUT();
    bool confirmed = set_level(false);
    _last_toggle_ms = DUAL_BLINK_CPP_MILLIS();
    return confirmed;
}

bool dual_blink_cpp::update() {
    uint32_t now = DUAL_BLINK_CPP_MILLIS();
    if (now - _last_toggle_ms >= _interval_ms) {
        _last_toggle_ms = now;
        return set_level(!_level);
    }
    return true;
}

bool dual_blink_cpp::set_level(bool level) {
    write_level(level);
    _level = read_level();
    return _level == level;
}

bool dual_blink_cpp::get_level() const {
    return _level;
}

void dual_blink_cpp::set_interval_ms(uint32_t interval_ms) {
    _interval_ms = interval_ms;
}

uint32_t dual_blink_cpp::get_interval_ms() const {
    return _interval_ms;
}

bool dual_blink_cpp::read_level() {
    return DUAL_BLINK_CPP_PIN_READ() != 0;
}

void dual_blink_cpp::write_level(bool level) {
    DUAL_BLINK_CPP_PIN_WRITE(level);
}
