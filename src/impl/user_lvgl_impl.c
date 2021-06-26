#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "drivers/user_config_driver.h"

#include "impl/user_lvgl_impl.h"
#include "impl/user_st7789_impl.h"

#include "utils/user_log_util.h"

extern user_config_t g_config;

user_st7789_impl_t g_lcd_impl;
st7789_lcd_t g_lcd = {
    .cb =
        {
            .write_cmd_cb = user_st7789_impl_write_cmd,
            .write_data_cb = user_st7789_impl_write_data,
            .reset_cb = user_st7789_impl_reset,
            .backlight_cb = user_st7789_impl_backlight,
        },
    .config =
        {
            .direction = ST7789_DIR_270,
            .pix_fmt = ST7789_RGB565,
            .bgr_mode = 0,
            .inversion = 0,
        },
    .user_data = &g_lcd_impl,
};

void user_lvgl_impl_init(void) {
    user_st7789_impl_init(&g_lcd_impl);
    st7789_lcd_init(&g_lcd);
}

void user_lvgl_impl_flush_cb(lv_disp_drv_t *disp_drv, const lv_area_t *area,
                             lv_color_t *color_p) {
    st7789_lcd_load(&g_lcd, (uint8_t *)color_p, area->x1, area->x2, area->y1,
                    area->y2);
    lv_disp_flush_ready(disp_drv);
}

void *user_lvgl_impl_fs_open_cb(lv_fs_drv_t *drv, const char *path,
                                lv_fs_mode_t mode) {
    char canonical_path[256];
    char *fs_base_dir =
        user_config_lookup_string(&g_config, "agent.libraries.lvgl.fs_base");
    if(fs_base_dir == NULL) {
        snprintf(canonical_path, 256, "%s", path);
    } else {
        snprintf(canonical_path, 256, "%s/%s", fs_base_dir, path);
    }

    int oflag = (mode == LV_FS_MODE_RD) ? O_RDONLY : O_RDWR;

    int fd = open(canonical_path, oflag);

    USER_LOG(USER_LOG_DEBUG, "Called open() on %s, fd=%d", canonical_path, fd);

    if(fd < 0) return NULL;

    int *ret = malloc(sizeof(fd));
    if(ret != NULL) *ret = fd;

    USER_LOG(USER_LOG_DEBUG, "Allocated fd %d@%p", fd, ret);

    return ret;
}

lv_fs_res_t user_lvgl_impl_fs_close_cb(lv_fs_drv_t *drv, void *file_p) {
    int fd = *(int *)file_p;

    if(fd > 0) {
        free(file_p);

        USER_LOG(USER_LOG_DEBUG, "Free'd fd %d@%p", fd, file_p);

        close(fd);

        USER_LOG(USER_LOG_DEBUG, "Called close() on fd %d", fd);

        return LV_FS_RES_OK;
    }
    return LV_FS_RES_FS_ERR;
}

lv_fs_res_t user_lvgl_impl_fs_read_cb(lv_fs_drv_t *drv, void *file_p, void *buf,
                                      uint32_t btr, uint32_t *br) {
    int fd = *(int *)file_p;

    if(fd > 0) {
        *br = read(fd, buf, btr);

        USER_LOG(USER_LOG_DEBUG, "Called read() on fd %d, len=%d, rlen=%d", fd,
                 btr, *br);

        if(*br < 0) return LV_FS_RES_FS_ERR;
        return LV_FS_RES_OK;
    }

    return LV_FS_RES_FS_ERR;
}

lv_fs_res_t user_lvgl_impl_fs_write_cb(lv_fs_drv_t *drv, void *file_p,
                                       const void *buf, uint32_t btw,
                                       uint32_t *bw) {
    int fd = *(int *)file_p;

    if(fd > 0) {
        *bw = write(fd, buf, btw);
        if(*bw < 0) return LV_FS_RES_FS_ERR;
        return LV_FS_RES_OK;
    }

    return LV_FS_RES_FS_ERR;
}

lv_fs_res_t user_lvgl_impl_fs_seek_cb(lv_fs_drv_t *drv, void *file_p,
                                      uint32_t pos, lv_fs_whence_t whence) {
    int fd = *(int *)file_p;

    int l_whence = SEEK_SET;
    switch(whence) {
    case LV_FS_SEEK_CUR:
        l_whence = SEEK_CUR;
        break;
    case LV_FS_SEEK_END:
        l_whence = SEEK_END;
        break;
    case LV_FS_SEEK_SET:
    default:
        break;
    }

    if(fd > 0) {
        int new_offset = lseek(fd, pos, l_whence);

        USER_LOG(USER_LOG_DEBUG, "Called seek() on fd %d, pos=%d, whence=%d",
                 fd, pos, l_whence);

        if(new_offset < 0) return LV_FS_RES_FS_ERR;
        return LV_FS_RES_OK;
    }

    return LV_FS_RES_FS_ERR;
}

lv_fs_res_t user_lvgl_impl_fs_tell_cb(lv_fs_drv_t *drv, void *file_p,
                                      uint32_t *pos_p) {
    int fd = *(int *)file_p;

    if(fd > 0) {
        *pos_p = lseek(fd, 0, LV_FS_SEEK_CUR);

        USER_LOG(USER_LOG_DEBUG, "Called tell() on fd %d, pos=%d", fd, *pos_p);

        if(*pos_p < 0) return LV_FS_RES_FS_ERR;
        return LV_FS_RES_OK;
    }

    return LV_FS_RES_FS_ERR;
}

void user_lvgl_impl_deinit(void) {
    st7789_lcd_display(&g_lcd, 0);
    user_st7789_impl_deinit(&g_lcd_impl);
}