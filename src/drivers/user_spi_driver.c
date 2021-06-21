#include <string.h>

#include <errno.h>
#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <linux/types.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "drivers/user_config_driver.h"
#include "utils/user_log_util.h"

#include "drivers/user_spi_driver.h"

int user_spi_driver_init(user_spi_driver_t *spi, char *path,
                                    uint32_t speed_hz) {
    USER_LOG(USER_LOG_INFO, "SPI driver init.");

    spi->spidev_fd = open(path, O_RDWR);
    if(spi->spidev_fd < 0) {
        USER_LOG(USER_LOG_ERROR, "Failed to open spidev device, %s",
                 strerror(errno));

        return -1;
    }

    spi->speed_hz = speed_hz;

    uint32_t mode = SPI_MODE_0;
    if(ioctl(spi->spidev_fd, SPI_IOC_WR_MODE, &mode) == -1) {
        USER_LOG(USER_LOG_ERROR, "Failed to set SPI mode.");
        return -1;
    }

    uint32_t bits = 8;
    if(ioctl(spi->spidev_fd, SPI_IOC_WR_BITS_PER_WORD, &bits) == -1) {
        USER_LOG(USER_LOG_ERROR, "Failed to set SPI bits per word.");
        return -1;
    }

    return 0;
}

int user_spi_driver_deinit(user_spi_driver_t *spi) {
    USER_LOG(USER_LOG_INFO, "SPI driver deinit.");

    close(spi->spidev_fd);

    return 0;
}

int user_spi_driver_xfer(user_spi_driver_t *spi, uint8_t *tx_buf,
                                    uint8_t *rx_buf, uint32_t len) {
    struct spi_ioc_transfer txn = {
        .tx_buf = (unsigned long)tx_buf,
        .rx_buf = (unsigned long)rx_buf,
        .len = len,
        .speed_hz = spi->speed_hz,
        .bits_per_word = 8,
        .tx_nbits = 1,
        .rx_nbits = 1,
    };

    uint32_t xfer_count = (len / SPIDEV_MAX_TRANSFER_SIZE) + ((len % SPIDEV_MAX_TRANSFER_SIZE) ? 1 : 0);
    for(uint32_t i = 0; i < xfer_count; i++) {
        if(txn.tx_buf != 0) txn.tx_buf = (unsigned long)&tx_buf[i * SPIDEV_MAX_TRANSFER_SIZE];
        if(txn.rx_buf != 0) txn.rx_buf = (unsigned long)&rx_buf[i * SPIDEV_MAX_TRANSFER_SIZE];
        if(i == xfer_count - 1) {
            txn.len = len % SPIDEV_MAX_TRANSFER_SIZE;
        } else {
            txn.len = SPIDEV_MAX_TRANSFER_SIZE;
        }

        if(ioctl(spi->spidev_fd, SPI_IOC_MESSAGE(1), &txn) < 0) {
            USER_LOG(USER_LOG_ERROR, "SPI transaction error.");
            return -1;
        }
    }

    return 0;
}
