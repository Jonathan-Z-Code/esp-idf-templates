// start of espressif headers
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
// end of espressif headers

// my peripheral implementations so far
#include "uart.h"
#include "wdt.h"
#include "tasks.h"
#include "timers.h"
#include "custom_gpio.h"
// end of peripheral implementations


// app_main theoretically is a freeRTOS task
// dont call vStartScheduler(); , you'll get a panic reset

// main entry point
void app_main() {

    char* TAG = "timer";
    uart_init_0();
    wdt_disable();
    timer_init_0();
    gpio_enable_dbg_led();

    /* create test task via freeRTOS API! */
    xTaskCreate(my_task, "test", 4000, NULL, 1, NULL);

    // don't exit app_main, might mess with freeRTOS scheduler?
    // rn this is a interrupt_driven main loop
    while(1) {
        if(timer_overflow_event()) {
            ESP_LOGI(TAG, "overflow event discovered");
            timer_clear_overflow_flag();
        }
    }
}

