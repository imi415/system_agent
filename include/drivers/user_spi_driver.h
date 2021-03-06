#ifndef __USER_SPI_DRIVER_H
#define __USER_SPI_DRIVER_H

#include <stdint.h>

typedef struct {
    int spidev_fd;
    uint32_t speed_hz;
} user_spi_driver_t;

#define SPIDEV_MAX_TRANSFER_SIZE 4096

int user_spi_driver_init(user_spi_driver_t *spi, char *path, uint32_t speed_hz);
int user_spi_driver_deinit(user_spi_driver_t *spi);
int user_spi_driver_xfer(user_spi_driver_t *spi, uint8_t *tx_buf,
                                    uint8_t *rx_buf, uint32_t len);

#endif
