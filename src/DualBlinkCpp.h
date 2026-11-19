#ifndef DUAL_BLINK_CPP_H
#define DUAL_BLINK_CPP_H

#include "core/dual_blink_cpp.h"

class DualBlinkCpp {
  private:
    dual_blink_cpp _core;

  public:
    DualBlinkCpp(int pin, uint32_t intervalMs = 500) : _core(pin, intervalMs) {}

    void begin()                        { _core.begin(); }
    void update()                       { _core.update(); }
    void setLevel(bool level)           { _core.set_level(level); }
    bool getLevel() const               { return _core.get_level(); }
    void setIntervalMs(uint32_t ms)     { _core.set_interval_ms(ms); }
    uint32_t getIntervalMs() const      { return _core.get_interval_ms(); }
};

#endif /* DUAL_BLINK_CPP_H */