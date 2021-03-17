#include "utils/user_log_util.h"

#include "drivers/user_gpio_driver.h"


user_gpio_ret_t user_gpio_init(user_gpio_t *gpio, char *chip, uint32_t offset) {

    gpio->chip = gpiod_chip_open(chip);
    if(gpio->chip == NULL) {
        USER_LOG(USER_LOG_ERROR, "Failed to open chip %s.", chip);
        return USER_GPIO_ERROR;
    }

    gpio->line = gpiod_chip_get_line(gpio->chip, offset);
    if(gpio->line == NULL) {
        USER_LOG(USER_LOG_ERROR, "Failed to get GPIO line %d.", offset);
        gpiod_chip_close(gpio->chip);
        return USER_GPIO_ERROR;
    }

    return USER_GPIO_OK;
}

user_gpio_ret_t user_gpio_set(user_gpio_t *gpio, uint8_t value) {

    int ret = gpiod_line_request_output(gpio->line, "SA", value);
    if(ret != 0) {
        USER_LOG(USER_LOG_ERROR, "Failed to reserve GPIO line.");
        return USER_GPIO_ERROR;
    }

    ret = gpiod_line_set_value(gpio->line, value);
    if(ret != 0) {
        USER_LOG(USER_LOG_ERROR, "Failed to set GPIO value.");
        return USER_GPIO_ERROR;
    }

    return USER_GPIO_OK;
}

user_gpio_ret_t user_gpio_get(user_gpio_t *gpio, uint8_t *value){
    
    int ret = gpiod_line_request_input(gpio->line, "SA");
    if(ret != 0) {
        USER_LOG(USER_LOG_ERROR, "Failed to reserve GPIO line.");
        return USER_GPIO_ERROR;
    }

    *value = gpiod_line_get_value(gpio->line);

    return USER_GPIO_OK;
}

user_gpio_ret_t user_gpio_deinit(user_gpio_t *gpio) {

    gpiod_line_release(gpio->line);
    gpiod_chip_close(gpio->chip);

    return USER_GPIO_OK;
}