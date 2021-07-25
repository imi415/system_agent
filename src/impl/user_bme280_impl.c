#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "drivers/user_config_driver.h"

#include "utils/user_log_util.h"

#include "impl/user_bme280_impl.h"

extern user_config_t g_config;

int user_bme280_impl_init(user_bme280_impl_t *impl) {
    char *i2cdev_path =
        user_config_lookup_string(&g_config, "agent.devices.dht.i2c.path");
    if(i2cdev_path == NULL) return -1;
    int i2c_slaveaddr = 0;
    if(user_config_lookup_int(&g_config, "agent.devices.dht.i2c.addr",
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

int user_bme280_impl_deinit(user_bme280_impl_t *impl) {
    close(impl->i2cdev_fd);
    return 0;
}

bme280_ret_t user_bme280_impl_read_register_cb(user_bme280_impl_t *impl,
                                               uint8_t reg, uint8_t *data,
                                               uint8_t len) {
    if(write(impl->i2cdev_fd, &reg, 0x01) < 1) {
        USER_LOG(USER_LOG_WARN, "I2C write failed, reg=%d.", reg);
        return BME_FAIL;
    }

    if(read(impl->i2cdev_fd, data, len) < len) {
        USER_LOG(USER_LOG_WARN, "I2C read failed, reg=%d.", reg);
        return BME_FAIL;
    }

    for(uint8_t i = 0; i < len; i++) {
        USER_LOG(USER_LOG_DEBUG, "I2C read, reg=%x, value=%x.", reg, data[i]);
    }
    return BME_OK;
}

bme280_ret_t user_bme280_impl_write_register_cb(user_bme280_impl_t *impl,
                                                uint8_t reg, uint8_t data) {
    uint8_t tx_buf[2] = {reg, data};
    USER_LOG(USER_LOG_DEBUG, "I2C write, reg=%d, value=%d.", reg, data);
    if(write(impl->i2cdev_fd, tx_buf, 0x02) < 2) {
        USER_LOG(USER_LOG_WARN, "I2C write failed, reg=%d.", reg);
        return BME_FAIL;
    }
    return BME_OK;
}

bme280_ret_t user_bme280_impl_delay_cb(user_bme280_impl_t *impl, uint32_t delay_ms) {
    usleep(delay_ms * 1000);
    return BME_OK;
}