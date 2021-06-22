#include <stdint.h>

#include <unistd.h>
#include <pthread.h>

#include "lvgl.h"

#include "drivers/user_config_driver.h"

#include "utils/user_log_util.h"

extern uint8_t g_running;
extern uint8_t g_lvgl_ready;
extern user_config_t g_config;

pthread_t user_clock_task_thread;

void *user_clock_task(void *arguments);

int user_clock_task_init(void) {
    int ret;

    ret = pthread_create(&user_clock_task_thread, NULL, user_clock_task, NULL);
    if(ret) return ret;

    pthread_setname_np(user_clock_task_thread, "CLOCK");

    USER_LOG(USER_LOG_INFO, "Clock thread created.");
}

int user_clock_task_deinit(void) {
    USER_LOG(USER_LOG_INFO, "CLOCK task_deinit() called.");

    pthread_join(user_clock_task_thread, NULL);

    USER_LOG(USER_LOG_INFO, "CLOCK tasks joined.");

    return 0;
}

void *user_clock_task(void *arguments) {
    while(g_running && !g_lvgl_ready) {
        sleep(1);
    }

    char *background_path = user_config_lookup_string(&g_config, "agent.theme.background");

    lv_obj_t * bg_image = lv_img_create(lv_scr_act());
    if(background_path != NULL) {
        lv_img_set_src(bg_image, background_path);
    }
    
    while(g_running) {
        sleep(1);
    }
}