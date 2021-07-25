#include "tasks/user_task_lvgl_common.h"

pthread_t user_base_task_thread;

void *user_base_task(void *arguments);

int user_base_task_init(void) {
    int ret;

    ret = pthread_create(&user_base_task_thread, NULL, user_base_task, NULL);
    if(ret) return ret;

    pthread_setname_np(user_base_task_thread, "BASE");

    USER_LOG(USER_LOG_INFO, "BASE thread created.");

    return 0;
}

int user_base_task_deinit(void) {
    USER_LOG(USER_LOG_INFO, "BASE task_deinit() called.");

    pthread_join(user_base_task_thread, NULL);

    USER_LOG(USER_LOG_INFO, "BASE tasks joined.");

    return 0;
}

static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CLICKED) {
        LV_LOG_USER("Clicked");
    }
    else if(code == LV_EVENT_VALUE_CHANGED) {
        LV_LOG_USER("Toggled");
    }
}

void *user_base_task(void *arguments) {
    while(g_running && !g_lvgl_ready) {
        sleep(1);
    }

    pthread_mutex_lock(&g_lvgl_mutex);

    lv_style_t background_style;
    lv_style_init(&background_style);
    lv_style_set_opa(&background_style, LV_OPA_30);

    char *background_path = user_config_lookup_string(&g_config, "agent.theme.background");
    lv_obj_t * background_image = lv_img_create(lv_scr_act());
    if(background_path != NULL) {
        lv_img_set_src(background_image, background_path);
    }

    lv_obj_move_background(background_image);
    lv_obj_add_style(background_image, &background_style, 0);

    pthread_mutex_unlock(&g_lvgl_mutex);
    
    while(g_running) {
        sleep(1);
    }

    return NULL;
}