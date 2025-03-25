#include "esp_err.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "unity.h"
#include <string.h>

#include "uart.h"
#include "wdt.h"

static const int UART_BUF_SIZE = 1024;
static const int UART_QUEUE_SIZE = 15;
static const int PATTERN_CHR_NUM = 1;
#define RD_BUF_SIZE (UART_BUF_SIZE)

static QueueHandle_t uart1_queue;
const char* TAG = "uart1";

// taken from esp-idf uart examples and edited to my application
static void uart_event_task(void *pvParameters)
{
    uart_event_t event;
    size_t buffered_size;
    uint8_t* dtmp = (uint8_t*) malloc(RD_BUF_SIZE);
    for (;;) {
        //Waiting for UART event.
        if (xQueueReceive(uart1_queue, (void *)&event, (TickType_t)portMAX_DELAY)) {
            // zero out dtmp buffer
            bzero(dtmp, RD_BUF_SIZE);
            // log specific callback function
            ESP_LOGI(TAG, "uart[%d] event:", UART_NUM_1);
            
            // based on the event, process data differently
            switch (event.type) {
            //Event of UART receiving data
            /*We'd better handler data event fast, there would be much more data events than
            other types of events. If we take too much time on data event, the queue might
            be full.*/
            case UART_DATA:
                ESP_LOGI(TAG, "[UART DATA]: %d", event.size);
                uart_read_bytes(UART_NUM_1, dtmp, event.size, portMAX_DELAY);
                ESP_LOGI(TAG, "[DATA EVT]:");
                uart_write_bytes(UART_NUM_1, (const char*) dtmp, event.size);
                break;
            //Event of HW FIFO overflow detected
            case UART_FIFO_OVF:
                ESP_LOGI(TAG, "hw fifo overflow");
                // If fifo overflow happened, you should consider adding flow control for your application.
                // The ISR has already reset the rx FIFO,
                // As an example, we directly flush the rx buffer here in order to read more data.
                uart_flush_input(UART_NUM_1);
                xQueueReset(uart1_queue);
                break;
            //Event of UART ring buffer full
            case UART_BUFFER_FULL:
                ESP_LOGI(TAG, "ring buffer full");
                // If buffer full happened, you should consider increasing your buffer size
                // As an example, we directly flush the rx buffer here in order to read more data.
                uart_flush_input(UART_NUM_1);
                xQueueReset(uart1_queue);
                break;
            //Event of UART RX break detected
            case UART_BREAK:
                ESP_LOGI(TAG, "uart rx break");
                break;
            //Event of UART parity check error
            case UART_PARITY_ERR:
                ESP_LOGI(TAG, "uart parity error");
                break;
            //Event of UART frame error
            case UART_FRAME_ERR:
                ESP_LOGI(TAG, "uart frame error");
                break;
            //UART_PATTERN_DET
            case UART_PATTERN_DET:
                uart_get_buffered_data_len(UART_NUM_1, &buffered_size);
                int pos = uart_pattern_pop_pos(UART_NUM_1);
                ESP_LOGI(TAG, "[UART PATTERN DETECTED] pos: %d, buffered size: %d", pos, buffered_size);
                if (pos == -1) {
                    // There used to be a UART_PATTERN_DET event, but the pattern position queue is full so that it can not
                    // record the position. We should set a larger queue size.
                    // As an example, we directly flush the rx buffer here.
                    uart_flush_input(UART_NUM_1);
                } else {
                    // im pretty sure uart_read_bytes pops off chars that are read 
                    uart_read_bytes(UART_NUM_1, dtmp, pos, 100 / portTICK_PERIOD_MS);
                    uint8_t pat[PATTERN_CHR_NUM + 1];
                    memset(pat, 0, sizeof(pat));
                    uart_read_bytes(UART_NUM_1, pat, PATTERN_CHR_NUM, 100 / portTICK_PERIOD_MS);
                    ESP_LOGI(TAG, "read data: %s", dtmp);
                    ESP_LOGI(TAG, "read pat : 0x%02X, 0x%02X", pat[0], pat[1]);
                }
                break;
            //Others
            default:
                ESP_LOGI(TAG, "uart event type: %d", event.type);
                break;
            }
        }
    }
    free(dtmp);
    dtmp = NULL;
    vTaskDelete(NULL);
}


static const uart_config_t gps_config = {
    .baud_rate = 38400,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
};

void uart_init_1(void) {

    // edit the install and config settings for GPS
    // NOTE: UART1 ISR should be higher priority than UART0 ISR
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_1, UART_BUF_SIZE, UART_BUF_SIZE, UART_QUEUE_SIZE, &uart1_queue, ESP_INTR_FLAG_LEVEL3));
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_1, &gps_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_1, 10, 9, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    
    // you might have to mess with chr_tout functionality. 
    /*
    Via CHATGPT:
    How TIMEOUT_THR (char_tout value) Works:
    - if post_idle and pre_idle are 0, then message is immediately processed.
    - effectively, post_idle and pre_idle override char_tout if both are set to 0
    */

    ESP_ERROR_CHECK(uart_enable_pattern_det_baud_intr(UART_NUM_1, '\n', PATTERN_CHR_NUM, 25, 0, 0));
    ESP_ERROR_CHECK(uart_pattern_queue_reset(UART_NUM_1, UART_QUEUE_SIZE));

    // flush entire rx buffer
    ESP_ERROR_CHECK(uart_flush_input(UART_NUM_1));

    // check if UART 1 interrupt was established!
    // CAREFUL THIS FUNCTION TAKES A WHILE TO PRINT SERIAL DATA TO STDOUT (UART0)
    esp_intr_dump(NULL);
    // create uart event callback function!
    xTaskCreate(uart_event_task, "uart_ev_tsk", 3500, NULL, 10, NULL);

}

void test_uart1_init(void) {
    // try to init this uart 1 port (unsure)
    uart_init_1();
}



int run_unity_tests(void) {
    
    UNITY_BEGIN();
    
    // check if init even works
    RUN_TEST(test_uart1_init);

    return UNITY_END();
}


void app_main(void) {
    // disable wdt for this unit test
    wdt_disable();
    // enable stable uart 0
    uart_init_0();
    
    // esp_intr_dump(NULL);
    // wait ~2 for unity test runner 
    // to establish connection
    vTaskDelay(pdMS_TO_TICKS(2000));

    // run unity tests
    //run_unity_tests();
    test_uart1_init();

    while(1) {}
}