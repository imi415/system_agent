#include "utils/user_log_util.h"

#include "drivers/user_gpio_driver.h"

#define CONSUMER_NAME "SA"

int user_gpio_init(user_gpio_t *gpio, char *chip, uint32_t offset,
                   uint8_t output_enabled) {

    gpio->chip = gpiod_chip_open(chip);
    if(gpio->chip == NULL) {
        USER_LOG(USER_LOG_ERROR, "Failed to open chip %s.", chip);
        return -1;
    }

    gpio->line = gpiod_chip_get_line(gpio->chip, offset);
    if(gpio->line == NULL) {
        USER_LOG(USER_LOG_ERROR, "Failed to get GPIO line %d.", offset);
        gpiod_chip_close(gpio->chip);
        return -1;
    }

    int ret;

    if(output_enabled) {
        ret = gpiod_line_request_output(gpio->line, CONSUMER_NAME, 1);
    } else {
        ret = gpiod_line_request_input(gpio->line, CONSUMER_NAME);
    }

    if(ret != 0) {
        USER_LOG(USER_LOG_ERROR, "Failed to reserve GPIO line.");
        return -1;
    }

    return 0;
}

int user_gpio_set(user_gpio_t *gpio, uint8_t value) {
    int ret = gpiod_line_set_value(gpio->line, value);

    if(ret != 0) {
        USER_LOG(USER_LOG_ERROR, "Failed to set GPIO value.");
        return -1;
    }

    return 0;
}

int user_gpio_setup_intr(user_gpio_t *gpio, user_gpio_intr_t type) {

    int request_type = 0;
    if(type == USER_GPIO_INTR_RISING)
        request_type = GPIOD_LINE_REQUEST_EVENT_RISING_EDGE;
    else if(type == USER_GPIO_INTR_FALLING)
        request_type = GPIOD_LINE_REQUEST_EVENT_FALLING_EDGE;
    else if(type == (USER_GPIO_INTR_RISING | USER_GPIO_INTR_FALLING))
        request_type = GPIOD_LINE_REQUEST_EVENT_BOTH_EDGES;

    const struct gpiod_line_request_config config = {
        .consumer = CONSUMER_NAME,
        .request_type = request_type,
    };

    int ret = gpiod_line_request(gpio->line, &config, 0);
    if(ret != 0) {
        USER_LOG(USER_LOG_ERROR, "Failed to reserve GPIO line for interrupt.");
    }

    return ret;
}

int user_gpio_intr_poll(user_gpio_t *gpio, uint32_t timeout_ms,
                        user_gpio_intr_t *event) {
    struct timespec ts = {
        .tv_sec = timeout_ms / 1000,
        .tv_nsec = (timeout_ms % 1000) * 1000000000,
    };
    struct gpiod_line_event ev;

    int ret = gpiod_line_event_wait(gpio->line, &ts);
    if(ret > 0) {
        ret = gpiod_line_event_read(gpio->line, &ev);
    }

    if(ret < 0) return ret;

    if(ev.event_type == GPIOD_LINE_EVENT_RISING_EDGE) {
        *event = USER_GPIO_INTR_RISING;
    } else {
        *event = USER_GPIO_INTR_FALLING;
    }

    return 0;
}

int user_gpio_get(user_gpio_t *gpio, uint8_t *value) {
    *value = gpiod_line_get_value(gpio->line);

    return 0;
}

int user_gpio_deinit(user_gpio_t *gpio) {

    gpiod_line_release(gpio->line);
    gpiod_chip_close(gpio->chip);

    return 0;
}