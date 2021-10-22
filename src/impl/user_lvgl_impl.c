#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "user_common.h"
#include "drivers/user_config_driver.h"
#include "impl/user_lvgl_impl.h"
#include "impl/user_st7789_impl.h"
#include "impl/user_stick_impl.h"
#include "utils/user_log_util.h"

extern user_config_t g_config;

user_st7789_impl_t g_lcd_impl;
user_stick_impl_t g_stick_impl;
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

static user_stick_key_t s_previous_key = USER_STICK_NONE;
static bool s_lv_log_enabled = false;

int user_lvgl_impl_init(void) {
    if(user_st7789_impl_init(&g_lcd_impl) != 0) {
        USER_LOG(USER_LOG_ERROR,
                 "ST7789 support library initialization failed");
        return -1;
    };
    if(user_stick_impl_init(&g_stick_impl) != 0) {
        USER_LOG(USER_LOG_ERROR,
                 "Control stick support library initialization failed");
        return -1;
    }
    if(st7789_lcd_init(&g_lcd) != ST7789_OK) {
        USER_LOG(USER_LOG_ERROR, "ST7789 driver initialization failed.");
        return -1;
    }

    if(user_config_lookup_bool(&g_config, "agent.libraries.lvgl.logging_enabled",
                               &s_lv_log_enabled) != 0) {
        USER_LOG(USER_LOG_WARN,
                 "LVGL log level not found, fallback to default(off).");
    };

    return 0;
}

void user_lvgl_impl_flush_cb(lv_disp_drv_t *disp_drv, const lv_area_t *area,
                             lv_color_t *color_p) {
    st7789_lcd_load(&g_lcd, (uint8_t *)color_p, area->x1, area->x2, area->y1,
                    area->y2);
    lv_disp_flush_ready(disp_drv);
}

void user_lvgl_impl_indev_read_cb(lv_indev_drv_t *drv, lv_indev_data_t *data) {
    UNUSED(drv);

    user_stick_key_t key = user_stick_impl_read(&g_stick_impl);
    user_stick_key_t key_to_determine = USER_STICK_NONE;
    if(key != s_previous_key) {                 // Key state changed
        if(s_previous_key == USER_STICK_NONE) { // New key pressed
            data->state = LV_INDEV_STATE_PR;
            key_to_determine = key;
        } else {
            data->state = LV_INDEV_STATE_REL;
            key_to_determine = s_previous_key;
        }
        switch(key_to_determine) {
        case USER_STICK_LEFT:
            data->key = LV_KEY_PREV;
            break;
        case USER_STICK_RIGHT:
            data->key = LV_KEY_NEXT;
            break;
        case USER_STICK_UP:
            data->key = LV_KEY_UP;
            break;
        case USER_STICK_DOWN:
            data->key = LV_KEY_DOWN;
            break;
        case USER_STICK_PUSH:
            data->key = LV_KEY_ENTER;
            break;
        default:
            data->state = LV_INDEV_STATE_REL;
            break;
        }
        s_previous_key = key;
        USER_LOG(USER_LOG_INFO, "Stick event: %d, key: %d", data->state,
                 data->key);
    }
}

void user_lvgl_impl_log_cb(const char *buf) {
    if(s_lv_log_enabled) USER_LOG(USER_LOG_INFO, "LVGL: %s", buf);
}

void *user_lvgl_impl_fs_open_cb(lv_fs_drv_t *drv, const char *path,
                                lv_fs_mode_t mode) {
    UNUSED(drv);

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
    UNUSED(drv);

    int fd = *(int *)file_p;

    if(fd > 0) {
        USER_LOG(USER_LOG_DEBUG, "Free'd fd %d@%p", fd, file_p);
        free(file_p);

        USER_LOG(USER_LOG_DEBUG, "Called close() on fd %d", fd);
        close(fd);

        return LV_FS_RES_OK;
    }
    return LV_FS_RES_FS_ERR;
}

lv_fs_res_t user_lvgl_impl_fs_read_cb(lv_fs_drv_t *drv, void *file_p, void *buf,
                                      uint32_t btr, uint32_t *br) {
    UNUSED(drv);

    int fd = *(int *)file_p;

    if(fd > 0) {
        *br = read(fd, buf, btr);

        USER_LOG(USER_LOG_DEBUG, "Called read() on fd %d, len=%d, rlen=%d", fd,
                 btr, *br);

        if((int)*br < 0) return LV_FS_RES_FS_ERR;
        return LV_FS_RES_OK;
    }

    return LV_FS_RES_FS_ERR;
}

lv_fs_res_t user_lvgl_impl_fs_write_cb(lv_fs_drv_t *drv, void *file_p,
                                       const void *buf, uint32_t btw,
                                       uint32_t *bw) {
    UNUSED(drv);

    int fd = *(int *)file_p;

    if(fd > 0) {
        *bw = write(fd, buf, btw);
        if((int)*bw < 0) return LV_FS_RES_FS_ERR;
        return LV_FS_RES_OK;
    }

    return LV_FS_RES_FS_ERR;
}

lv_fs_res_t user_lvgl_impl_fs_seek_cb(lv_fs_drv_t *drv, void *file_p,
                                      uint32_t pos, lv_fs_whence_t whence) {
    UNUSED(drv);

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
    UNUSED(drv);

    int fd = *(int *)file_p;

    if(fd > 0) {
        *pos_p = lseek(fd, 0, LV_FS_SEEK_CUR);

        USER_LOG(USER_LOG_DEBUG, "Called tell() on fd %d, pos=%d", fd, *pos_p);

        if((int)*pos_p < 0) return LV_FS_RES_FS_ERR;
        return LV_FS_RES_OK;
    }

    return LV_FS_RES_FS_ERR;
}

void user_lvgl_impl_deinit(void) {
    st7789_lcd_display(&g_lcd, 0);
    user_st7789_impl_deinit(&g_lcd_impl);
}