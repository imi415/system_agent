#include <stdint.h>

#include <unistd.h>
#include <pthread.h>

#include "lvgl.h"

#include "utils/user_log_util.h"

extern uint8_t g_running;
extern uint8_t g_lvgl_ready;

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
    lv_obj_t * label1 = lv_label_create(lv_scr_act());
    lv_label_set_long_mode(label1, LV_LABEL_LONG_WRAP);     /*Break the long lines*/
    lv_label_set_recolor(label1, true);                      /*Enable re-coloring by commands in the text*/
    lv_label_set_text(label1, "#0000ff Re-color# #ff00ff words# #ff0000 of a# label, align the lines to the center "
                              "and wrap long text automatically.");
    lv_obj_set_width(label1, 150);  /*Set smaller width to make the lines wrap*/
    lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label1, LV_ALIGN_CENTER, 0, -40);


    lv_obj_t * label2 = lv_label_create(lv_scr_act());
    lv_label_set_long_mode(label2, LV_LABEL_LONG_SCROLL_CIRCULAR);     /*Circular scroll*/
    lv_obj_set_width(label2, 150);
    lv_label_set_text(label2, "It is a circularly scrolling text. ");
    lv_obj_align(label2, LV_ALIGN_CENTER, 0, 40);
    while(g_running) {
        sleep(1);
    }
}