#if defined(LEGACY_TIMER_VERSION)
    #include "driver/timer.h"
#else 
    #include "driver/gptimer.h"
#endif /* defined(LEGACY_TIMER_VERSION) */

#include "esp_log.h"
#include "driver/gpio.h"
#include <stdatomic.h>

// atomic boolean ISR flag for timer0 overflow 
atomic_bool _overflow_event = false;

// callback event when alarm event triggers (overflow)
bool alarm_event(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx) {
    atomic_store(&_overflow_event, true);
    return true;
}

#if defined(LEGACY_TIMER_VERSION)

timer_config_t timer_config = {
    .alarm_en = TIMER_ALARM_EN,         /*!< Timer alarm enable */
    .counter_en = TIMER_PAUSE,      /*!< Counter enable */
    .intr_type = TIMER_INTR_LEVEL,    /*!< Interrupt mode */
    .counter_dir = TIMER_COUNT_UP,  /*!< Counter direction  */
    .auto_reload = TIMER_AUTORELOAD_EN, /*!< Timer auto-reload */
    .clk_src = TIMER_SRC_CLK_APB,        /*!< Selects source clock. */
    .divider = 80000 - 1,               /*!< Counter clock divider */
};

#else

gptimer_handle_t gptimer = NULL;

const gptimer_config_t timer_config = {
    .clk_src = TIMER_SRC_CLK_APB,
    .direction = GPTIMER_COUNT_UP,
    .resolution_hz = 2000,
    .intr_priority = 2,
};
const gptimer_alarm_config_t alarm_config = {
    .alarm_count = 2000,
    .reload_count = 0,
    .flags.auto_reload_on_alarm = true,
};
const gptimer_event_callbacks_t callback_config = {
    .on_alarm = alarm_event,
};

#endif /* defined(LEGACY_TIMER_VERSION) */

// esp-idf docs say APB freq is typically 80 MHZ
void timer_init_0(void) {

    #if defined(LEGACY_TIMER_VERSION)
        ESP_ERROR_CHECK(timer_init(TIMER_GROUP_0, TIMER_0, &timer_config)); // should be a 1kHz timer (currently paused)
        ESP_ERROR_CHECK(timer_set_alarm_value(0,0,1000)); // autoreload is at 1000, (@ 1kHz, should be 1 second overflow timer)
        ESP_ERROR_CHECK(timer_start(TIMER_GROUP_0, TIMER_0)); // start timer after all config has completed
    #else
        ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer)); // init new timer (if success, timer is in "init stage")
        ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config)); // set alarm properties 
        ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &callback_config, 0)); // set alarm callbacks
        ESP_ERROR_CHECK(gptimer_enable(gptimer)); // set timer to "enable" mode 
        ESP_ERROR_CHECK(gptimer_start(gptimer)); // set timer to "start" mode
    #endif /* defined(LEGACY_TIMER_VERSION) */

}

bool timer_overflow_event(void) {
    return atomic_load(&_overflow_event);
}

void timer_clear_overflow_flag(void) {
    atomic_store(&_overflow_event, false);
}