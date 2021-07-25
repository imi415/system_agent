#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "impl/user_stick_impl.h"

extern user_config_t g_config;

static int user_stick_impl_init_pin(user_gpio_t *gpio, char *pin_name, uint8_t is_output) {
    char *chip;
    uint32_t offset;

    USER_LOG(USER_LOG_DEBUG, "Init GPIO %s", pin_name);

    char name_buffer[64];
    snprintf(name_buffer, 64, "agent.devices.stick.%s.path", pin_name);

    chip = user_config_lookup_string(&g_config, name_buffer);
    if(chip == NULL) return -1;

    snprintf(name_buffer, 64, "agent.devices.stick.%s.line", pin_name);

    if(user_config_lookup_int(&g_config, name_buffer, (int *)&offset) != 0) return -2;

    if(user_gpio_init(gpio, chip, offset, is_output) != 0) return -3;

    return 0;
}

int user_stick_impl_init(user_stick_impl_t *stick) {
    memset(stick, 0x00, sizeof(user_stick_impl_t));

    stick->push_gpio = malloc(sizeof(user_gpio_t));
    if(!stick->push_gpio) goto free_and_exit;
    stick->up_gpio = malloc(sizeof(user_gpio_t));
    if(!stick->up_gpio) goto free_and_exit;
    stick->down_gpio = malloc(sizeof(user_gpio_t));
    if(!stick->down_gpio) goto free_and_exit;
    stick->left_gpio = malloc(sizeof(user_gpio_t));
    if(!stick->left_gpio) goto free_and_exit;
    stick->right_gpio = malloc(sizeof(user_gpio_t));
    if(!stick->right_gpio) goto free_and_exit;

    if(user_stick_impl_init_pin(stick->push_gpio, "push", 0) != 0) {
        free(stick->push_gpio);
        stick->push_gpio = NULL;
        goto deinit_and_exit;
    }

    if(user_stick_impl_init_pin(stick->up_gpio, "up", 0) != 0) {
        free(stick->up_gpio);
        stick->up_gpio = NULL;
        goto deinit_and_exit;
    }

    if(user_stick_impl_init_pin(stick->down_gpio, "down", 0) != 0) {
        free(stick->down_gpio);
        stick->down_gpio = NULL;
        goto deinit_and_exit;
    }

    if(user_stick_impl_init_pin(stick->left_gpio, "left", 0) != 0) {
        free(stick->left_gpio);
        stick->left_gpio = NULL;
        goto deinit_and_exit;
    }

    if(user_stick_impl_init_pin(stick->right_gpio, "right", 0) != 0) {
        free(stick->right_gpio);
        stick->right_gpio = NULL;
        goto deinit_and_exit;
    }

    USER_LOG(USER_LOG_INFO, "Stick initialized successfully.");

    return 0;

deinit_and_exit:
    USER_LOG(USER_LOG_ERROR, "Failed to initialize GPIO.");
    if(stick->push_gpio) user_gpio_deinit(stick->push_gpio);
    if(stick->up_gpio) user_gpio_deinit(stick->up_gpio);
    if(stick->down_gpio) user_gpio_deinit(stick->down_gpio);
    if(stick->left_gpio) user_gpio_deinit(stick->left_gpio);
    if(stick->right_gpio) user_gpio_deinit(stick->right_gpio);

free_and_exit:
    USER_LOG(USER_LOG_ERROR, "Failed to allocate memory.");
    if(stick->push_gpio) free(stick->push_gpio);
    if(stick->up_gpio) free(stick->up_gpio);
    if(stick->down_gpio) free(stick->down_gpio);
    if(stick->left_gpio) free(stick->left_gpio);
    if(stick->right_gpio) free(stick->right_gpio);

    return -1;
}

user_stick_key_t user_stick_impl_read(user_stick_impl_t *stick) {
    uint8_t val[5];
    user_gpio_get(stick->push_gpio, &val[0]);
    user_gpio_get(stick->up_gpio, &val[1]);
    user_gpio_get(stick->down_gpio, &val[2]);
    user_gpio_get(stick->left_gpio, &val[3]);
    user_gpio_get(stick->right_gpio, &val[4]);

    if(val[0]) return USER_STICK_NONE;
    else {
        if(!(val[1] || val[3])) return USER_STICK_UPLEFT;
        if(!(val[1] || val[4])) return USER_STICK_UPRIGHT;
        if(!(val[2] || val[3])) return USER_STICK_DOWNLEFT;
        if(!(val[2] || val[4])) return USER_STICK_DOWNRIGHT;
        if(!val[1]) return USER_STICK_UP;
        if(!val[2]) return USER_STICK_DOWN;
        if(!val[3]) return USER_STICK_LEFT;
        if(!val[4]) return USER_STICK_RIGHT;
    }

    return USER_STICK_PUSH;
}

int user_stick_impl_deinit(user_stick_impl_t *stick) {
    if(stick->push_gpio) {
        user_gpio_deinit(stick->push_gpio);
        free(stick->push_gpio);
    }
    if(stick->up_gpio) {
        user_gpio_deinit(stick->up_gpio);
        free(stick->up_gpio);
    }
    if(stick->down_gpio) {
        user_gpio_deinit(stick->down_gpio);
        free(stick->down_gpio);
    }
    if(stick->left_gpio) {
        user_gpio_deinit(stick->left_gpio);
        free(stick->left_gpio);
    }
    if(stick->right_gpio) {
        user_gpio_deinit(stick->right_gpio);
        free(stick->right_gpio);
    }

    USER_LOG(USER_LOG_INFO, "Stick de-initialized successfully.");

    return 0;
}