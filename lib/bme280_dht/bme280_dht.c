#include <stdint.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

#include "bme280_dht.h"

bme280_ret_t _bme280_read_trim_data(bme280_t *bme);

double _bme280_compensate_T(bme280_t *bme, int32_t adc_T);
double _bme280_compensate_P(bme280_t *bme, int32_t adc_P);
double _bme280_compensate_H(bme280_t *bme, int32_t adc_H);

bme280_ret_t bme280_init(bme280_t *bme) {
    if(!bme) return BME_FAIL;
    if(_bme280_read_trim_data(bme) == BME_FAIL) return BME_FAIL;
    return BME_OK;
}

bme280_ret_t bme280_preset_config(bme280_config_t *config, bme280_mode_preset_t preset) {
    switch(preset) {
        case BME_PRESET_WEATHER:
        config->mode = BME_MODE_FORCED;
        config->osrs_t = BME_OS_1;
        config->osrs_p = BME_OS_1;
        config->osrs_h = BME_OS_1;
        config->filter = BME_FILTER_OFF;
        break;
        case BME_PRESET_HUMIDITY:
        config->mode = BME_MODE_FORCED;
        config->osrs_t = BME_OS_1;
        config->osrs_p = BME_OS_SKIP;
        config->osrs_h = BME_OS_1;
        config->filter = BME_FILTER_OFF;
        break;
        case BME_PRESET_INDOOR_NAV:
        config->mode = BME_MODE_NORMAL;
        config->osrs_t = BME_OS_2;
        config->osrs_p = BME_OS_16;
        config->osrs_h = BME_OS_1;
        config->filter = BME_FILTER_16;
        break;
        case BME_PRESET_GAMING:
        config->mode = BME_MODE_NORMAL;
        config->osrs_t = BME_OS_1;
        config->osrs_p = BME_OS_4;
        config->osrs_h = BME_OS_SKIP;
        config->filter = BME_FILTER_4;
        break;
    }
    return BME_OK;
}

bme280_ret_t bme280_apply_config(bme280_t *bme, bme280_config_t *config) {
    uint8_t reg_config = config->filter << 0x02;
    uint8_t reg_ctrl_measure = (config->osrs_t << 0x05) | (config->osrs_p << 0x02) | config->mode;
    uint8_t reg_ctrl_hum = config->osrs_h;
    bme->cb.write_register_cb(bme->user_data, 0xF5, reg_config);
    bme->cb.write_register_cb(bme->user_data, 0xF4, reg_ctrl_measure);
    bme->cb.write_register_cb(bme->user_data, 0xF2, reg_ctrl_hum);
    return BME_OK;
}

bme280_ret_t bme280_measure(bme280_t *bme, bme280_result_t *result) {
    uint32_t raw_P = 0x00;
    uint32_t raw_T = 0x00;
    uint32_t raw_H = 0x00;
    uint8_t measure_data[8];
    uint8_t status = 0;
    uint8_t ctrl_meas;
    uint8_t loop_count = 0;
    bme->cb.read_register_cb(bme->user_data, 0xF4, &ctrl_meas, 0x01);
    bme->cb.write_register_cb(bme->user_data, 0xF4, ctrl_meas | BME_MODE_FORCED);
    do {
        bme->cb.read_register_cb(bme->user_data, 0xF3, &status, 0x01);
        loop_count++;
        usleep(100 * 1000);
    } while(status & 0x08 && (loop_count < 12));
    bme->cb.read_register_cb(bme->user_data, 0xF7, measure_data, 0x08);

    raw_P = measure_data[0] << 12 | measure_data[1] << 0x04 | measure_data[2];
    raw_T = measure_data[3] << 12 | measure_data[4] << 0x04 | measure_data[5];
    raw_H = measure_data[6] << 8 | measure_data[7];
    result->temperature = _bme280_compensate_T(bme, raw_T);
    result->pressure = _bme280_compensate_P(bme, raw_P);
    result->humidity = _bme280_compensate_H(bme, raw_H);
    return BME_OK;
}

bme280_ret_t _bme280_read_trim_data(bme280_t *bme) {
    bme280_ret_t ret;
    uint8_t rx_buf[2];
    ret = bme->cb.read_register_cb(bme->user_data, 0x88, rx_buf, 0x02); // T1
    if(ret == BME_FAIL) return ret;
    bme->trim.dig_T1 = rx_buf[1] << 0x08 | rx_buf[0];

    ret = bme->cb.read_register_cb(bme->user_data, 0x8A, rx_buf, 0x02); // T2
    if(ret == BME_FAIL) return ret;
    bme->trim.dig_T2 = rx_buf[1] << 0x08 | rx_buf[0];

    ret = bme->cb.read_register_cb(bme->user_data, 0x8C, rx_buf, 0x02); // T3
    if(ret == BME_FAIL) return ret;
    bme->trim.dig_T3 = rx_buf[1] << 0x08 | rx_buf[0];

    ret = bme->cb.read_register_cb(bme->user_data, 0x8E, rx_buf, 0x02); // P1
    if(ret == BME_FAIL) return ret;
    bme->trim.dig_P1 = rx_buf[1] << 0x08 | rx_buf[0];

    ret = bme->cb.read_register_cb(bme->user_data, 0x90, rx_buf, 0x02); // P2
    if(ret == BME_FAIL) return ret;
    bme->trim.dig_P2 = rx_buf[1] << 0x08 | rx_buf[0];

    ret = bme->cb.read_register_cb(bme->user_data, 0x92, rx_buf, 0x02); // P3
    if(ret == BME_FAIL) return ret;
    bme->trim.dig_P3 = rx_buf[1] << 0x08 | rx_buf[0];

    ret = bme->cb.read_register_cb(bme->user_data, 0x94, rx_buf, 0x02); // P4
    if(ret == BME_FAIL) return ret;
    bme->trim.dig_P4 = rx_buf[1] << 0x08 | rx_buf[0];

    ret = bme->cb.read_register_cb(bme->user_data, 0x96, rx_buf, 0x02); // P5
    if(ret == BME_FAIL) return ret;
    bme->trim.dig_P5 = rx_buf[1] << 0x08 | rx_buf[0];

    ret = bme->cb.read_register_cb(bme->user_data, 0x98, rx_buf, 0x02); // P5
    if(ret == BME_FAIL) return ret;
    bme->trim.dig_P6 = rx_buf[1] << 0x08 | rx_buf[0];

    ret = bme->cb.read_register_cb(bme->user_data, 0x9A, rx_buf, 0x02); // P7
    if(ret == BME_FAIL) return ret;
    bme->trim.dig_P7 = rx_buf[1] << 0x08 | rx_buf[0];

    ret = bme->cb.read_register_cb(bme->user_data, 0x9C, rx_buf, 0x02); // P8
    if(ret == BME_FAIL) return ret;
    bme->trim.dig_P8 = rx_buf[1] << 0x08 | rx_buf[0];

    ret = bme->cb.read_register_cb(bme->user_data, 0x9E, rx_buf, 0x02); // P9
    if(ret == BME_FAIL) return ret;
    bme->trim.dig_P9 = rx_buf[1] << 0x08 | rx_buf[0];

    ret = bme->cb.read_register_cb(bme->user_data, 0xA1, rx_buf, 0x01); // H1
    if(ret == BME_FAIL) return ret;
    bme->trim.dig_H1 = rx_buf[0];

    ret = bme->cb.read_register_cb(bme->user_data, 0xE1, rx_buf, 0x02); // H2
    if(ret == BME_FAIL) return ret;
    bme->trim.dig_H2 = rx_buf[1] << 0x08 | rx_buf[0];

    ret = bme->cb.read_register_cb(bme->user_data, 0xE3, rx_buf, 0x01); // H3
    if(ret == BME_FAIL) return ret;
    bme->trim.dig_H3 = rx_buf[0];

    ret = bme->cb.read_register_cb(bme->user_data, 0xE4, rx_buf, 0x02); // H4
    if(ret == BME_FAIL) return ret;
    bme->trim.dig_H4 = (rx_buf[0] << 0x04) | (rx_buf[1] & 0x0F);

    ret = bme->cb.read_register_cb(bme->user_data, 0xE5, rx_buf, 0x02); // H5
    if(ret == BME_FAIL) return ret;
    bme->trim.dig_H5 = (rx_buf[1] << 0x04) | (rx_buf[0] >> 0x04);

    ret = bme->cb.read_register_cb(bme->user_data, 0xE7, rx_buf, 0x01); // H6
    if(ret == BME_FAIL) return ret;
    bme->trim.dig_H6 = rx_buf[0];
    return BME_OK;
}

double _bme280_compensate_T(bme280_t *bme, int32_t adc_T) {
    double var1, var2, T;
    var1 = (((double)adc_T) / 16384.0 - ((double)bme->trim.dig_T1) / 1024.0) * ((double)bme->trim.dig_T2);
    var2 = ((((double)adc_T) / 131072.0 - ((double)bme->trim.dig_T1) / 8192.0) *
            (((double)adc_T) / 131072.0 - ((double)bme->trim.dig_T1) / 8192.0)) *
           ((double)bme->trim.dig_T3);
    bme->t_fine = (int32_t)(var1 + var2);
    T = (var1 + var2) / 5120.0;
    return T;
}

double _bme280_compensate_P(bme280_t *bme, int32_t adc_P) {
    double var1, var2, p;
    var1 = ((double)bme->t_fine / 2.0) - 64000.0;
    var2 = var1 * var1 * ((double)bme->trim.dig_P6) / 32768.0;
    var2 = var2 + var1 * ((double)bme->trim.dig_P5) * 2.0;
    var2 = (var2 / 4.0) + (((double)bme->trim.dig_P4) * 65536.0);
    var1 = (((double)bme->trim.dig_P3) * var1 * var1 / 524288.0 + ((double)bme->trim.dig_P2) * var1) / 524288.0;
    var1 = (1.0 + var1 / 32768.0) * ((double)bme->trim.dig_P1);
    if (var1 == 0.0)
    {
        return 0; // avoid exception caused by division by zero
    }
    p = 1048576.0 - (double)adc_P;
    p = (p - (var2 / 4096.0)) * 6250.0 / var1;
    var1 = ((double)bme->trim.dig_P9) * p * p / 2147483648.0;
    var2 = p * ((double)bme->trim.dig_P8) / 32768.0;
    p = p + (var1 + var2 + ((double)bme->trim.dig_P7)) / 16.0;
    return p;
}

double _bme280_compensate_H(bme280_t *bme, int32_t adc_H) {
    double var_H;
    var_H = (((double)bme->t_fine) - 76800.0);
    var_H = (adc_H - (((double)bme->trim.dig_H4) * 64.0 + ((double)bme->trim.dig_H5) / 16384.0 *
                                                              var_H)) *
            (((double)bme->trim.dig_H2) / 65536.0 * (1.0 + ((double)bme->trim.dig_H6) / 67108864.0 * var_H * (1.0 + ((double)bme->trim.dig_H3) / 67108864.0 * var_H)));
    var_H = var_H * (1.0 - ((double)bme->trim.dig_H1) * var_H / 524288.0);
    if (var_H > 100.0)
        var_H = 100.0;
    else if (var_H < 0.0)
        var_H = 0.0;
    return var_H;
}
