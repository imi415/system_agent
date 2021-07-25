#include <stdio.h>
#include <string.h>
#include <time.h>

#include "impl/user_mqtt_impl.h"

#include "utils/user_log_util.h"

int user_mqtt_impl_init(user_mqtt_impl_t *handle) {
    int mosq_major, mosq_minor, mosq_revision;
    mosquitto_lib_init();

    mosquitto_lib_version(&mosq_major, &mosq_minor, &mosq_revision);
    USER_LOG(USER_LOG_INFO, "libmosquitto version %d.%d rev. %d.", mosq_major, mosq_minor, mosq_revision);

    return 0;
}

int user_mqtt_impl_deinit(user_mqtt_impl_t *handle) {
    mosquitto_lib_cleanup();

    return 0;
}

mqtt_influx_ret_t user_mqtt_get_nsec_timestamp_cb(user_mqtt_impl_t *handle, char *timestamp_string) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    uint64_t ts_int = ts.tv_sec * 1e9 + ts.tv_nsec;
    sprintf(timestamp_string, "%lu", ts_int);
    return MQTT_INFLUX_OK;
}

mqtt_influx_ret_t user_mqtt_publish_message_cb(user_mqtt_impl_t *handle, char *data) {
    USER_LOG(USER_LOG_DEBUG, "MQTT message: %s", data);

    return MQTT_INFLUX_OK;
}