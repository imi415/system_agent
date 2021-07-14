#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "drivers/user_config_driver.h"

#include "utils/user_log_util.h"

#include "impl/user_ccs811_impl.h"

extern user_config_t g_config;

int user_ccs811_impl_init(user_ccs811_impl_t *impl) {

    impl->nwake_pin = NULL;
    char *nwake_gpio_path = user_config_lookup_string(&g_config, "agent.devices.tvoc.nwake_pin.path");
    if(nwake_gpio_path == NULL) {
        USER_LOG(USER_LOG_WARN, "Can not find nWAKE pin, assuming tied to low.");
    } else {
        int nwake_gpio_num = 0;
        if(user_config_lookup_int(&g_config, "agent.devices.tvoc.nwake_pin.line", &nwake_gpio_num) != 0) {
            USER_LOG(USER_LOG_ERROR, "Incomplete nWAKE pin configuration: no lines found, bailing out.");
            return -1;
        }

        impl->nwake_pin = malloc(sizeof(user_gpio_t));
        if(impl->nwake_pin == NULL) {
            USER_LOG(USER_LOG_ERROR, "Failed to allocate memory.");
            return -1;
        }
        if(user_gpio_init(impl->nwake_pin, nwake_gpio_path, nwake_gpio_num, 1) != 0) {
            USER_LOG(USER_LOG_ERROR, "Failed to init nWAKE GPIO, path=%s, line=%d.", nwake_gpio_path, nwake_gpio_num);
            return -1;
        }

        if(user_gpio_set(impl->nwake_pin, 0) != 0) {
            USER_LOG(USER_LOG_ERROR, "Failed to set nWAKE value.");
            return -1;
        }
    }

    char *i2cdev_path =
        user_config_lookup_string(&g_config, "agent.devices.tvoc.i2c.path");
    if(i2cdev_path == NULL) return -1;
    int i2c_slaveaddr = 0;
    if(user_config_lookup_int(&g_config, "agent.devices.tvoc.i2c.addr",
                              &i2c_slaveaddr) != 0) {
        USER_LOG(USER_LOG_ERROR, "Failed to lookup I2C device address.");
        return -2;
    }

    if((impl->i2cdev_fd = open(i2cdev_path, O_RDWR)) < 0) {
        USER_LOG(USER_LOG_ERROR, "Failed to open I2C device.");
        return -3;
    }

    USER_LOG(USER_LOG_INFO, "Opened %s for read-write, fd=%d.", i2cdev_path, impl->i2cdev_fd);

    impl->i2c_addr = i2c_slaveaddr & 0xFFU;

    if(ioctl(impl->i2cdev_fd, I2C_SLAVE, impl->i2c_addr) < 0) {
        int err = errno;
        USER_LOG(USER_LOG_ERROR, "Failed to select I2C slave 0x%x, %s.", impl->i2c_addr, strerror(err));
        close(impl->i2cdev_fd);
        return -3;
    }

    return 0;
}

int user_ccs811_impl_deinit(user_ccs811_impl_t *impl) {
    close(impl->i2cdev_fd);
    if(impl->nwake_pin != NULL) {
        user_gpio_deinit(impl->nwake_pin);
        free(impl->nwake_pin);
    }
    return 0;
}

ccs811_ret_t user_ccs811_impl_delay_ms_cb(user_ccs811_impl_t *impl, uint32_t delay_ms) {
    usleep(delay_ms * 1000);
    return CCS_OK;
}

ccs811_ret_t user_ccs811_impl_read_register_cb(user_ccs811_impl_t *impl,
                                               uint8_t reg, uint8_t *data,
                                               uint8_t len) {
    if(write(impl->i2cdev_fd, &reg, 0x01) < 1) {
        USER_LOG(USER_LOG_WARN, "I2C write failed, reg=%d.", reg);
        return CCS_FAIL;
    }

    if(read(impl->i2cdev_fd, data, len) < len) {
        USER_LOG(USER_LOG_WARN, "I2C read failed, reg=%d.", reg);
        return CCS_FAIL;
    }

    return CCS_OK;
}

ccs811_ret_t user_ccs811_impl_write_register_cb(user_ccs811_impl_t *impl,
                                                uint8_t reg, uint8_t *data, uint8_t len) {
    uint8_t *tx_buf = malloc(len + 1);
    if(tx_buf == NULL) {
        USER_LOG(USER_LOG_WARN, "Failed to alloc %d bytes, reg=%d", len, reg);
        return CCS_FAIL;
    }

    tx_buf[0] = reg;
    memcpy(&tx_buf[1], data, len);

    if(write(impl->i2cdev_fd, tx_buf, len + 1) < len + 1) {
        USER_LOG(USER_LOG_WARN, "I2C write failed, reg=%d.", reg);
        return CCS_FAIL;
    }

    return CCS_OK;
}