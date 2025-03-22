#include "esp_task_wdt.h"
#include "wdt.h"

static const int CORE_0 = 0;

static const esp_task_wdt_config_t wdt_config = {
    .timeout_ms = 10000,
    .idle_core_mask = 1 << CORE_0,
    .trigger_panic = true,
};

void wdt_disable(void) {
    ESP_ERROR_CHECK(esp_task_wdt_reconfigure(&wdt_config));
    // disable esp_wdt (you could implement your own idle task and reset esp_wdt there)
    esp_task_wdt_deinit();
}