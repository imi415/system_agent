#include "tasks/user_task_lvgl_common.h"

#include "impl/user_lvgl_impl.h"

#define PIXBUF_SIZE 320 * 20

uint8_t g_lvgl_ready = 0;
pthread_mutex_t g_lvgl_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_t user_lv_task_thread;
pthread_t user_lv_tick_thread;

static lv_disp_draw_buf_t s_disp_buf;
static lv_color_t s_pix_buf[2][PIXBUF_SIZE];
static lv_disp_drv_t s_disp_drv;
static lv_fs_drv_t s_fs_drv;
static lv_indev_drv_t s_indev_drv;

void *user_lv_task(void *arguments);
void *user_lv_tick(void *arguments);

int user_lvgl_task_init(void) {
    int ret;

    user_lvgl_impl_init();

    USER_LOG(USER_LOG_INFO, "lv_init() called.");

    pthread_mutex_lock(&g_lvgl_mutex);

    lv_init();

    lv_log_register_print_cb(user_lvgl_impl_log_cb);

    lv_disp_draw_buf_init(&s_disp_buf, s_pix_buf[0], s_pix_buf[1], PIXBUF_SIZE);

    lv_disp_drv_init(&s_disp_drv);
    s_disp_drv.draw_buf = &s_disp_buf;
    s_disp_drv.hor_res = 320;
    s_disp_drv.ver_res = 240;
    s_disp_drv.flush_cb = user_lvgl_impl_flush_cb;
    lv_disp_t *disp = lv_disp_drv_register(&s_disp_drv);
    if(disp == NULL) return -2;

    lv_indev_drv_init(&s_indev_drv);
    s_indev_drv.type = LV_INDEV_TYPE_KEYPAD;
    s_indev_drv.read_cb = user_lvgl_impl_indev_read_cb;
    lv_indev_t *indev = lv_indev_drv_register(&s_indev_drv);

    lv_group_t *indev_group = lv_group_create();
    lv_group_set_default(indev_group);
    lv_indev_set_group(indev, indev_group);

    lv_fs_drv_init(&s_fs_drv);
    s_fs_drv.letter = 'A';
    s_fs_drv.open_cb = user_lvgl_impl_fs_open_cb;
    s_fs_drv.close_cb = user_lvgl_impl_fs_close_cb;
    s_fs_drv.read_cb = user_lvgl_impl_fs_read_cb;
    s_fs_drv.write_cb = user_lvgl_impl_fs_write_cb;
    s_fs_drv.seek_cb = user_lvgl_impl_fs_seek_cb;
    s_fs_drv.tell_cb = user_lvgl_impl_fs_tell_cb;

    lv_fs_drv_register(&s_fs_drv);

    pthread_mutex_unlock(&g_lvgl_mutex);

    ret = pthread_create(&user_lv_task_thread, NULL, user_lv_task, NULL);
    if(ret) return ret;
    ret = pthread_create(&user_lv_tick_thread, NULL, user_lv_tick, NULL);
    if(ret) return ret;

    USER_LOG(USER_LOG_INFO, "LVGL threads created.");

    pthread_setname_np(user_lv_task_thread, "LV_TASK");
    pthread_setname_np(user_lv_tick_thread, "LV_TICK");

    g_lvgl_ready = 1;

    return 0;
}

int user_lvgl_task_deinit(void) {
    USER_LOG(USER_LOG_INFO, "LVGL task_deinit() called.");

    pthread_join(user_lv_task_thread, NULL);
    pthread_join(user_lv_tick_thread, NULL);

    USER_LOG(USER_LOG_INFO, "LVGL threads joined.");

    user_lvgl_impl_deinit();
    return 0;
}

void *user_lv_task(void *arguments) {

    while(g_running) {
        usleep(30 * 1000);
        pthread_mutex_lock(&g_lvgl_mutex);
        lv_timer_handler();
        pthread_mutex_unlock(&g_lvgl_mutex);
    }

    return NULL;
}

void *user_lv_tick(void *arguments) {
    while(g_running) {
        usleep(30 * 1000);
        pthread_mutex_lock(&g_lvgl_mutex);
        lv_tick_inc(30);
        pthread_mutex_unlock(&g_lvgl_mutex);
    }

    return NULL;
}
