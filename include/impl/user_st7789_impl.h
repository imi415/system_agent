#ifndef __USER_ST7789_IMPL_H
#define __USER_ST7789_IMPL_H

#include "drivers/user_gpio_driver.h"
#include "drivers/user_spi_driver.h"
#include "drivers/user_config_driver.h"

#include "st7789_lcd.h"

typedef struct {
    user_spi_driver_t *spi_driver;
    user_gpio_t *cs_gpio;
    user_gpio_t *dc_gpio;
    user_gpio_t *reset_gpio;
} user_st7789_impl_t;

int user_st7789_impl_init(void  *handle);
void user_st7789_impl_deinit(void *handle);
st7789_ret_t user_st7789_impl_write_cmd(void  *handle, uint8_t *cmd,
                                         uint8_t len);
st7789_ret_t user_st7789_impl_write_data(void  *handle,
                                          uint8_t *data, uint32_t len);
st7789_ret_t user_st7789_impl_reset(void  *handle);

#endif