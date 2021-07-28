#ifndef USER_MQTT_IMPL_H
#define USER_MQTT_IMPL_H

#include <stdint.h>
#include <mosquitto.h>

#include "mqtt_influx.h"

typedef struct {
    struct mosquitto *mosq;
    char topic[32];
} user_mqtt_impl_t;

int user_mqtt_impl_init(user_mqtt_impl_t *handle);
int user_mqtt_impl_deinit(user_mqtt_impl_t *handle);
mqtt_influx_ret_t user_mqtt_get_nsec_timestamp_cb(user_mqtt_impl_t *handle, char *timestamp_string);
mqtt_influx_ret_t user_mqtt_publish_message_cb(user_mqtt_impl_t *handle, char *data);

#endif