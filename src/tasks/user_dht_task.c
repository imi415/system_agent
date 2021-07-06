#include <stdint.h>

#include "impl/user_bme280_impl.h"

#include "tasks/user_task_lvgl_common.h"

void *user_dht_task(void *arguments);
pthread_t user_dht_task_thread;

int user_dht_task_init(void) {
    int ret;

    ret = pthread_create(&user_dht_task_thread, NULL, user_dht_task, NULL);
    if(ret) return ret;
}

int user_dht_task_deinit(void) {
    pthread_join(user_dht_task_thread, NULL);

    return 0;
}

void *user_dht_task(void *arguments) {
    while(g_running && !g_lvgl_ready) {
        sleep(1);
    }

    user_bme280_impl_t bme_impl;
    user_bme280_impl_init(&bme_impl);

    bme280_t bme = {
        .cb =
            {
                .read_register_cb =
                    (bme280_ret_t(*)(void *, uint8_t, uint8_t *,
                                     uint8_t))user_bme280_impl_read_register_cb,
                .write_register_cb = (bme280_ret_t(*)(void *, uint8_t, uint8_t))
                    user_bme280_impl_write_register_cb,
            },
            .user_data = &bme_impl
    };

    bme280_config_t cfg;

    bme280_init(&bme);
    bme280_preset_config(&cfg, BME_PRESET_WEATHER);
    bme280_apply_config(&bme, &cfg);

    bme280_result_t res;

    while(g_running) {
        bme280_measure(&bme, &res);
        USER_LOG(USER_LOG_INFO, "BME: %.02f, %.02f, %.02f", res.temperature, res.humidity, res.pressure);
        sleep(1);
    }
}