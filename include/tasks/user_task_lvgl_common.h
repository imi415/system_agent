#ifndef USER_TASK_LVGL_COMMON_H
#define USER_TASK_LVGL_COMMON_H

#include <unistd.h>
#include <pthread.h>

#include "lvgl.h"

#include "drivers/user_config_driver.h"

#include "utils/user_log_util.h"

extern uint8_t g_running;
extern uint8_t g_lvgl_ready;
extern user_config_t g_config;

extern pthread_mutex_t g_lvgl_mutex;

#endif