#ifndef __USER_LVGL_IMPL_H
#define __USER_LVGL_IMPL_H

#include "lvgl.h"

void user_lvgl_impl_init(void);
void user_lvgl_impl_flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
void user_lvgl_impl_deinit(void);

#endif