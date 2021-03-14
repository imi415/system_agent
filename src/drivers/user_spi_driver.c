#include <string.h>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include "drivers/user_config_driver.h"
#include "utils/user_log_util.h"

#include "drivers/user_spi_driver.h"

user_spi_ret_t user_spi_driver_init(user_spi_driver_t *spi, char *path) {
    USER_LOG(USER_LOG_INFO, "SPI driver init.");

    spi->spidev_fd = open(path, O_RDWR);
    if(spi->spidev_fd < 0) {
        USER_LOG(USER_LOG_ERROR, "Failed to open spidev device, %s",
                 strerror(errno));

        return USER_SPI_ERROR;
    }

    return USER_SPI_OK;
}

user_spi_ret_t user_spi_driver_deinit(user_spi_driver_t *spi) {
    USER_LOG(USER_LOG_INFO, "SPI driver deinit.");

    close(spi->spidev_fd);
}

user_spi_ret_t user_spi_driver_xfer(user_spi_driver_t *spi, uint8_t *tx_buf,
                                    uint8_t rx_buf, uint32_t len) {
    //
}