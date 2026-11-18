#include "dual_blink_cpp.h"

dual_blink_cpp::dual_blink_cpp(int pin, uint32_t interval_ms)
    : _pin(pin), _interval_ms(interval_ms), _last_toggle_ms(0), _level(false) {}

void dual_blink_cpp::begin() {
    DUAL_BLINK_CPP_PIN_OUTPUT();
    set_level(false);
    _last_toggle_ms = DUAL_BLINK_CPP_MILLIS();
}

void dual_blink_cpp::update() {
    uint32_t now = DUAL_BLINK_CPP_MILLIS();
    if (now - _last_toggle_ms >= _interval_ms) {
        _last_toggle_ms = now;
        set_level(!_level);
    }
}

void dual_blink_cpp::set_level(bool level) {
    _level = level;
    DUAL_BLINK_CPP_PIN_WRITE(_level);
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