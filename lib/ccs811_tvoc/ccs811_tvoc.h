#ifndef __I2CDEV_CCS811_H
#define __I2CDEV_CCS811_H

#include <stdint.h>

#define CCS_OK 0
#define CCS_FAIL -1

typedef int ccs811_ret_t;

typedef struct {
    uint16_t eco2;
    uint16_t tvoc;
} ccs811_result_t;

typedef struct {
    ccs811_ret_t (*delay_ms_cb)(void *handle, uint32_t msec);
    ccs811_ret_t (*read_register_cb)(void *handle, uint8_t reg, uint8_t *data, uint8_t len);
    ccs811_ret_t (*write_register_cb)(void *handle, uint8_t reg, uint8_t *data, uint8_t len);
} ccs811_cb_t;

typedef struct {
    uint16_t baseline;
    void *user_data;
    ccs811_cb_t cb;
} ccs811_t;

ccs811_ret_t ccs811_init(ccs811_t *ccs);
ccs811_ret_t ccs811_set_env_data(ccs811_t *ccs, double temperature, double humidity);
ccs811_ret_t ccs811_read_baseline(ccs811_t *ccs, uint8_t *baseline);
ccs811_ret_t ccs811_store_baseline(ccs811_t *ccs, uint8_t *baseline);
ccs811_ret_t ccs811_measure(ccs811_t *ccs, ccs811_result_t *result);

#endif