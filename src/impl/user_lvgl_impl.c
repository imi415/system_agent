#include "impl/user_st7789_impl.h"
#include "impl/user_lvgl_impl.h"

#include "utils/user_log_util.h"

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
            .direction = ST7789_DIR_270,
            .pix_fmt = ST7789_RGB565,
        },
    .user_data = &g_lcd_impl,
};

void user_lvgl_impl_init(void) {
    user_st7789_impl_init(&g_lcd_impl);
    st7789_lcd_init(&g_lcd);
}

void user_lvgl_impl_flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p) {
    st7789_lcd_load(&g_lcd, (uint8_t *)color_p, area->x1, area->x2, area->y1, area->y2);
    lv_disp_flush_ready(disp_drv);
}

void user_lvgl_impl_deinit(void) {
    st7789_lcd_display(&g_lcd, 0);
    user_st7789_impl_deinit(&g_lcd_impl);
}