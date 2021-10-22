#include "user_common.h"
#include "tasks/user_task_lvgl_common.h"
#include "impl/user_mqtt_impl.h"

static user_mqtt_impl_t s_mqtt_impl;

mqtt_influx_t g_mqtt_influx = {
    .cb = {
        .get_nsec_timestamp_cb = (mqtt_influx_ret_t(*)(void *, char *))user_mqtt_get_nsec_timestamp_cb,
        .publish_message_cb = (mqtt_influx_ret_t(*)(void *, char *))user_mqtt_publish_message_cb,
        .ready_cb = (bool(*)(void *))user_mqtt_ready_cb,
    },
    .user_data = &s_mqtt_impl,
    .hostname = "SystemAgent",
};

pthread_t user_mqtt_task_thread;

void *user_mqtt_task(void *arguments);

int user_mqtt_task_init(void) {
    int ret;

    ret = pthread_create(&user_mqtt_task_thread, NULL, user_mqtt_task, NULL);
    if(ret) return ret;

    pthread_setname_np(user_mqtt_task_thread, "MQTT");

    USER_LOG(USER_LOG_INFO, "MQTT thread created.");

    return 0;
}

int user_mqtt_task_deinit(void) {
    USER_LOG(USER_LOG_INFO, "MQTT task_deinit() called.");

    pthread_join(user_mqtt_task_thread, NULL);

    USER_LOG(USER_LOG_INFO, "MQTT tasks joined.");

    return 0;
}

void *user_mqtt_task(void *arguments) {
    UNUSED(arguments);

    if(user_mqtt_impl_init(&s_mqtt_impl) != 0) {
        USER_LOG(USER_LOG_ERROR, "MQTT implementation init failed.");
        return NULL;
    }

    mqtt_influx_init(&g_mqtt_influx);

    while(g_running) {
        if(user_mqtt_network_loop(&s_mqtt_impl) !=0) {
            USER_LOG(USER_LOG_ERROR, "MQTT network loop returned failure.");
        }
        usleep(5 * 1000);
    }

    user_mqtt_impl_deinit(&s_mqtt_impl);

    return NULL;
}