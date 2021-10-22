#ifndef MQTT_INFLUX_H
#define MQTT_INFLUX_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    MQTT_INFLUX_OK,
    MQTT_INFLUX_ERROR
} mqtt_influx_ret_t;

typedef struct {
    mqtt_influx_ret_t (*get_nsec_timestamp_cb)(void *handle, char *timestamp_string);
    mqtt_influx_ret_t (*publish_message_cb)(void *handle, char *data);
    bool (*ready_cb)(void *handle);
} mqtt_influx_cb_t;

typedef struct {
    char *key;
    char *value;
    void *next;
} mqtt_influx_measure_item_t;

typedef struct {
    char *measurement;
    mqtt_influx_measure_item_t *first_item;
} mqtt_influx_measure_t;

typedef struct {
    mqtt_influx_cb_t cb;
    void *user_data;
    char hostname[32];
} mqtt_influx_t;

mqtt_influx_ret_t mqtt_influx_init(mqtt_influx_t *influx);
mqtt_influx_ret_t mqtt_influx_publish_measurement(mqtt_influx_t *influx, mqtt_influx_measure_t *meas);

#endif