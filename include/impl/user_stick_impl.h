#ifndef USER_STICK_IMPL_H
#define USER_STICK_IMPL_H

#include "drivers/user_config_driver.h"
#include "drivers/user_gpio_driver.h"

#include "utils/user_log_util.h"

typedef enum {
    USER_STICK_NONE,
    USER_STICK_PUSH,
    USER_STICK_UP,
    USER_STICK_UPLEFT,
    USER_STICK_UPRIGHT,
    USER_STICK_DOWN,
    USER_STICK_DOWNLEFT,
    USER_STICK_DOWNRIGHT,
    USER_STICK_LEFT,
    USER_STICK_RIGHT,
} user_stick_key_t;

typedef struct {
    user_gpio_t *push_gpio;
    user_gpio_t *up_gpio;
    user_gpio_t *down_gpio;
    user_gpio_t *left_gpio;
    user_gpio_t *right_gpio;
} user_stick_impl_t;

int user_stick_impl_init(user_stick_impl_t *stick);
user_stick_key_t user_stick_impl_read(user_stick_impl_t *stick);
int user_stick_impl_deinit(user_stick_impl_t *stick);

#endif