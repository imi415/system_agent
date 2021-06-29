#ifndef __USER_GPIO_DRIVER_H
#define __USER_GPIO_DRIVER_H

#include <stdint.h>

#include <gpiod.h>

typedef struct {
    struct gpiod_chip *chip;
    struct gpiod_line *line;
} user_gpio_t;

typedef enum {
    USER_GPIO_INTR_RISING = 0x01,
    USER_GPIO_INTR_FALLING = 0x02,
} user_gpio_intr_t;

int user_gpio_init(user_gpio_t *gpio, char *chip, uint32_t offset,
                   uint8_t output_enabled);
int user_gpio_set(user_gpio_t *gpio, uint8_t value);
int user_gpio_get(user_gpio_t *gpio, uint8_t *value);
int user_gpio_intr_poll(user_gpio_t *gpio, uint32_t timeout_ms,
                        user_gpio_intr_t *event);
int user_gpio_setup_intr(user_gpio_t *gpio, user_gpio_intr_t type);
int user_gpio_deinit(user_gpio_t *gpio);

#endif