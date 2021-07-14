#include "ccs811_tvoc.h"

ccs811_ret_t _ccs811_sw_reset(ccs811_t *ccs);
ccs811_ret_t _ccs811_app_start(ccs811_t *ccs);
ccs811_ret_t _ccs811_set_mode(ccs811_t *ccs, uint8_t mode);


ccs811_ret_t ccs811_init(ccs811_t *ccs, uint8_t addr, void *arg) {
    if(_ccs811_sw_reset(ccs) == CCS_FAIL) return CCS_FAIL;
    ccs->cb.delay_ms_cb(ccs->user_data, 5);
    if(_ccs811_app_start(ccs) == CCS_FAIL) return CCS_FAIL;
    ccs->cb.delay_ms_cb(ccs->user_data, 5);
    if(_ccs811_set_mode(ccs, 0x01) == CCS_FAIL) return CCS_FAIL;
    return CCS_OK;
}

ccs811_ret_t ccs811_set_env_data(ccs811_t *ccs, double temperature, double humidity) {
    uint16_t temp_in_512 = (uint16_t)((temperature + 25.0) * 512.0);
    uint16_t humid_in_512 = (uint16_t)(humidity * 512.0);
    uint8_t env_data[4] = {humid_in_512 >> 0x08, humid_in_512 & 0xFF, temp_in_512 >> 0x08, temp_in_512 & 0xFF};
    return ccs->cb.write_register_cb(ccs->user_data, 0x05, env_data, 0x04);
}

ccs811_ret_t ccs811_read_baseline(ccs811_t *ccs, uint8_t *baseline) {
    ccs->cb.read_register_cb(ccs->user_data, 0x11, baseline, 0x02);
    return CCS_OK;
}

ccs811_ret_t ccs811_store_baseline(ccs811_t *ccs, uint8_t *baseline) {
    ccs->cb.write_register_cb(ccs->user_data, 0x11, baseline, 0x02);
    return CCS_OK;
}

ccs811_ret_t ccs811_measure(ccs811_t *ccs, ccs811_result_t *result) {
    uint8_t alg_result[8];
    uint8_t loop_count = 0;
    do {
        ccs->cb.read_register_cb(ccs->user_data, 0x02, alg_result, 0x08);
        ccs->cb.delay_ms_cb(ccs->user_data, 100);
        loop_count++;
    } while(((alg_result[4] & 0x08) == 0) && (loop_count < 12));
    result->eco2 = alg_result[0] << 0x08 | alg_result[1];
    result->tvoc = alg_result[2] << 0x08 | alg_result[3];
    return CCS_OK;
}

ccs811_ret_t _ccs811_sw_reset(ccs811_t *ccs) {
    uint8_t magic[4] = {0x11, 0xE5, 0x72, 0x8A};
    return ccs->cb.write_register_cb(ccs->user_data, 0xFF, magic, 0x04);
}

ccs811_ret_t _ccs811_app_start(ccs811_t *ccs) {
    uint8_t status;
    if(ccs->cb.read_register_cb(ccs->user_data, 0x00, &status, 0x01) == CCS_FAIL) {
        return CCS_FAIL;
    }
    if(status & 0x10 == 0) {
        return CCS_FAIL;
    }
    return ccs->cb.write_register_cb(ccs->user_data, 0xF4, (uint8_t *)0x00, 0x00);
}

ccs811_ret_t _ccs811_set_mode(ccs811_t *ccs, uint8_t mode) {
    uint8_t drive_mode = mode << 0x04;
    return ccs->cb.write_register_cb(ccs->user_data, 0x01, &drive_mode, 0x01);
}
