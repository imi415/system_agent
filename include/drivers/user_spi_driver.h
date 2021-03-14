#ifndef __USER_SPI_DRIVER_H
#define __USER_SPI_DRIVER_H

#include <stdint.h>

typedef enum { USER_SPI_OK, USER_SPI_ERROR } user_spi_ret_t;

typedef struct {
    int spidev_fd;
} user_spi_driver_t;

user_spi_ret_t user_spi_driver_init(user_spi_driver_t *spi, char *path);
user_spi_ret_t user_spi_driver_deinit(user_spi_driver_t *spi);
user_spi_ret_t user_spi_driver_xfer(user_spi_driver_t *spi, uint8_t *tx_buf,
                                    uint8_t rx_buf, uint32_t len);

#endif
