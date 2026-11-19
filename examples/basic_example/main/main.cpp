#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "dual_blink_cpp.h"

// Test pin configuration (adjust according to your hardware setup)
#ifndef BLINK_PIN
#define BLINK_PIN 4
#endif

extern "C" void app_main(void) {
    dual_blink_cpp led(BLINK_PIN, 500);
    led.begin();

    while (true) {
        led.update();
        vTaskDelay(1);  // lets the IDLE task run, see Appendix B at https://github.com/FMazz97/dual-framework-library-guide
    }
}