#include "driver/i2c_master.h"
#include "driver/i2c_types.h"
#include "esp_intr_alloc.h"

#define I2C_MASTER_SCL_IO (GPIO_NUM_22)
#define I2C_MASTER_SDA_IO (GPIO_NUM_21)

// i2c master bus config
const i2c_master_bus_config_t i2c_master_config = {
    .clk_source = I2C_CLK_SRC_APB,
    .i2c_port = 0,
    .scl_io_num = I2C_MASTER_SCL_IO,
    .sda_io_num = I2C_MASTER_SDA_IO,
    .glitch_ignore_cnt = 7,
    .trans_queue_depth = 10,
    .flags.enable_internal_pullup = true,
};

// i2c mock temperature sensor device config
const i2c_device_config_t temp_sensor = {
    .dev_addr_length = I2C_ADDR_BIT_LEN_7,
    .device_address = 0x1A,
    .scl_speed_hz = 100000, // deafault 100kHz operation
    .scl_wait_us = 0, // set to 0 for default value 
};

// i2c handle variables to keep track of configs
i2c_master_bus_handle_t bus_handle;
i2c_master_dev_handle_t temp_handle;

static volatile bool _i2c_master_trans_finished = false;

// interesting observation, I think you can configure multiple callback functions for different I2C devices (peripherals)
bool i2c_master_trans_finished_cb(i2c_master_dev_handle_t i2c_dev, const i2c_master_event_data_t *evt_data, void *arg) {
    if( evt_data->event == I2C_EVENT_DONE) {
        _i2c_master_trans_finished = true;
    }
    return false;
};

i2c_master_event_callbacks_t test = {
    .on_trans_done = i2c_master_trans_finished_cb,
};

void i2c_setup_master(void) {

    // allocate space in memory bus for I2C master bus
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_master_config, &bus_handle));
    // register I2C device to the master bus (add your external peripherals)
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &temp_sensor, &temp_handle));
    ESP_ERROR_CHECK(i2c_master_register_event_callbacks(temp_handle, &test, NULL));
    return;

}

void i2c_send_data(void) {
    // declare test data and timeout value
    const uint8_t I2C_TRANSMIT_TIMEOUT = 24;
    uint8_t test_buf[10] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A};
    // send entire data stream to temperature peripheral
    ESP_ERROR_CHECK(i2c_master_transmit(temp_handle, test_buf, sizeof(test_buf), I2C_TRANSMIT_TIMEOUT));
    return;
}

bool i2c_is_trans_done(void) {
    return _i2c_master_trans_finished;
}

void i2c_clear_trans_flag(void) {
    _i2c_master_trans_finished = false;
}