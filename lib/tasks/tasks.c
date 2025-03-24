#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "tasks.h"

void my_task(void *pvParameter) {
    while(1) {
        static const char* TAG = "my_task";
        ESP_LOGI(TAG, "rtos task test!");
        vTaskDelay(pdMS_TO_TICKS(3000));   
    }
}