#include <stdio.h>
#include <stdint.h>

#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "drivers/user_config_driver.h"
#include "drivers/user_spi_driver.h"
#include "utils/user_log_util.h"
#include "tasks/user_tasks.h"

uint8_t g_running = 1;

user_config_t g_config;

static void signal_handler(int signo) {
    if(signo == SIGINT) {
        g_running = 0;
    }
    USER_LOG(USER_LOG_INFO, "Signal %d captured, stopping.", signo);
}

int main(int argc, const char *argv[]) {
    USER_LOG(USER_LOG_INFO, "Application started.");

    if(signal(SIGINT, signal_handler) == SIG_ERR) {
        USER_LOG(USER_LOG_FATAL, "Cannot register signal handler.");
        return -1;
    }

    if(user_config_init(&g_config, "config.cfg") != 0) {
        USER_LOG(USER_LOG_FATAL, "Failed to load config file.");
        return -2;
    }

    user_log_level_t log_level = USER_LOG_INFO;
    user_config_lookup_int(&g_config, "agent.common.log_level", (int *)&log_level);
    user_log_set_level(log_level);

    user_lvgl_task_init();
    user_base_task_init();
    user_clock_task_init();

    USER_LOG(USER_LOG_INFO, "Initialized, main thread sleeping.");
    while(g_running) {
        sleep(1);
    }

    user_clock_task_deinit();
    user_base_task_deinit();
    user_lvgl_task_deinit();

    user_config_deinit(&g_config);

    USER_LOG(USER_LOG_INFO, "Application exit.");

    return 0;
}
