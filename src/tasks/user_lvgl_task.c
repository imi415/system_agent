#include <stdint.h>

#include <unistd.h>
#include <pthread.h>

#include "lvgl.h"

#include "tasks/user_tasks.h"

extern uint8_t g_running;

pthread_t user_lv_task_thread;
pthread_t user_lv_tick_thread;

void *user_lv_task(void *arguments);
void *user_lv_tick(void *arguments);

int user_lvgl_task_init(void) {
    int ret;

    lv_init();

    ret = pthread_create(&user_lv_task_thread, NULL, user_lv_task, NULL);
    if(ret) return ret;
    ret = pthread_create(&user_lv_tick_thread, NULL, user_lv_tick, NULL);
    if(ret) return ret;
}

int user_lvgl_task_deinit(void) {
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