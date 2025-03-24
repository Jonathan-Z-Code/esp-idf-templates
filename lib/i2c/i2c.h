#ifndef I2C_H_
#define I2C_H_

void i2c_setup_master(void);
void i2c_send_data(void);
bool i2c_is_trans_done(void);
void i2c_clear_trans_flag(void);

#endif