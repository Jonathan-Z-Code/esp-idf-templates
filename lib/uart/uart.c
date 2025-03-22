#include "driver/uart.h"
#include "uart.h"

// static const int UART_BUF_SIZE = 1024;
static const uart_port_t uart_num = UART_NUM_0;

static const uart_config_t uart_config = {
    .baud_rate = 115200,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
};

void uart_init_0(void) {
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));
}