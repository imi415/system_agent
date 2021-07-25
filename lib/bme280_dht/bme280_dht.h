#ifndef BME280_DHT_H
#define BME280_DHT_H

#include <stdint.h>

typedef int bme280_ret_t;

#define BME_OK 0
#define BME_FAIL -1

#define BME_MODE_SLEEP  0x00
#define BME_MODE_FORCED 0x01
#define BME_MODE_NORMAL 0x03

#define BME_OS_SKIP 0x00
#define BME_OS_1    0x01
#define BME_OS_2    0x02
#define BME_OS_4    0x03
#define BME_OS_8    0x04
#define BME_OS_16   0x05

#define BME_FILTER_OFF 0x00
#define BME_FILTER_2   0x01
#define BME_FILTER_4   0x02
#define BME_FILTER_8   0x03
#define BME_FILTER_16  0x04

typedef enum {
    BME_PRESET_WEATHER,
    BME_PRESET_HUMIDITY,
    BME_PRESET_INDOOR_NAV,
    BME_PRESET_GAMING
} bme280_mode_preset_t;

typedef struct {
    bme280_ret_t (*read_register_cb)(void *handle, uint8_t reg, uint8_t *data, uint8_t len);
    bme280_ret_t (*write_register_cb)(void *handle, uint8_t reg, uint8_t data);
    bme280_ret_t (*delay_cb)(void *handle, uint32_t delay_ms);
} bme280_cb_t;

typedef struct {
    uint16_t dig_T1;
    int16_t dig_T2;
    int16_t dig_T3;
    uint16_t dig_P1;
    int16_t dig_P2;
    int16_t dig_P3;
    int16_t dig_P4;
    int16_t dig_P5;
    int16_t dig_P6;
    int16_t dig_P7;
    int16_t dig_P8;
    int16_t dig_P9;
    uint8_t dig_H1;
    int16_t dig_H2;
    uint8_t dig_H3;
    int16_t dig_H4;
    int16_t dig_H5;
    int8_t dig_H6;
} bme280_trim_t;

typedef struct {
    uint8_t mode;
    uint8_t osrs_t;
    uint8_t osrs_p;
    uint8_t osrs_h;
    uint8_t t_sb;
    uint8_t filter;
} bme280_config_t;

typedef struct {
    double temperature;
    double pressure;
    double humidity;
} bme280_result_t;

typedef struct {
    void *user_data;
    bme280_trim_t trim;
    bme280_cb_t cb;
    int32_t t_fine;
} bme280_t;

bme280_ret_t bme280_init(bme280_t *bme);
bme280_ret_t bme280_preset_config(bme280_config_t *config, bme280_mode_preset_t preset);
bme280_ret_t bme280_apply_config(bme280_t *bme, bme280_config_t *config);
bme280_ret_t bme280_measure(bme280_t *bme, bme280_result_t *result);

#endif