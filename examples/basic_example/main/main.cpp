#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "dual_blink_cpp.h"

extern "C" void app_main(void) {
    dual_blink_cpp led(2, 500);
    led.begin();

    while (true) {
        led.update();
        vTaskDelay(1);  // lets the IDLE task run, see Appendix B
    }
}