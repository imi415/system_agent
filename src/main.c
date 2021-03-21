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
user_spi_driver_t g_spi;

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

    char *spi_path = user_config_lookup_string(&g_config, "agent.drivers.spi.path");
    if(spi_path == NULL) {
        USER_LOG(USER_LOG_ERROR, "Failed to find SPI device from config, using default.");
        spi_path = "/dev/spidev0.0";
    }

    int spi_speed;
    if(user_config_lookup_int(&g_config, "agent.drivers.spi.clock_speed", &spi_speed) != 0) {
        USER_LOG(USER_LOG_ERROR, "Failed to find SPI speed from config, using default.");
        spi_speed = 10000000;
    }

    if(user_spi_driver_init(&g_spi, spi_path, spi_speed) != 0) {
        USER_LOG(USER_LOG_FATAL, "Failed to initialize SPI driver.");
        return -2;
    }

    user_lvgl_task_init();

    USER_LOG(USER_LOG_INFO, "Initialized, main thread sleeping.");
    while(g_running) {
        sleep(1);
    }

    user_lvgl_task_deinit();

    user_spi_driver_deinit(&g_spi);
    user_config_deinit(&g_config);

    USER_LOG(USER_LOG_INFO, "Application exit.");

    return 0;
}
