#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "tasks.h"

void my_task(void *pvParameter) {
    while(1) {
        static const char* TAG = "testing";
        ESP_LOGI(TAG, "MSG");
        vTaskDelay(pdMS_TO_TICKS(1000));   
    }
}