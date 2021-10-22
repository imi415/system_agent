#include <stdint.h>

#include "user_common.h"
#include "impl/user_bme280_impl.h"
#include "tasks/user_task_lvgl_common.h"
#include "tasks/user_task_mqtt_common.h"

void *user_dht_task(void *arguments);
pthread_t user_dht_task_thread;

int user_dht_task_init(void) {
    int ret;

    ret = pthread_create(&user_dht_task_thread, NULL, user_dht_task, NULL);
    if(ret) return ret;

    pthread_setname_np(user_dht_task_thread, "DHT");

    return 0;
}

int user_dht_task_deinit(void) {
    pthread_join(user_dht_task_thread, NULL);

    return 0;
}

void *user_dht_task(void *arguments) {
    UNUSED(arguments);

    while(g_running && !g_lvgl_ready) {
        sleep(1);
    }

    user_bme280_impl_t bme_impl;
    if(user_bme280_impl_init(&bme_impl) != 0) {
        USER_LOG(USER_LOG_FATAL, "Failed to init BME280 implementation.");
        return NULL;
    }

    bme280_t bme = {
        .cb =
            {
                .read_register_cb =
                    (bme280_ret_t(*)(void *, uint8_t, uint8_t *,
                                     uint8_t))user_bme280_impl_read_register_cb,
                .write_register_cb = (bme280_ret_t(*)(void *, uint8_t, uint8_t))
                    user_bme280_impl_write_register_cb,
                .delay_cb = (bme280_ret_t(*)(
                    void *, uint32_t))user_bme280_impl_delay_cb,
            },
        .user_data = &bme_impl};

    bme280_config_t cfg;

    if(bme280_init(&bme) == BME_FAIL) {
        USER_LOG(USER_LOG_FATAL, "BME280 init failed.");
    }
    bme280_preset_config(&cfg, BME_PRESET_WEATHER);
    bme280_apply_config(&bme, &cfg);

    bme280_result_t res;
    mqtt_influx_measure_t meas;
    mqtt_influx_measure_item_t meas_item[3];
    char temperature_buf[32];
    char humidity_buf[32];
    char pressure_buf[32];

    meas.measurement = "dht";
    meas.first_item = meas_item;

    meas_item[0].key = "temperature";
    meas_item[0].next = &meas_item[1];

    meas_item[1].key = "humidity";
    meas_item[1].next = &meas_item[2];

    meas_item[2].key = "pressure";
    meas_item[2].next = NULL;

    while(g_running) {
        bme280_measure(&bme, &res);
        snprintf(temperature_buf, 32, "%.02f", res.temperature);
        snprintf(humidity_buf, 32, "%.02f", res.humidity);
        snprintf(pressure_buf, 32, "%.02f", res.pressure);
        meas_item[0].value = temperature_buf;
        meas_item[1].value = humidity_buf;
        meas_item[2].value = pressure_buf;
        mqtt_influx_publish_measurement(&g_mqtt_influx, &meas);
        sleep(1);
    }

    return NULL;
}
