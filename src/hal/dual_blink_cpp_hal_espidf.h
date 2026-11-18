#ifndef DUAL_BLINK_CPP_HAL_ESPIDF_H
#define DUAL_BLINK_CPP_HAL_ESPIDF_H

#include "driver/gpio.h"
#include "esp_timer.h"

#define DUAL_BLINK_CPP_PIN_OUTPUT() gpio_set_direction(static_cast<gpio_num_t>(_pin), GPIO_MODE_OUTPUT)
#define DUAL_BLINK_CPP_PIN_WRITE(level) gpio_set_level(static_cast<gpio_num_t>(_pin), level)
#define DUAL_BLINK_CPP_MILLIS() (static_cast<uint32_t>(esp_timer_get_time() / 1000))

#endif /* DUAL_BLINK_CPP_HAL_ESPIDF_H */