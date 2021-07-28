#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#include "user_common.h"
#include "impl/user_mqtt_impl.h"
#include "utils/user_log_util.h"

// TODO: Add callbacks here as static functions.

int user_mqtt_impl_init(user_mqtt_impl_t *handle) {
    int mosq_major, mosq_minor, mosq_revision;
    mosquitto_lib_init();

    mosquitto_lib_version(&mosq_major, &mosq_minor, &mosq_revision);
    USER_LOG(USER_LOG_INFO, "libmosquitto library version %d.%d rev. %d.", mosq_major, mosq_minor, mosq_revision);

    // Init mosquitto instance.
    handle->mosq = mosquitto_new("ID", false, handle);
    if(handle->mosq == NULL) {
        int err = errno;
        USER_LOG(USER_LOG_ERROR, "mosquitto client creation failed, reason: (%s).", strerror(err));
        return -1;
    }

    // Enable multi-thread support.
    mosquitto_threaded_set(handle->mosq, true);

    return 0;
}

int user_mqtt_impl_deinit(user_mqtt_impl_t *handle) {
    mosquitto_destroy(handle->mosq);
    mosquitto_lib_cleanup();

    return 0;
}

int user_mqtt_network_loop(user_mqtt_impl_t *handle) {
    return mosquitto_loop(handle->mosq, 1000, 1);
}

mqtt_influx_ret_t user_mqtt_get_nsec_timestamp_cb(user_mqtt_impl_t *handle, char *timestamp_string) {
    UNUSED(handle);
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    uint64_t ts_int = ts.tv_sec * 1e9 + ts.tv_nsec;
    sprintf(timestamp_string, "%lu", ts_int);
    return MQTT_INFLUX_OK;
}

mqtt_influx_ret_t user_mqtt_publish_message_cb(user_mqtt_impl_t *handle, char *data) {
    USER_LOG(USER_LOG_DEBUG, "MQTT message: %s", data);

    // TODO: Exception handling required.
    mosquitto_publish(handle->mosq, NULL, handle->topic, strlen(data), data, 1, false);

    return MQTT_INFLUX_OK;
}