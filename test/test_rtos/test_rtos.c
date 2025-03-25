#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "unity.h"
#include "esp_log.h"

#include "wdt.h"

#define CORE_0 (0)

// test bit masks for freeRTOS to use
#define RX_BIT (1 << 6)
#define TX_BIT (1 << 5)

TaskHandle_t generate_event_handle;
TaskHandle_t process_event_handle;

/* Simple control flow between two freeRTOS tasks */
/* This example was influenced by freeRTOS API documentation */

// This task generates a notification
void task_generate_event(void* pvParameters) {
    const char* TAG = "generator_event";
    
    // hard code a delay so the other tasks get a chance to be created before
    // sending task notifications
    vTaskDelay(pdMS_TO_TICKS(1000));
    while(1) {
        xTaskNotify(process_event_handle, RX_BIT, eSetBits);
        ESP_LOGI(TAG, "Task is notified!");
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

// This task processes notifications
void task_process_event(void* pvParameters) {
    const char* TAG = "process_event";
    static unsigned long notify_value;
    
    while(1) {
        
        BaseType_t result = xTaskNotifyWait(pdTRUE,RX_BIT,&notify_value, portMAX_DELAY);
        ESP_LOGI(TAG, "Exit value is = %d", result);
        if(result == pdPASS) {

            if(notify_value & RX_BIT) {
                ESP_LOGI(TAG, "RX_BIT set!");
            }
            if(notify_value & TX_BIT) {
                ESP_LOGI(TAG, "TX_BIT set!");
            }
        }
    }
}

// Main entry function
void app_main() {
    
    // esp_log tag for freeRTOS API functions
    const char* TAG = "freeRTOS API";

    // disable wdt
    wdt_disable();    

    // create mock RTOS tasks
    BaseType_t result = xTaskCreatePinnedToCore(task_generate_event, "generatorTask", 2048, NULL, 13, &generate_event_handle, CORE_0);
    if(result != pdPASS) ESP_LOGW(TAG, "generatorTask not init"); 
    result = xTaskCreatePinnedToCore(task_process_event, "processTask", 2048, NULL, 12, &process_event_handle, CORE_0);
    if(result != pdPASS) ESP_LOGW(TAG, "processTask not init");

    // do something
    while(1) {}

}