#include <stdint.h>

#include "tasks/user_task_lvgl_common.h"

pthread_t user_clock_task_thread;

static char *s_wday[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

void *user_clock_task(void *arguments);

int user_clock_task_init(void) {
    int ret;

    ret = pthread_create(&user_clock_task_thread, NULL, user_clock_task, NULL);
    if(ret) return ret;

    pthread_setname_np(user_clock_task_thread, "CLOCK");

    USER_LOG(USER_LOG_INFO, "CLOCK thread created.");
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

    pthread_mutex_lock(&g_lvgl_mutex);

    lv_style_t label_clock_time_style;
    lv_style_init(&label_clock_time_style);

    lv_style_set_text_font(&label_clock_time_style, &encode_sans_sc_bold_48);
    lv_style_set_text_color(&label_clock_time_style,
                            lv_palette_main(LV_PALETTE_PINK));

    lv_style_t label_clock_date_style;
    lv_style_init(&label_clock_date_style);
    lv_style_set_text_font(&label_clock_date_style, &encode_sans_sc_regular_32);
    lv_style_set_text_color(&label_clock_date_style,
                            lv_palette_main(LV_PALETTE_PINK));

    lv_obj_t *label_clock_time = lv_label_create(lv_scr_act());
    lv_obj_t *label_clock_date = lv_label_create(lv_scr_act());

    lv_obj_add_style(label_clock_time, &label_clock_time_style, 0);
    lv_obj_add_style(label_clock_date, &label_clock_date_style, 0);

    lv_label_set_text(label_clock_date, "--- ----/--/--");
    lv_label_set_text(label_clock_time, "--:--:--");

    lv_obj_align(label_clock_time, LV_ALIGN_CENTER, 0, -24);
    lv_obj_align(label_clock_date, LV_ALIGN_CENTER, 0, 24);

    pthread_mutex_unlock(&g_lvgl_mutex);

    struct tm *cur_tm;
    time_t cur_time;

    while(g_running) {
        time(&cur_time);
        cur_tm = localtime(&cur_time);
        pthread_mutex_lock(&g_lvgl_mutex);
        lv_label_set_text_fmt(label_clock_date, "%s %04d/%02d/%02d",
                              s_wday[cur_tm->tm_wday], cur_tm->tm_year + 1900,
                              cur_tm->tm_mon + 1, cur_tm->tm_mday);
        lv_label_set_text_fmt(label_clock_time, "%02d:%02d:%02d",
                              cur_tm->tm_hour, cur_tm->tm_min, cur_tm->tm_sec);
        pthread_mutex_unlock(&g_lvgl_mutex);
        usleep(200 * 1000);
    }
}