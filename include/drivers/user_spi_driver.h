#ifndef __USER_SPI_DRIVER_H
#define __USER_SPI_DRIVER_H

#include <stdint.h>

typedef enum {
    USER_SPI_OK,
    USER_SPI_ERROR
} user_spi_ret_t;

typedef struct {
    int spidev_fd;
} user_spi_driver_t;

#endif