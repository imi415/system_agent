#include <stdio.h>
#include <stdarg.h>

#include <time.h>
#include <unistd.h>

#include "drivers/user_config_driver.h"

#include "utils/user_log_util.h"

static user_log_level_t s_current_level = USER_LOG_DEBUG;
static int s_lock = 0;

void user_log_set_level(user_log_level_t level) {
    s_current_level = level;
}

void user_log_print(user_log_level_t level, char *fmt, ...) {

    if(level < s_current_level) return;

    while(s_lock) {
        usleep(1000);
    }

    s_lock = 1;

    char *level_str;
    switch(level) {
        case USER_LOG_DEBUG:
        level_str = "[DEBUG]";
        break;
        case USER_LOG_INFO:
        level_str = "[ INFO]";
        break;
        case USER_LOG_WARN:
        level_str = "[ WARN]";
        break;
        case USER_LOG_ERROR:
        level_str = "[ERROR]";
        break;
        case USER_LOG_FATAL:
        level_str = "[FATAL]";
        break;
        default:
        level_str = "[UNKNOWN]";
        break;
    }

    va_list args;
    va_start(args, fmt);
    
    fprintf(stderr, "[% 9ld]%s", time(NULL), level_str);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    fflush(stderr);
    
    va_end(args);

    s_lock = 0;
}
