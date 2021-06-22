#include <stdint.h>

#include <unistd.h>
#include <pthread.h>

#include "lvgl.h"

#include "utils/user_log_util.h"
#include "tasks/user_tasks.h"
#include "impl/user_lvgl_impl.h"

#define PIXBUF_SIZE 320 * 10

extern uint8_t g_running;

uint8_t g_lvgl_ready = 0;

pthread_t user_lv_task_thread;
pthread_t user_lv_tick_thread;

static lv_disp_draw_buf_t s_disp_buf;
static lv_color_t s_pix_buf[2][PIXBUF_SIZE];
static lv_disp_drv_t s_disp_drv;

void *user_lv_task(void *arguments);
void *user_lv_tick(void *arguments);

int user_lvgl_task_init(void) {
    int ret;

    user_lvgl_impl_init();

    USER_LOG(USER_LOG_INFO, "lv_init() called.");

    lv_init();

    lv_disp_draw_buf_init(&s_disp_buf, s_pix_buf[0], s_pix_buf[1], PIXBUF_SIZE);

    
    lv_disp_drv_init(&s_disp_drv);
    s_disp_drv.draw_buf = &s_disp_buf;
    s_disp_drv.hor_res = 320;
    s_disp_drv.ver_res = 240;
    s_disp_drv.flush_cb = user_lvgl_impl_flush_cb;
    lv_disp_t *disp = lv_disp_drv_register(&s_disp_drv);

    ret = pthread_create(&user_lv_task_thread, NULL, user_lv_task, NULL);
    if(ret) return ret;
    ret = pthread_create(&user_lv_tick_thread, NULL, user_lv_tick, NULL);
    if(ret) return ret;

    USER_LOG(USER_LOG_INFO, "LVGL threads created.");

    pthread_setname_np(user_lv_task_thread, "LV_TASK");
    pthread_setname_np(user_lv_tick_thread, "LV_TICK");

    g_lvgl_ready = 1;
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
        lv_timer_handler();
    }

    return NULL;
}

void *user_lv_tick(void *arguments) {
    while(g_running) {
        usleep(30 * 1000);
        lv_tick_inc(30);
    }

    return NULL;
}
