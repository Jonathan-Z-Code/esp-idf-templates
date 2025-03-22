#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_intr_alloc.h"

#include "custom_gpio.h"

static const gpio_num_t blue_led_gpio = GPIO_NUM_2;
static const uint64_t blue_led_msk = (1ULL << 2);
//static const uint32_t HIGH = 1;
// static const int LOW = 0;

const gpio_config_t blue_led_config = {
    .pin_bit_mask = blue_led_msk,
    .mode = GPIO_MODE_DEF_OUTPUT,
    .intr_type = GPIO_INTR_DISABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .pull_up_en = GPIO_PULLUP_ENABLE,
};

void gpio_enable_dbg_led(void) {
    ESP_ERROR_CHECK(gpio_config(&blue_led_config));
    ESP_ERROR_CHECK(gpio_set_level(blue_led_gpio, 1));
}