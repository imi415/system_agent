#include "lvgl.h"

#include "impl/user_st7789_impl.h"

extern user_spi_driver_t g_spi;

user_st7789_impl_t g_lcd_impl;
st7789_lcd_t g_lcd = {
    .cb =
        {
            .write_cmd_cb = user_st7789_impl_write_cmd,
            .write_data_cb = user_st7789_impl_write_data,
            .reset_cb = user_st7789_impl_reset,
        },
    .config =
        {
            .direction = ST7789_DIR_0,
            .pix_fmt = ST7789_RGB888,
        },
    .user_data = &g_lcd_impl,
};

void user_lvgl_impl_init(void) {
    user_st7789_impl_init(&g_lcd_impl, &g_spi);
}
