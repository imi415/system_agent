#ifndef __USER_LOG_UTIL_H
#define __USER_LOG_UTIL_H

typedef enum {
    USER_LOG_DEBUG = 1,
    USER_LOG_INFO,
    USER_LOG_WARN,
    USER_LOG_ERROR,
    USER_LOG_FATAL
} user_log_level_t;

#define USER_LOG(level, fmt, ...) user_log_print(level, "["__FILE__ ":%d] " fmt, __LINE__, ##__VA_ARGS__)

void user_log_print(user_log_level_t level, char *fmt, ...);

#endif
