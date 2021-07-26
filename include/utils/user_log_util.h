#ifndef __USER_LOG_UTIL_H
#define __USER_LOG_UTIL_H

#include <string.h>

typedef enum {
    USER_LOG_FATAL = 1,
    USER_LOG_ERROR,
    USER_LOG_WARN,
    USER_LOG_INFO,
    USER_LOG_DEBUG,
} user_log_level_t;

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define USER_LOG(level, fmt, ...) user_log_print(level, "[% 24s:%04d] " fmt, __FILENAME__, __LINE__, ##__VA_ARGS__)

void user_log_set_level(user_log_level_t level);
void user_log_print(user_log_level_t level, char *fmt, ...);

#endif
