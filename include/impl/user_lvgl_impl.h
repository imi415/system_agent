#ifndef __USER_LVGL_IMPL_H
#define __USER_LVGL_IMPL_H

#include "lvgl.h"

int user_lvgl_impl_init(void);

void user_lvgl_impl_flush_cb(lv_disp_drv_t *disp_drv, const lv_area_t *area,
                             lv_color_t *color_p);

void user_lvgl_impl_indev_read_cb(lv_indev_drv_t *drv, lv_indev_data_t *data);

void user_lvgl_impl_log_cb(const char *buf);

void *user_lvgl_impl_fs_open_cb(lv_fs_drv_t *drv, const char *path,
                                lv_fs_mode_t mode);

lv_fs_res_t user_lvgl_impl_fs_close_cb(lv_fs_drv_t *drv, void *file_p);

lv_fs_res_t user_lvgl_impl_fs_read_cb(lv_fs_drv_t *drv, void *file_p, void *buf,
                                      uint32_t btr, uint32_t *br);

lv_fs_res_t user_lvgl_impl_fs_write_cb(lv_fs_drv_t *drv, void *file_p,
                                       const void *buf, uint32_t btw,
                                       uint32_t *bw);

lv_fs_res_t user_lvgl_impl_fs_seek_cb(lv_fs_drv_t *drv, void *file_p,
                                      uint32_t pos, lv_fs_whence_t whence);

lv_fs_res_t user_lvgl_impl_fs_tell_cb(lv_fs_drv_t *drv, void *file_p,
                                      uint32_t *pos_p);

void user_lvgl_impl_deinit(void);

#endif