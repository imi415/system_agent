#include <stdlib.h>
#include <unistd.h>

#include "lvgl.h"

#include "impl/user_st7789_impl.h"

int user_st7789_impl_init(void *handle, user_spi_driver_t *spi_driver) {

    user_st7789_impl_t *impl = handle;

    if(user_gpio_init(&impl->cs_gpio, "/dev/gpiochip0", 0) != USER_GPIO_OK)
        return -1;
    if(user_gpio_init(&impl->dc_gpio, "/dev/gpiochip0", 0) != USER_GPIO_OK)
        return -2;
    if(user_gpio_init(&impl->reset_gpio, "/dev/gpiochip0", 0) != USER_GPIO_OK)
        return -3;

    if(spi_driver == NULL) return -4;

    impl->spi_driver = spi_driver;

    return 0;
}

void user_st7789_impl_deinit(void *handle) {

    user_st7789_impl_t *impl = handle;

    user_gpio_deinit(&impl->cs_gpio);
    user_gpio_deinit(&impl->dc_gpio);
    user_gpio_deinit(&impl->reset_gpio);
}

st7789_ret_t user_st7789_impl_write_cmd(void *handle, uint8_t *cmd,
                                        uint8_t len) {

    user_st7789_impl_t *impl = handle;

    if(user_gpio_set(&impl->dc_gpio, 0) != USER_GPIO_OK) return ST7789_ERROR;
    if(user_gpio_set(&impl->cs_gpio, 0) != USER_GPIO_OK) return ST7789_ERROR;

    // ST7789VW requires parameters to be sent with DC=0.
    if(user_spi_driver_xfer(impl->spi_driver, cmd, NULL, len) != USER_SPI_OK) {
        user_gpio_set(&impl->cs_gpio, 1);
        return ST7789_ERROR;
    }
    if(user_gpio_set(&impl->cs_gpio, 1) != USER_GPIO_OK) return ST7789_ERROR;

    return ST7789_OK;
}

st7789_ret_t user_st7789_impl_write_data(void *handle, uint8_t *data,
                                         uint32_t len) {
    user_st7789_impl_t *impl = handle;

    if(user_gpio_set(&impl->dc_gpio, 1) != USER_GPIO_OK) return ST7789_ERROR;
    if(user_gpio_set(&impl->cs_gpio, 0) != USER_GPIO_OK) return ST7789_ERROR;
    if(user_spi_driver_xfer(impl->spi_driver, data, NULL, len) != USER_SPI_OK) {
        user_gpio_set(&impl->cs_gpio, 1);
        return ST7789_ERROR;
    }
    if(user_gpio_set(&impl->cs_gpio, 1) != USER_GPIO_OK) return ST7789_ERROR;

    return ST7789_OK;
}

st7789_ret_t user_st7789_impl_reset(void *handle) {

    user_st7789_impl_t *impl = handle;

    if(&impl->reset_gpio == NULL) return ST7789_OK;

    if(user_gpio_set(&impl->reset_gpio, 0) != USER_GPIO_OK) return ST7789_ERROR;
    usleep(10 * 1000); // Sleep 10ms
    if(user_gpio_set(&impl->cs_gpio, 1) != USER_GPIO_OK) return ST7789_ERROR;
}
