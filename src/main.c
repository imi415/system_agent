#include <stdio.h>
#include <stdint.h>

#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>

#include "drivers/user_config_driver.h"
#include "drivers/user_spi_driver.h"
#include "utils/user_log_util.h"
#include "utils/user_system_util.h"
#include "tasks/user_tasks.h"

uint8_t g_running = 1;

user_config_t g_config;

static void signal_handler(int signo) {
    if(signo == SIGINT || signo == SIGTERM) {
        g_running = 0;
    }
    fprintf(stderr, "\n");
    USER_LOG(USER_LOG_INFO, "Signal [%s] captured, stopping.", strsignal(signo));
}

int main(int argc, const char *argv[]) {
    USER_LOG(USER_LOG_INFO, "Application started.");

    int signal_arr[] = { SIGINT, SIGTERM };
    for(uint8_t i = 0; i < sizeof(signal_arr) / sizeof(int); i++) {
        if(signal(signal_arr[i], signal_handler) == SIG_ERR) {
            USER_LOG(USER_LOG_FATAL, "Cannot register signal handler %s.", strsignal(signal_arr[i]));
            return -1;
        }
    }

    if(user_config_init(&g_config, "config.cfg") != 0) {
        USER_LOG(USER_LOG_FATAL, "Failed to load config file.");
        return -2;
    }

    user_log_level_t log_level = USER_LOG_INFO;
    user_config_lookup_int(&g_config, "agent.common.log_level", (int *)&log_level);
    user_log_set_level(log_level);

    char system_uuid[33] = { 0x00 };
    user_system_get_systemd_unique_id(system_uuid);
    USER_LOG(USER_LOG_INFO, "System UUID: %s", system_uuid);

    int rc;
    if((rc = user_mqtt_task_init()) != 0) {
        USER_LOG(USER_LOG_FATAL, "MQTT task initialization failed(%d).", rc);
        goto mqtt_task_init_failed;
    }
    if((rc = user_lvgl_task_init()) != 0) {
        USER_LOG(USER_LOG_FATAL, "LVGL task initialization failed(%d).", rc);
        goto lvgl_task_init_failed;
    }
    if((rc = user_base_task_init()) != 0) {
        USER_LOG(USER_LOG_FATAL, "BASE task initialization failed(%d).", rc);
        goto base_task_init_failed;
    }
    if((rc = user_clock_task_init()) != 0) {
        USER_LOG(USER_LOG_FATAL, "CLOCK task initialization failed(%d).", rc);
        goto clock_task_init_failed;
    }
    if((rc = user_dht_task_init()) != 0) {
        USER_LOG(USER_LOG_FATAL, "DHT task initialization failed(%d).", rc);
        goto dht_task_init_failed;
    }
    if((rc = user_tvoc_task_init()) != 0) {
        USER_LOG(USER_LOG_FATAL, "TVOC task initialization failed(%d).", rc);
        goto tvoc_task_init_failed;
    }

    USER_LOG(USER_LOG_INFO, "Initialized, main thread now going to sleep.");
    while(g_running) {
        sleep(1);
    }

tvoc_task_init_failed:
    user_tvoc_task_deinit();
dht_task_init_failed:
    user_dht_task_deinit();
clock_task_init_failed:
    user_clock_task_deinit();
base_task_init_failed:
    user_base_task_deinit();
lvgl_task_init_failed:
    user_lvgl_task_deinit();
mqtt_task_init_failed:
    user_mqtt_task_deinit();

    user_config_deinit(&g_config);

    USER_LOG(USER_LOG_INFO, "Application exit.");

    return 0;
}
