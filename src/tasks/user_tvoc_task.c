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
}

int user_tvoc_task_deinit(void) {
    pthread_join(user_tvoc_task_thread, NULL);

    return 0;
}

void *user_tvoc_task(void *arguments) {
    while(g_running && !g_lvgl_ready) {
        sleep(1);
    }

    while(g_running) {
        sleep(1);
    }
}
