#include <stdio.h>
#include <stdint.h>

#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "tasks/user_tasks.h"

uint8_t g_running = 1;

int main(int argc, const char *argv[]) {
    user_lvgl_task_init();
    while(g_running) {
        sleep(1);
    }
    user_lvgl_task_deinit();
    return 0;
}