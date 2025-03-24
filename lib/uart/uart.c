#include "driver/uart.h"
#include "uart.h"
#include "esp_intr_alloc.h"

static const int UART_BUF_SIZE = 1024;
static const int UART_QUEUE_SIZE = 10;
// static const int uart_intr_flag = ESP_INTR_FLAG_LEVEL4;

static const uart_config_t uart_config = {
    .baud_rate = 115200,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
};

void uart_init_0(void) {
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_0, UART_BUF_SIZE, UART_BUF_SIZE, UART_QUEUE_SIZE, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_0, &uart_config));
}