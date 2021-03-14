#include <stdint.h>

#include <unistd.h>
#include <pthread.h>

#include "lvgl.h"

#include "utils/user_log_util.h"
#include "tasks/user_tasks.h"

extern uint8_t g_running;

pthread_t user_lv_task_thread;
pthread_t user_lv_tick_thread;

void *user_lv_task(void *arguments);
void *user_lv_tick(void *arguments);

int user_lvgl_task_init(void) {
    int ret;

    USER_LOG(USER_LOG_INFO, "lv_init() called.");

    lv_init();

    ret = pthread_create(&user_lv_task_thread, NULL, user_lv_task, NULL);
    if(ret) return ret;
    ret = pthread_create(&user_lv_tick_thread, NULL, user_lv_tick, NULL);
    if(ret) return ret;

    USER_LOG(USER_LOG_INFO, "LVGL threads created.");

    pthread_setname_np(user_lv_task_thread, "LV_TASK");
    pthread_setname_np(user_lv_tick_thread, "LV_TICK");
}

int user_lvgl_task_deinit(void) {
    USER_LOG(USER_LOG_INFO, "LVGL task_deinit() called.");

    pthread_join(user_lv_task_thread, NULL);
    pthread_join(user_lv_tick_thread, NULL);

    USER_LOG(USER_LOG_INFO, "LVGL threads joined.");
    return 0;
}

void *user_lv_task(void *arguments) {

    while(g_running) {
        usleep(30 * 1000);
        lv_task_handler();
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
