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
#include "i2c.h"
// end of peripheral implementations

#define DEBUG_INTERRUPT_DUMP (1)

// app_main theoretically is a freeRTOS task? I think?
// dont call vStartScheduler(); , you'll get a panic reset
// main entry point
void app_main() {

    const char* TIM0_TAG = "timerISR";
    const char* I2C_TAG = "I2C_PORT0";
    uart_init_0();
    wdt_disable();
    timer_init_0();
    gpio_enable_dbg_led();
    i2c_setup_master();

    /* create test task via freeRTOS API! */
    xTaskCreate(my_task, "test", 4000, NULL, 1, NULL);

    // don't exit app_main, might mess with freeRTOS scheduler?
    // rn this is a interrupt_driven main loop

    // this interrupt dump does not include callback function names you provided
    #if DEBUG_INTERRUPT_DUMP
        esp_intr_dump(NULL); // get interrupt data from all cores
    #endif

    while(1) {
        if(timer_overflow_event()) {
            ESP_LOGI(TIM0_TAG, "overflow event discovered");
            gpio_toggle_dbg_led();
            timer_clear_overflow_flag();
            i2c_send_data();
        }
        if(i2c_is_trans_done()) {
            ESP_LOGI(I2C_TAG, "i2c transaction done!");
            i2c_clear_trans_flag();
        }
    }
}