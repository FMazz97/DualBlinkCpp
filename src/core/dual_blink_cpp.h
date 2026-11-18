#ifndef DUAL_BLINK_CPP_CORE_H
#define DUAL_BLINK_CPP_CORE_H

#include <stdint.h>

#if defined(ARDUINO)
#include "../hal/dual_blink_cpp_hal_arduino.h"
#elif defined(ESP_PLATFORM)
#include "../hal/dual_blink_cpp_hal_espidf.h"
#else
#error "DualBlinkCpp core: unsupported platform (expected ARDUINO or ESP_PLATFORM)"
#endif

class dual_blink_cpp {
   public:
    explicit dual_blink_cpp(int pin, uint32_t interval_ms = 500);

    void begin();
    void update();

    void set_level(bool level);
    bool get_level() const;

    void set_interval_ms(uint32_t interval_ms);
    uint32_t get_interval_ms() const;

   private:
    int _pin;
    uint32_t _interval_ms;
    uint32_t _last_toggle_ms;
    bool _level;
};

#endif /* DUAL_BLINK_CPP_CORE_H */