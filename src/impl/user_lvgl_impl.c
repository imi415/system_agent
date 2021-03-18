#include "lvgl.h"

#include "st7789_lcd.h"

#include "drivers/user_gpio_driver.h"
#include "drivers/user_spi_driver.h"

extern user_spi_driver_t g_spi;

st7789_ret_t _user_lvgl_impl_lcd_write_cmd(void *handle, uint8_t *cmd,
                                           uint8_t len);
st7789_ret_t _user_lvgl_impl_lcd_write_data(void *handle, uint8_t *data,
                                            uint32_t len);
st7789_ret_t _user_lvgl_impl_lcd_reset(void *handle);

st7789_lcd_t g_lcd = {
    .cb =
        {
            .write_cmd_cb = _user_lvgl_impl_lcd_write_cmd,
            .write_data_cb = _user_lvgl_impl_lcd_write_data,
            .reset_cb = _user_lvgl_impl_lcd_reset,
        },
    .config =
        {
            .direction = ST7789_DIR_0,
            .pix_fmt = ST7789_RGB888,
        },
};

void user_lvgl_impl_init(void) {}

st7789_ret_t _user_lvgl_impl_lcd_write_cmd(void *handle, uint8_t *cmd,
                                           uint8_t len) {
    // TODO: Change CS and DC here
    if(user_spi_driver_xfer(&g_spi, cmd, NULL, len) != USER_SPI_OK) {
        return ST7789_ERROR;
    }
}

st7789_ret_t _user_lvgl_impl_lcd_write_data(void *handle, uint8_t *data,
                                            uint32_t len) {
    // TODO: Change CS and DC here
    if(user_spi_driver_xfer(&g_spi, data, NULL, len) != USER_SPI_OK) {
        return ST7789_ERROR;
    }
}

st7789_ret_t _user_lvgl_impl_lcd_reset(void *handle) {
    // Implement reset here.
}