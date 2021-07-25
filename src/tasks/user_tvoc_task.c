#include <stdint.h>

#include "impl/user_ccs811_impl.h"

#include "tasks/user_task_lvgl_common.h"

void *user_tvoc_task(void *arguments);
pthread_t user_tvoc_task_thread;

int user_tvoc_task_init(void) {
    int ret;

    ret = pthread_create(&user_tvoc_task_thread, NULL, user_tvoc_task, NULL);
    if(ret) return ret;

    pthread_setname_np(user_tvoc_task_thread, "TVOC");

    return 0;
}

int user_tvoc_task_deinit(void) {
    pthread_join(user_tvoc_task_thread, NULL);

    return 0;
}

void *user_tvoc_task(void *arguments) {

    user_ccs811_impl_t impl;

    user_ccs811_impl_init(&impl);

    ccs811_t ccs = {
        .cb =
            {
                .write_register_cb = (ccs811_ret_t (*)(void *, uint8_t,  uint8_t *, uint8_t))user_ccs811_impl_write_register_cb,
                .read_register_cb = (ccs811_ret_t(*)(void *, uint8_t,  uint8_t *, uint8_t))user_ccs811_impl_read_register_cb,
                .delay_ms_cb = (ccs811_ret_t (*)(void *, uint32_t))user_ccs811_impl_delay_ms_cb,
            },
        .user_data = &impl,
    };

    ccs811_init(&ccs);
    ccs811_set_env_data(&ccs, 26.000, 40.000);

    while(g_running && !g_lvgl_ready) {
        sleep(1);
    }

    while(g_running) {
        ccs811_result_t result;
        ccs811_measure(&ccs, &result);
        sleep(1);
    }

    user_ccs811_impl_deinit(&impl);

    return NULL;
}
