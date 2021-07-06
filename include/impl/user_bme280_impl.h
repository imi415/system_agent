#ifndef USER_BME280_IMPL_H
#define USER_BME280_IMPL_H

#include <stdint.h>

#include "bme280_dht.h"

typedef struct {
    int i2cdev_fd;
    uint8_t i2c_addr;
} user_bme280_impl_t;

int user_bme280_impl_init(user_bme280_impl_t *impl);

int user_bme280_impl_deinit(user_bme280_impl_t *impl);

bme280_ret_t user_bme280_impl_read_register_cb(user_bme280_impl_t *impl,
                                               uint8_t reg, uint8_t *data,
                                               uint8_t len);

bme280_ret_t user_bme280_impl_write_register_cb(user_bme280_impl_t *impl,
                                                uint8_t reg, uint8_t data);

#endif