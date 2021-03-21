#include <stdio.h>
#include <stdarg.h>

#include <time.h>

#include "utils/user_log_util.h"

void user_log_print(user_log_level_t level, char *fmt, ...) {
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
    
    fprintf(stderr, "[% 9d]%s ", time(NULL), level_str);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    
    va_end(args);
}
