#ifndef USER_CCS811_IMPL_H
#define USER_CCS811_IMPL_H

#include <stdint.h>

#include "drivers/user_gpio_driver.h"

#include "ccs811_tvoc.h"

typedef struct {
    int i2cdev_fd;
    uint8_t i2c_addr;
    user_gpio_t *nwake_pin;
} user_ccs811_impl_t;

int user_ccs811_impl_init(user_ccs811_impl_t *impl);

int user_ccs811_impl_deinit(user_ccs811_impl_t *impl);

ccs811_ret_t user_ccs811_impl_read_register_cb(user_ccs811_impl_t *impl,
                                               uint8_t reg, uint8_t *data,
                                               uint8_t len);

ccs811_ret_t user_ccs811_impl_write_register_cb(user_ccs811_impl_t *impl,
                                                uint8_t reg, uint8_t *data, uint8_t len);

ccs811_ret_t user_ccs811_impl_delay_ms_cb(user_ccs811_impl_t *impl, uint32_t delay_ms);

#endif