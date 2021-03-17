#ifndef __USER_GPIO_DRIVER_H
#define __USER_GPIO_DRIVER_H

#include <stdint.h>

#include <gpiod.h>

typedef enum {
    USER_GPIO_OK,
    USER_GPIO_ERROR,
} user_gpio_ret_t;

typedef struct {
    struct gpiod_chip *chip;
    struct gpiod_line *line;
} user_gpio_t;

user_gpio_ret_t user_gpio_init(user_gpio_t *gpio, char *chip, uint32_t offset);
user_gpio_ret_t user_gpio_set(user_gpio_t *gpio, uint8_t value);
user_gpio_ret_t user_gpio_get(user_gpio_t *gpio, uint8_t *value);
user_gpio_ret_t user_gpio_deinit(user_gpio_t *gpio);

#endif