#include <stdlib.h>
#include <unistd.h>

#include "lvgl.h"

#include "utils/user_log_util.h"

#include "impl/user_st7789_impl.h"

extern user_config_t g_config;
extern user_spi_driver_t g_spi;

int _user_st7789_impl_init_pin(user_gpio_t *gpio, char *pin_name, uint8_t is_output) {
    char *chip;
    uint32_t offset;

    char name_buffer[64];
    snprintf(name_buffer, 64, "agent.devices.lcd.%s_pin.path", pin_name);

    chip = user_config_lookup_string(&g_config, name_buffer);
    if(chip == NULL) return -1;

    snprintf(name_buffer, 64, "agent.devices.lcd.%s_pin.line", pin_name);

    if(user_config_lookup_int(&g_config, name_buffer, &offset) != 0) return -2;

    if(user_gpio_init(gpio, chip, offset, is_output) != 0) return -3;

    return 0;
}

int user_st7789_impl_init(void *handle) {

    user_st7789_impl_t *impl = handle;

    impl->cs_gpio = malloc(sizeof(user_gpio_t));
    if(!impl->cs_gpio) return -1;
    impl->dc_gpio = malloc(sizeof(user_gpio_t));
    if(!impl->dc_gpio) return -1;
    impl->reset_gpio = malloc(sizeof(user_gpio_t));
    if(!impl->reset_gpio) return -1;
    impl->bl_gpio = malloc(sizeof(user_gpio_t));
    if(!impl->bl_gpio) return -1;

    if(_user_st7789_impl_init_pin(impl->cs_gpio, "cs", 1) != 0) {
        free(impl->cs_gpio);
        impl->cs_gpio = NULL;
        USER_LOG(USER_LOG_WARN, "No CS pin found, SPI must be correctly configured.");
    }

    if(_user_st7789_impl_init_pin(impl->dc_gpio, "dc", 1) != 0) {
        USER_LOG(USER_LOG_ERROR, "DC pin not found.");
        return -2;
    }

    if(_user_st7789_impl_init_pin(impl->reset_gpio, "reset", 1) != 0) {
        free(impl->reset_gpio);
        impl->reset_gpio = NULL;
        USER_LOG(USER_LOG_WARN, "No Reset pin found.");
    }

    if(_user_st7789_impl_init_pin(impl->bl_gpio, "backlight", 1) != 0) {
        free(impl->bl_gpio);
        impl->bl_gpio = NULL;
        USER_LOG(USER_LOG_WARN, "No backlight pin found.");
    }

    impl->spi_driver = &g_spi;

    return 0;
}

void user_st7789_impl_deinit(void *handle) {

    user_st7789_impl_t *impl = handle;

    if(impl->cs_gpio) {
        user_gpio_deinit(impl->cs_gpio);
        free(impl->cs_gpio);
    }
    if(impl->dc_gpio) {
        user_gpio_deinit(impl->dc_gpio);
        free(impl->dc_gpio);
    }
    if(impl->reset_gpio) {
        user_gpio_deinit(impl->reset_gpio);
        free(impl->reset_gpio);
    }
    if(impl->bl_gpio) {
        user_gpio_deinit(impl->bl_gpio);
        free(impl->bl_gpio);
    }
}

st7789_ret_t user_st7789_impl_write_cmd(void *handle, uint8_t *cmd,
                                        uint8_t len) {

    user_st7789_impl_t *impl = handle;

    if(impl->dc_gpio && (user_gpio_set(impl->dc_gpio, 0) != 0)) return ST7789_ERROR;
    if(impl->cs_gpio && (user_gpio_set(impl->cs_gpio, 0) != 0)) return ST7789_ERROR;

    if(user_spi_driver_xfer(impl->spi_driver, cmd, NULL, 0x01) != 0) {
        if(impl->cs_gpio) user_gpio_set(impl->cs_gpio, 1);
        return ST7789_ERROR;
    }

    // ST7789VW requires parameters to be sent with DC=1.
    if(len > 1) {
        if(impl->dc_gpio && (user_gpio_set(impl->dc_gpio, 1) != 0)) return ST7789_ERROR;
        if(user_spi_driver_xfer(impl->spi_driver, &cmd[1], NULL, len - 1) != 0) {
            if(impl->cs_gpio) user_gpio_set(impl->cs_gpio, 1);
            return ST7789_ERROR;
        }
    }
    if(impl->cs_gpio && (user_gpio_set(impl->cs_gpio, 1) != 0)) return ST7789_ERROR;

    return ST7789_OK;
}

st7789_ret_t user_st7789_impl_write_data(void *handle, uint8_t *data,
                                         uint32_t len) {
    user_st7789_impl_t *impl = handle;

    if(impl->dc_gpio && (user_gpio_set(impl->dc_gpio, 1) != 0)) return ST7789_ERROR;
    if(impl->cs_gpio && (user_gpio_set(impl->cs_gpio, 0) != 0)) return ST7789_ERROR;
    if(user_spi_driver_xfer(impl->spi_driver, data, NULL, len) != 0) {
        if(impl->cs_gpio) user_gpio_set(impl->cs_gpio, 1);
        return ST7789_ERROR;
    }
    if(impl->cs_gpio && (user_gpio_set(impl->cs_gpio, 1) != 0)) return ST7789_ERROR;

    return ST7789_OK;
}

st7789_ret_t user_st7789_impl_reset(void *handle) {

    user_st7789_impl_t *impl = handle;

    if(impl->reset_gpio == NULL) return ST7789_OK;

    if(user_gpio_set(impl->reset_gpio, 0) != 0) return ST7789_ERROR;
    usleep(1 * 1000); // Sleep 1ms
    if(user_gpio_set(impl->reset_gpio, 1) != 0) return ST7789_ERROR;

    if(impl->cs_gpio == NULL) return ST7789_OK;
    if(user_gpio_set(impl->cs_gpio, 1) != 0) return ST7789_ERROR;

    return ST7789_OK;
}

st7789_ret_t user_st7789_impl_backlight(void *handle, uint8_t on) {
    user_st7789_impl_t *impl = handle;

    if(impl->bl_gpio == NULL) return ST7789_OK;

    if(user_gpio_set(impl->bl_gpio, (on ? 0 : 1)) != 0) return ST7789_ERROR;

    return ST7789_OK;
}